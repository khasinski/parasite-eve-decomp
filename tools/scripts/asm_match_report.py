#!/usr/bin/env python3
"""Summarize target-vs-candidate MIPS codegen differences.

This is intentionally lighter than objdiff/asm-differ. It answers the questions
that come up during source-shape debugging: frame shape, first structural drift,
branch-delay slots, relocation access forms, and symbol addressing classes.
"""

from __future__ import annotations

import argparse
import re
import subprocess
import sys
import tempfile
from collections import Counter
from dataclasses import dataclass, field
from pathlib import Path
from typing import Iterable


BRANCH_OPS = {
    "beq",
    "beqz",
    "bne",
    "bnez",
    "bgez",
    "bgtz",
    "blez",
    "bltz",
    "j",
    "jal",
    "jr",
}


@dataclass
class Reloc:
    kind: str
    symbol: str


@dataclass
class Insn:
    offset: int
    word: str
    op: str
    operands: str
    raw: str
    relocs: list[Reloc] = field(default_factory=list)

    @property
    def norm(self) -> str:
        op, operands = normalize_op_operands(self.op, self.operands)
        return f"{op} {operands}".strip()

    @property
    def sym_norm(self) -> str:
        op, operands = normalize_symbolic(self)
        return f"{op} {operands}".strip()

    @property
    def shape_norm(self) -> str:
        text = self.sym_norm
        text = re.sub(r"addiu sp,sp,-?(?:0x)?[0-9A-Fa-f]+", "addiu sp,sp,<frame>", text)
        text = re.sub(r"addiu sp,sp,-?[0-9]+", "addiu sp,sp,<frame>", text)
        text = re.sub(r"([sl]w [a-z0-9]+,)-?(?:0x)?[0-9A-Fa-f]+\(sp\)", r"\1<sp>(sp)", text)
        return text


def parse_int(text: str) -> int:
    text = text.strip()
    return int(text, 16) if text.lower().startswith("0x") else int(text, 0)


def split_operands(operands: str) -> list[str]:
    if not operands:
        return []
    return [part.strip() for part in operands.split(",")]


def normalize_mem(text: str) -> str:
    text = text.replace("$", "")
    text = re.sub(r"\s+", "", text)
    text = text.replace("0x0(", "0(")
    text = re.sub(
        r"\((0x[0-9A-Fa-f]+)>>16\)",
        lambda m: str(int(m.group(1), 16) >> 16),
        text,
    )
    text = re.sub(
        r"(?<![A-Za-z_])(-?0x[0-9A-Fa-f]+)(?![A-Za-z_])",
        lambda m: str(int(m.group(1), 16)),
        text,
    )
    return text


def normalize_op_operands(op: str, operands: str) -> tuple[str, str]:
    op = op.strip()
    ops = [normalize_mem(part) for part in split_operands(operands)]

    if op == "nop":
        return "nop", ""
    if op == "sll" and ops == ["zero", "zero", "0"]:
        return "nop", ""
    if op == "move" and len(ops) == 2:
        return "addu", f"{ops[0]},{ops[1]},zero"
    if op == "li" and len(ops) == 2:
        return "addiu", f"{ops[0]},zero,{ops[1]}"
    if op == "beqz" and len(ops) == 2:
        return "beq", f"{ops[0]},zero,{ops[1]}"
    if op == "bnez" and len(ops) == 2:
        return "bne", f"{ops[0]},zero,{ops[1]}"
    if op in {"beq", "bne"} and len(ops) == 3:
        ops[2] = "<target>"
        return op, ",".join(ops)
    if op in {"bgez", "bgtz", "blez", "bltz"} and len(ops) == 2:
        ops[1] = "<target>"
        return op, ",".join(ops)
    if op == "j" and len(ops) == 1 and not ops[0].startswith("func_"):
        return op, "<target>"
    if op == "addu" and len(ops) == 3 and ops[1] == "zero" and ops[2] == "zero":
        return "addu", f"{ops[0]},zero,zero"
    return op, ",".join(ops)


