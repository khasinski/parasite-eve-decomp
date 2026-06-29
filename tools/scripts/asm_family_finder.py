#!/usr/bin/env python3
"""Find asm-only function families with the same normalized instruction shape.

This is a batch-candidate discovery tool. It scans spimdisasm `.s` files,
extracts `glabel` functions, normalizes symbols/immediates, and groups functions
that differ mainly by data symbol, call target, or constant.
"""

from __future__ import annotations

import argparse
import csv
import hashlib
import re
import sys
from collections import Counter, defaultdict
from dataclasses import dataclass, field
from pathlib import Path
from typing import Iterable


INSN_RE = re.compile(
    r"/\*\s+([0-9A-Fa-f]+)\s+([0-9A-Fa-f]+)\s+([0-9A-Fa-f]{8})\s+\*/\s+"
    r"([A-Za-z0-9_.]+)\s*(.*)$"
)
GLABEL_RE = re.compile(r"^\s*glabel\s+([A-Za-z_.$][A-Za-z0-9_.$]*)\b")
ENDLABEL_RE = re.compile(r"^\s*endlabel\s+([A-Za-z_.$][A-Za-z0-9_.$]*)\b")

REGISTER_RE = re.compile(
    r"\$(?:zero|at|v[01]|a[0-3]|t[0-9]|s[0-8]|k[01]|gp|sp|fp|ra|[0-9]+)\b"
)
SYMBOL_RE = re.compile(r"\b(?:D|jtbl|func|glabel|RO|BSS)_[A-Za-z0-9_.$]+\b")
HEX_RE = re.compile(r"(?<![A-Za-z_.$])-?0x[0-9A-Fa-f]+(?![A-Za-z_.$])")
DEC_RE = re.compile(r"(?<![A-Za-z_.$])-?\d+(?![A-Za-z_.$])")

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
}
CALL_OPS = {"jal", "jalr"}


@dataclass
class Insn:
    offset: int
    vram: int
    word: str
    op: str
    operands: str


@dataclass
class Function:
    name: str
    path: Path
    start_offset: int
    start_vram: int
    insns: list[Insn] = field(default_factory=list)

    @property
    def size_bytes(self) -> int:
        return len(self.insns) * 4


def parse_int(text: str) -> int:
    return int(text, 16) if text.lower().startswith("0x") else int(text, 0)


def split_operands(operands: str) -> list[str]:
    if not operands.strip():
        return []
    return [part.strip() for part in operands.split(",")]


def normalize_alias(op: str, operands: str) -> tuple[str, str]:
    ops = [part.replace("$", "").replace(" ", "") for part in split_operands(operands)]
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
    return op, ",".join(ops)


def normalize_operands(op: str, operands: str, ignore_registers: bool) -> str:
    op, operands = normalize_alias(op, operands)
    operands = operands.replace(" ", "")
    operands = re.sub(r"%hi\([^)]+\)", "%hi(<sym>)", operands)
    operands = re.sub(r"%lo\([^)]+\)", "%lo(<sym>)", operands)
    operands = re.sub(r"%gp_rel\([^)]+\)", "%gp_rel(<sym>)", operands)
    operands = SYMBOL_RE.sub("<sym>", operands)
    if op in BRANCH_OPS | CALL_OPS:
        parts = split_operands(operands)
        if parts:
            if op in {"j", "jal"}:
                parts[-1] = "<target>"
            elif op != "jalr":
                parts[-1] = "<target>"
            operands = ",".join(parts)
    operands = HEX_RE.sub("<imm>", operands)
    operands = DEC_RE.sub("<imm>", operands)
    if ignore_registers:
        operands = re.sub(r"\b(?:zero|at|v[01]|a[0-3]|t[0-9]|s[0-8]|k[01]|gp|sp|fp|ra|[0-9]+)\b", "<reg>", operands)
        operands = REGISTER_RE.sub("<reg>", operands)
    return f"{op} {operands}".strip()


def function_signature(func: Function, ignore_registers: bool) -> str:
    return "\n".join(normalize_operands(insn.op, insn.operands, ignore_registers) for insn in func.insns)


def short_hash(text: str) -> str:
    return hashlib.sha1(text.encode()).hexdigest()[:12]


def symbols_in(func: Function) -> list[str]:
    seen: set[str] = set()
    out: list[str] = []
    for insn in func.insns:
        for sym in SYMBOL_RE.findall(insn.operands):
            if sym not in seen:
                seen.add(sym)
                out.append(sym)
    return out