def normalize_symbolic(insn: Insn) -> tuple[str, str]:
    op, operands = normalize_op_operands(insn.op, insn.operands)
    ops = split_operands(operands)

    relocs = insn.relocs
    if relocs:
        rel = relocs[-1]
        if rel.kind == "R_MIPS_26":
            if op == "j" and rel.symbol.startswith("."):
                return op, "<target>"
            return op, rel.symbol
        if rel.kind == "R_MIPS_GPREL16" and ops:
            if len(ops) == 2:
                ops[1] = f"%gp_rel({rel.symbol})(gp)"
            else:
                ops[-1] = f"%gp_rel({rel.symbol})(gp)"
            return op, ",".join(ops)
        if rel.kind == "R_MIPS_HI16" and ops:
            ops[-1] = f"%hi({rel.symbol})"
            return op, ",".join(ops)
        if rel.kind == "R_MIPS_LO16" and ops:
            if len(ops) >= 2 and "(" in ops[-1]:
                base = ops[-1].split("(", 1)[1]
                ops[-1] = f"%lo({rel.symbol})({base}"
            else:
                ops[-1] = f"%lo({rel.symbol})"
            return op, ",".join(ops)

    operands = operands.replace("$", "")
    operands = re.sub(r"%gp_rel\(([^)]+)\)\(gp\)", r"%gp_rel(\1)(gp)", operands)
    operands = re.sub(r"\s+", "", operands)
    return normalize_op_operands(op, operands)


def parse_target_asm(path: Path, function: str) -> list[Insn]:
    insns: list[Insn] = []
    in_func = False
    pat = re.compile(
        r"/\*\s+([0-9A-Fa-f]+)\s+[0-9A-Fa-f]+\s+([0-9A-Fa-f]{8})\s+\*/\s+"
        r"([A-Za-z0-9_.]+)\s*(.*)$"
    )
    for line in path.read_text().splitlines():
        if re.search(rf"\bglabel\s+{re.escape(function)}\b", line):
            in_func = True
            continue
        if in_func and re.search(rf"\bendlabel\s+{re.escape(function)}\b", line):
            break
        if not in_func:
            continue
        m = pat.search(line)
        if not m:
            continue
        off, word, op, operands = m.groups()
        # spimdisasm comments show big-endian word order in this repo; objdump
        # prints little-endian object words. Reverse target bytes for raw-byte
        # comparisons.
        rev_word = "".join([word[i : i + 2] for i in range(0, 8, 2)][::-1]).lower()
        insns.append(Insn(parse_int("0x" + off), rev_word, op, operands.strip(), line.rstrip()))
    if not insns:
        sys.exit(f"no target instructions found for {function} in {path}")
    base = insns[0].offset
    for insn in insns:
        insn.offset -= base
    return insns


def objdump_text(path: Path) -> str:
    if path.suffix == ".o":
        with tempfile.NamedTemporaryFile("w+", suffix=".dump") as tmp:
            subprocess.run(
                ["mipsel-none-elf-objdump", "-dr", "-M", "no-aliases", str(path)],
                check=True,
                stdout=tmp,
                text=True,
            )
            tmp.seek(0)
            return tmp.read()
    return path.read_text()


def parse_objdump(path: Path, function: str | None) -> list[Insn]:
    text = objdump_text(path)
    insns: list[Insn] = []
    current: Insn | None = None
    in_func = function is None
    func_pat = re.compile(r"^[0-9a-fA-F]+\s+<([^>]+)>:")
    insn_pat = re.compile(r"^\s*([0-9a-fA-F]+):\s*([0-9a-fA-F]{8})\s+([A-Za-z0-9_.]+)\s*(.*)$")
    reloc_pat = re.compile(r"^\s*[0-9a-fA-F]+:\s+(R_MIPS_[A-Z0-9_]+)\s+(.+?)\s*$")

    for line in text.splitlines():
        fm = func_pat.match(line)
        if fm:
            name = fm.group(1)
            if function is None:
                in_func = True
            elif name == function:
                in_func = True
            elif in_func and (name.startswith("LM") or name.startswith(".L")):
                pass
            else:
                in_func = False
            current = None
            continue
        if not in_func:
            continue
        im = insn_pat.match(line)
        if im:
            off, word, op, operands = im.groups()
            current = Insn(parse_int("0x" + off), word.lower(), op, operands.strip(), line.rstrip())
            insns.append(current)
            continue
        rm = reloc_pat.match(line)
        if rm and current is not None:
            current.relocs.append(Reloc(rm.group(1), rm.group(2).strip()))

    if not insns:
        sys.exit(f"no candidate instructions found in {path}")
    base = insns[0].offset
    for insn in insns:
        insn.offset -= base
    return insns


def frame_summary(insns: list[Insn]) -> dict[str, object]:
    frame = None
    saves: list[str] = []
    for insn in insns[:20]:
        op, operands = normalize_op_operands(insn.op, insn.operands)
        ops = split_operands(operands)
        if op == "addiu" and len(ops) == 3 and ops[0] == "sp" and ops[1] == "sp":
            try:
                imm = parse_int(ops[2])
            except ValueError:
                continue
            if imm < 0:
                frame = -imm
        if op == "sw" and len(ops) == 2 and "(sp)" in ops[1]:
            saves.append(f"{ops[0]}@{ops[1].split('(')[0]}")
    return {"frame": frame, "saves": saves}


def branch_delays(insns: list[Insn]) -> list[tuple[int, Insn, Insn | None]]:
    out = []
    for i, insn in enumerate(insns):
        op, _ = normalize_op_operands(insn.op, insn.operands)
        if op in BRANCH_OPS:
            out.append((i, insn, insns[i + 1] if i + 1 < len(insns) else None))
    return out


def target_symbol_forms(insns: list[Insn]) -> Counter[tuple[str, str]]:
    forms: Counter[tuple[str, str]] = Counter()
    for i, insn in enumerate(insns):
        text = insn.operands
        for sym in re.findall(r"%gp_rel\(([^)]+)\)", text):
            forms[(sym, "gp-rel")] += 1
        hi_syms = re.findall(r"%hi\(([^)]+)\)", text)
        for sym in hi_syms:
            form = "hi"
            if i + 1 < len(insns) and f"%lo({sym})" in insns[i + 1].operands:
                next_op, _ = normalize_op_operands(insns[i + 1].op, insns[i + 1].operands)
                form = "materialized" if next_op == "addiu" else "direct-hi-lo"
            elif any(f"%lo({sym})" in later.operands for later in insns[i + 1 : i + 4]):
                form = "direct-hi-lo"
            forms[(sym, form)] += 1
    return forms


def candidate_symbol_forms(insns: list[Insn]) -> Counter[tuple[str, str]]:
    forms: Counter[tuple[str, str]] = Counter()
    for i, insn in enumerate(insns):
        for rel in insn.relocs:
            if rel.kind == "R_MIPS_GPREL16":
                forms[(rel.symbol, "gp-rel")] += 1
            elif rel.kind == "R_MIPS_HI16":
                form = "hi"
                if i + 1 < len(insns):
                    next_kinds = {(r.kind, r.symbol) for r in insns[i + 1].relocs}
                    next_op, _ = normalize_op_operands(insns[i + 1].op, insns[i + 1].operands)
                    if ("R_MIPS_LO16", rel.symbol) in next_kinds:
                        form = "materialized" if next_op == "addiu" else "direct-hi-lo"
                if form == "hi":
                    for later in insns[i + 1 : i + 4]:
                        if any(r.kind == "R_MIPS_LO16" and r.symbol == rel.symbol for r in later.relocs):
                            form = "direct-hi-lo"
                            break
                forms[(rel.symbol, form)] += 1
    return forms


def op_hist(insns: Iterable[Insn]) -> Counter[str]:
    return Counter(normalize_op_operands(insn.op, insn.operands)[0] for insn in insns)


def lcs_len(a: list[str], b: list[str]) -> int:
    prev = [0] * (len(b) + 1)
    for x in a:
        cur = [0]
        for j, y in enumerate(b, 1):
            cur.append(prev[j - 1] + 1 if x == y else max(prev[j], cur[-1]))
        prev = cur
    return prev[-1]


def first_diff(target: list[Insn], cand: list[Insn], mode: str = "symbolic") -> int:
    n = min(len(target), len(cand))
    for i in range(n):
        if mode == "shape":
            t = target[i].shape_norm
            c = cand[i].shape_norm
        elif mode == "symbolic":
            t = target[i].sym_norm
            c = cand[i].sym_norm
        else:
            t = target[i].norm
            c = cand[i].norm
        if t != c:
            return i
    return n


def print_insn(label: str, i: int, insn: Insn | None) -> None:
    if insn is None:
        print(f"{label} {i:03d}: <none>")
        return
    reloc = ""
    if insn.relocs:
        reloc = "  [" + ", ".join(f"{r.kind}:{r.symbol}" for r in insn.relocs) + "]"
    print(f"{label} {i:03d} +0x{insn.offset:04X}: {insn.sym_norm}{reloc}")