def callees_in(func: Function) -> list[str]:
    seen: set[str] = set()
    out: list[str] = []
    for insn in func.insns:
        op, operands = normalize_alias(insn.op, insn.operands)
        if op not in CALL_OPS:
            continue
        for sym in SYMBOL_RE.findall(operands):
            if sym.startswith("func_") and sym not in seen:
                seen.add(sym)
                out.append(sym)
    return out


def parse_functions(path: Path) -> list[Function]:
    funcs: list[Function] = []
    current: Function | None = None

    for line in path.read_text(errors="replace").splitlines():
        gm = GLABEL_RE.match(line)
        if gm:
            if current is not None and current.insns:
                funcs.append(current)
            current = Function(gm.group(1), path, 0, 0)
            continue

        em = ENDLABEL_RE.match(line)
        if em and current is not None and em.group(1) == current.name:
            if current.insns:
                funcs.append(current)
            current = None
            continue

        if current is None:
            continue
        im = INSN_RE.search(line)
        if not im:
            continue
        off, vram, word, op, operands = im.groups()
        insn = Insn(parse_int("0x" + off), parse_int("0x" + vram), word.lower(), op, operands.strip())
        if not current.insns:
            current.start_offset = insn.offset
            current.start_vram = insn.vram
        current.insns.append(insn)

    if current is not None and current.insns:
        funcs.append(current)
    return funcs


def iter_asm_files(paths: Iterable[Path]) -> list[Path]:
    files: list[Path] = []
    for path in paths:
        if path.is_dir():
            files.extend(sorted(path.rglob("*.s")))
        elif path.suffix == ".s":
            files.append(path)
    return sorted(set(files))


def write_tsv(path: Path, groups: list[tuple[str, list[Function], str]]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("w", newline="") as f:
        writer = csv.writer(f, delimiter="\t")
        writer.writerow(
            [
                "group",
                "count",
                "insns",
                "bytes_total",
                "function",
                "vram",
                "file",
                "symbols",
                "callees",
            ]
        )
        for group_id, funcs, _sig in groups:
            for func in funcs:
                writer.writerow(
                    [
                        group_id,
                        len(funcs),
                        len(func.insns),
                        sum(f.size_bytes for f in funcs),
                        func.name,
                        f"0x{func.start_vram:08X}",
                        str(func.path),
                        ",".join(symbols_in(func)),
                        ",".join(callees_in(func)),
                    ]
                )


def print_markdown(groups: list[tuple[str, list[Function], str]], limit: int) -> None:
    print("# ASM Shape Families")
    print()
    print("| Group | Count | Insns | Total bytes | Functions | Symbols/callees sample |")
    print("| --- | ---: | ---: | ---: | --- | --- |")
    for group_id, funcs, _sig in groups[:limit]:
        names = ", ".join(f"`{func.name}`" for func in funcs[:8])
        if len(funcs) > 8:
            names += f", ... +{len(funcs) - 8}"
        samples: list[str] = []
        for func in funcs[:3]:
            bits = symbols_in(func)[:2] + callees_in(func)[:2]
            if bits:
                samples.append(f"{func.name}: {', '.join(bits)}")
        print(
            f"| `{group_id}` | {len(funcs)} | {len(funcs[0].insns)} | "
            f"{sum(func.size_bytes for func in funcs)} | {names} | {'; '.join(samples)} |"
        )


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("paths", nargs="*", type=Path, default=[Path("asm/USA/main/main")])
    parser.add_argument("--min-count", type=int, default=2)
    parser.add_argument("--min-insns", type=int, default=2)
    parser.add_argument("--max-insns", type=int, default=80)
    parser.add_argument("--limit", type=int, default=40)
    parser.add_argument("--ignore-registers", action="store_true")
    parser.add_argument("--tsv", type=Path)
    args = parser.parse_args()

    funcs: list[Function] = []
    for path in iter_asm_files(args.paths):
        funcs.extend(parse_functions(path))

    buckets: dict[str, list[Function]] = defaultdict(list)
    sigs: dict[str, str] = {}
    for func in funcs:
        if not (args.min_insns <= len(func.insns) <= args.max_insns):
            continue
        sig = function_signature(func, args.ignore_registers)
        group_id = short_hash(sig)
        buckets[group_id].append(func)
        sigs[group_id] = sig

    groups = [
        (group_id, sorted(group, key=lambda f: f.start_vram), sigs[group_id])
        for group_id, group in buckets.items()
        if len(group) >= args.min_count
    ]
    groups.sort(key=lambda item: (-sum(f.size_bytes for f in item[1]), -len(item[1]), len(item[1][0].insns), item[0]))

    if args.tsv:
        write_tsv(args.tsv, groups)
    print_markdown(groups, args.limit)
    print()
    print(f"Scanned functions: {len(funcs)}")
    print(f"Groups emitted: {len(groups)}")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