def print_counter_delta(title: str, target: Counter, cand: Counter, limit: int = 20) -> None:
    print(f"\n{title}:")
    keys = sorted(set(target) | set(cand), key=lambda k: (str(k)))
    rows = [(k, target[k], cand[k]) for k in keys if target[k] != cand[k]]
    if not rows:
        print("  no differences")
        return
    for k, tv, cv in rows[:limit]:
        print(f"  {k}: target={tv} candidate={cv}")
    if len(rows) > limit:
        print(f"  ... {len(rows) - limit} more")


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--asm", required=True, type=Path, help="Target asm file")
    parser.add_argument("--function", required=True, help="Function label to extract")
    parser.add_argument("--candidate", required=True, type=Path, help="Candidate .o or objdump text")
    parser.add_argument("--candidate-function", help="Candidate function name if objdump has multiple functions")
    parser.add_argument("--context", type=int, default=6, help="Instruction context around first diff")
    args = parser.parse_args()

    target = parse_target_asm(args.asm, args.function)
    cand = parse_objdump(args.candidate, args.candidate_function or args.function)

    t_norm = [insn.sym_norm for insn in target]
    c_norm = [insn.sym_norm for insn in cand]
    fd = first_diff(target, cand)
    shape_fd = first_diff(target, cand, mode="shape")
    lcs = lcs_len(t_norm, c_norm)
    word_matches = sum(1 for t, c in zip(target, cand) if t.word == c.word)

    print(f"Function: {args.function}")
    print(f"Target instructions:    {len(target)}")
    print(f"Candidate instructions: {len(cand)}")
    print(f"Same-position normalized matches: {sum(1 for t, c in zip(t_norm, c_norm) if t == c)}")
    print(f"LCS normalized matches: {lcs}")
    print(f"First shape diff ignoring frame offsets: {shape_fd if shape_fd < min(len(target), len(cand)) else 'none'}")
    print(f"Same-position raw word matches: {word_matches}")

    print("\nFrame:")
    print(f"  target:    {frame_summary(target)}")
    print(f"  candidate: {frame_summary(cand)}")

    print("\nFirst structural diff:")
    if fd == min(len(target), len(cand)) and len(target) == len(cand):
        print("  none")
    else:
        start = max(0, fd - args.context)
        end = min(max(len(target), len(cand)), fd + args.context + 1)
        for i in range(start, end):
            print_insn("  T", i, target[i] if i < len(target) else None)
            print_insn("  C", i, cand[i] if i < len(cand) else None)

    print("\nFirst shape diff ignoring frame offsets:")
    if shape_fd == min(len(target), len(cand)) and len(target) == len(cand):
        print("  none")
    else:
        start = max(0, shape_fd - args.context)
        end = min(max(len(target), len(cand)), shape_fd + args.context + 1)
        for i in range(start, end):
            print_insn("  T", i, target[i] if i < len(target) else None)
            print_insn("  C", i, cand[i] if i < len(cand) else None)

    print_counter_delta("Opcode histogram deltas", op_hist(target), op_hist(cand))
    print_counter_delta("Symbol access-form deltas", target_symbol_forms(target), candidate_symbol_forms(cand), limit=40)

    t_br = branch_delays(target)
    c_br = branch_delays(cand)
    print("\nBranch delay summary:")
    print(f"  target branches/calls:    {len(t_br)}")
    print(f"  candidate branches/calls: {len(c_br)}")
    for idx, (tb, cb) in enumerate(zip(t_br, c_br)):
        _, t_insn, t_delay = tb
        _, c_insn, c_delay = cb
        t_delay_norm = t_delay.shape_norm if t_delay is not None else None
        c_delay_norm = c_delay.shape_norm if c_delay is not None else None
        if t_insn.shape_norm != c_insn.shape_norm or t_delay_norm != c_delay_norm:
            print(f"  first branch-delay delta at branch #{idx}:")
            print_insn("    T branch", tb[0], t_insn)
            print_insn("    T delay ", tb[0] + 1, t_delay)
            print_insn("    C branch", cb[0], c_insn)
            print_insn("    C delay ", cb[0] + 1, c_delay)
            break
    else:
        if len(t_br) != len(c_br):
            print("  branch counts differ")
        else:
            print("  no branch-delay deltas in branch order")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
