#!/usr/bin/env python3
"""Small decompilation workflow helpers.

This script intentionally works at proposal granularity. It does not edit the
main splat config or run a full repo build; it gives humans and worker agents a
shared way to extract a target function, compile one proposed C file, diff the
result, and summarize proposal state.
"""

from __future__ import annotations

import argparse
import difflib
import json
import re
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
ASM_DIR = ROOT / "asm/USA/main"
PROPOSALS_DIR = ROOT / "proposals"
CC = ROOT / "tools/scripts/cc.sh"
OBJDUMP = "mipsel-none-elf-objdump"
NM = "mipsel-none-elf-nm"


FUNC_RE = re.compile(r"^[A-Za-z_.$][A-Za-z0-9_.$]*$")
NONMATCHING_RE = re.compile(r"^nonmatching\s+([A-Za-z_.$][A-Za-z0-9_.$]*)(?:,\s+0x([0-9A-Fa-f]+))?")
ASM_COMMENT_RE = re.compile(r"/\*\s*([0-9A-Fa-f]+)\s+([0-9A-Fa-f]+)\s+([0-9A-Fa-f]{8})\s*\*/\s*(.*)")
OBJDUMP_INS_RE = re.compile(r"^\s*([0-9A-Fa-f]+):\s+([0-9A-Fa-f]{8})\s+(.+)$")
RELOC_RE = re.compile(r"R_MIPS_([A-Z0-9_]+)\s+([^\s]+)")
SYMBOL_RE = re.compile(r"^([A-Za-z_.$][A-Za-z0-9_.$]*)\s*=\s*(0x[0-9A-Fa-f]+)")
MAP_SYMBOL_RE = re.compile(r"^(0x[0-9A-Fa-f]+)\s+([A-Za-z_.$][A-Za-z0-9_.$]*)$")
GP_VALUE = 0x8009CD70
SYMBOL_VALUES_CACHE: dict[str, int] | None = None


@dataclass(frozen=True)
class TargetInfo:
    function: str
    asm_file: Path | None
    size: int


def check_func_name(func: str) -> None:
    if not FUNC_RE.match(func):
        raise SystemExit(f"invalid function name: {func}")


def run(cmd: list[str], *, cwd: Path = ROOT, stdout: Path | None = None) -> subprocess.CompletedProcess[str]:
    if stdout is None:
        return subprocess.run(cmd, cwd=cwd, text=True, check=False)
    with stdout.open("w") as f:
        return subprocess.run(cmd, cwd=cwd, text=True, stdout=f, stderr=subprocess.STDOUT, check=False)


def find_target(func: str) -> TargetInfo:
    check_func_name(func)
    for asm_file in ASM_DIR.rglob("*.s"):
        lines = asm_file.read_text(errors="replace").splitlines()
        if f"glabel {func}" not in lines and f"alabel {func}" not in lines:
            continue
        size = None
        for line in lines:
            match = NONMATCHING_RE.match(line)
            if match and match.group(1) == func:
                if match.group(2) is not None:
                    size = int(match.group(2), 16)
                break
        if size is None:
            start_vram = None
            last_vram = None
            active = False
            for line in lines:
                if line in {f"glabel {func}", f"alabel {func}"}:
                    active = True
                    continue
                if not active:
                    continue
                match = ASM_COMMENT_RE.search(line)
                if match:
                    vram = int(match.group(2), 16)
                    if start_vram is None:
                        start_vram = vram
                    last_vram = vram
                if line.startswith(("endlabel ", "enddlabel ")):
                    break
            if start_vram is None or last_vram is None:
                raise SystemExit(f"found {func} in {asm_file}, but could not infer size")
            size = last_vram - start_vram + 4
        return TargetInfo(function=func, asm_file=asm_file, size=size)
    base = proposal_dir(func)
    meta = base / "target.meta.json"
    target_s = base / "target.s"
    if target_s.exists():
        size = None
        if meta.exists():
            try:
                data = json.loads(meta.read_text())
                size = int(data["size_bytes"])
            except Exception:
                size = None
        if size is None:
            size = 4 * len(normalize_target(target_s))
        return TargetInfo(function=func, asm_file=None, size=size)
    raise SystemExit(f"target asm not found for {func}")


def proposal_dir(func: str) -> Path:
    return PROPOSALS_DIR / func


def extract_target(func: str, outdir: Path | None = None) -> Path:
    outdir = outdir or proposal_dir(func)
    outdir.mkdir(parents=True, exist_ok=True)
    target_path = outdir / "target.s"
    source_path = outdir / "target_source.txt"
    meta_path = outdir / "target.meta.json"
    if target_path.exists() and meta_path.exists():
        return target_path
    target = find_target(func)
    if target.asm_file is None:
        if target_path.exists():
            if not meta_path.exists():
                source_path.write_text("cached target.s\n")
                meta_path.write_text(
                    json.dumps(
                        {
                            "function": func,
                            "source": None,
                            "size": f"0x{target.size:X}",
                            "size_bytes": target.size,
                        },
                        indent=2,
                        sort_keys=True,
                    )
                    + "\n"
                )
            return target_path
        raise SystemExit(f"target asm not found for {func}")
    lines = target.asm_file.read_text(errors="replace").splitlines()
    extracted: list[str] = []
    active = False
    for line in lines:
        if line in {f"glabel {func}", f"alabel {func}"}:
            active = True
        if active:
            extracted.append(line)
        if active and line.startswith(("endlabel ", "enddlabel ")):
            break
    if not extracted:
        raise SystemExit(f"failed to extract {func} from {target.asm_file}")
    target_path.write_text("\n".join(extracted) + "\n")
    source_path.write_text(str(target.asm_file.relative_to(ROOT)) + "\n")
    (outdir / "target.meta.json").write_text(
        json.dumps(
            {
                "function": func,
                "source": str(target.asm_file.relative_to(ROOT)),
                "size": f"0x{target.size:X}",
                "size_bytes": target.size,
            },
            indent=2,
            sort_keys=True,
        )
        + "\n"
    )
    return target_path


def source_for(func: str, source: str | None) -> Path:
    if source:
        return (ROOT / source).resolve() if not Path(source).is_absolute() else Path(source)
    default = proposal_dir(func) / "candidate.c"
    if default.exists():
        return default
    src_dirs = [ROOT / "src/main/main", ROOT / "src/main/akao"]
    for src_dir in src_dirs:
        src_default = src_dir / f"{func}.c"
        if src_default.exists():
            return src_default
    searched = ", ".join(str(path / f"{func}.c") for path in src_dirs)
    raise SystemExit(f"no source supplied and no {default} or {searched}")


def parse_nm_size(nm_path: Path, func: str) -> int | None:
    if not nm_path.exists():
        return None
    for line in nm_path.read_text(errors="replace").splitlines():
        parts = line.split()
        if len(parts) >= 4 and parts[-1] == func:
            try:
                return int(parts[1], 16)
            except ValueError:
                return None
    return None


def build_func(func: str, source: str | None, name: str) -> Path:
    check_func_name(func)
    outdir = proposal_dir(func)
    build_dir = outdir / "build"
    build_dir.mkdir(parents=True, exist_ok=True)
    extract_target(func, outdir)
    src = source_for(func, source)
    obj = build_dir / f"{name}.o"
    objdump = build_dir / f"{name}.objdump"
    nm = build_dir / f"{name}.nm"
    log = build_dir / f"{name}.log"

    proc = run([str(CC), str(src), str(obj)], stdout=log)
    if proc.returncode != 0:
        raise SystemExit(f"compile failed; see {log}")
    proc = run([OBJDUMP, "-dr", str(obj)], stdout=objdump)
    if proc.returncode != 0:
        raise SystemExit(f"objdump failed for {obj}")
    proc = run([NM, "-S", "--size-sort", str(obj)], stdout=nm)
    if proc.returncode != 0:
        raise SystemExit(f"nm failed for {obj}")

    target = find_target(func)
    best_size = parse_nm_size(nm, func)
    (build_dir / f"{name}.json").write_text(
        json.dumps(
            {
                "function": func,
                "source": str(src.relative_to(ROOT) if src.is_relative_to(ROOT) else src),
                "object": str(obj.relative_to(ROOT)),
                "objdump": str(objdump.relative_to(ROOT)),
                "nm": str(nm.relative_to(ROOT)),
                "target_size": f"0x{target.size:X}",
                "best_size": f"0x{best_size:X}" if best_size is not None else None,
            },
            indent=2,
            sort_keys=True,
        )
        + "\n"
    )
    return objdump


def normalize_target(target_s: Path) -> list[str]:
    out: list[str] = []
    lines = target_s.read_text(errors="replace").splitlines()
    func_vram: int | None = None
    label_offsets: dict[str, int] = {}
    pending_labels: list[str] = []

    for raw in lines:
        line = raw.strip()
        if line.startswith(".L") and line.endswith(":"):
            pending_labels.append(line[:-1])
            continue
        match = ASM_COMMENT_RE.search(raw)
        if not match:
            continue
        vram = int(match.group(2), 16)
        if func_vram is None:
            func_vram = vram
        for label in pending_labels:
            label_offsets[label] = vram - func_vram
        pending_labels.clear()

    for raw in lines:
        line = raw.strip()
        if not line or line.startswith(("glabel ", "endlabel ", ".")) or line.endswith(":"):
            continue
        match = ASM_COMMENT_RE.search(raw)
        if not match:
            continue
        asm = canonical_asm(match.group(4))
        word = byteswap_word(match.group(3))
        local_jump = local_target_asm(asm, label_offsets)
        if local_jump is not None:
            out.append(local_jump)
            continue
        if target_needs_reloc_key(asm):
            out.append(f"reloc  {asm}")
        else:
            out.append(f"word  {word}")
    return out


def normalize_objdump(objdump: Path) -> list[str]:
    out: list[dict[str, str | bool]] = []
    last_addr: int | None = None
    for raw in objdump.read_text(errors="replace").splitlines():
        stripped = raw.strip()
        if not stripped:
            continue
        if "R_MIPS_" in stripped:
            if out:
                out[-1]["asm"] = apply_reloc(out[-1]["asm"], stripped, str(out[-1]["word"]))
                out[-1]["reloc"] = True
                match = RELOC_RE.search(stripped)
                if match:
                    out[-1]["reloc_kind"], out[-1]["reloc_symbol"] = match.groups()
            continue
        match = OBJDUMP_INS_RE.match(raw)
        if not match:
            continue
        addr = int(match.group(1), 16)
        if last_addr is not None:
            for _missing in range(last_addr + 4, addr, 4):
                out.append({"word": "00000000", "asm": "nop", "reloc": False})
        last_addr = addr
        out.append({"word": match.group(2).lower(), "asm": canonical_asm(match.group(3)), "reloc": False})
    annotate_hi_lo_addends(out)
    return [
        f"local  {str(item['asm']).removeprefix('local  ')}"
        if str(item["asm"]).startswith("local  ")
        else f"reloc  {item['asm']}"
        if item["reloc"]
        else f"word  {item['word']}"
        for item in out
    ]


def annotate_hi_lo_addends(out: list[dict[str, str | bool]]) -> None:
    pending_hi: dict[str, int] = {}
    for index, item in enumerate(out):
        kind = item.get("reloc_kind")
        symbol = item.get("reloc_symbol")
        if not isinstance(kind, str) or not isinstance(symbol, str):
            continue
        symbol = symbol.split("+")[0]
        if kind == "HI16":
            pending_hi[symbol] = index
            continue
        if kind != "LO16":
            continue
        addend = reloc_lo_addend(str(item["word"]))
        if addend == 0:
            pending_hi.pop(symbol, None)
            continue
        item["asm"] = add_reloc_addend(str(item["asm"]), symbol, addend)
        hi_index = pending_hi.pop(symbol, None)
        if hi_index is not None:
            out[hi_index]["asm"] = add_reloc_addend(str(out[hi_index]["asm"]), symbol, addend)


def reloc_lo_addend(word: str) -> int:
    addend = int(word, 16) & 0xFFFF
    if addend >= 0x8000:
        addend -= 0x10000
    return addend


def add_reloc_addend(asm: str, symbol: str, addend: int) -> str:
    suffix = f"{symbol} + {addend}" if addend > 0 else f"{symbol} - {-addend}"
    return asm.replace(f"%hi({symbol})", f"%hi({suffix})").replace(f"%lo({symbol})", f"%lo({suffix})")


def load_symbol_values() -> dict[str, int]:
    symbols: dict[str, int] = {}
    for path in [
        ROOT / "configs/USA/sym.main.txt",
        ROOT / "linkers/USA/undefined_funcs_auto.main.txt",
        ROOT / "linkers/USA/undefined_syms_auto.main.txt",
        ROOT / "linkers/USA/undefined_syms_manual.txt",
    ]:
        if not path.exists():
            continue
        for line in path.read_text(errors="replace").splitlines():
            match = SYMBOL_RE.match(line.strip())
            if match:
                symbols[match.group(1)] = int(match.group(2), 16)
    map_path = ROOT / "build/USA/main.map"
    if map_path.exists():
        for line in map_path.read_text(errors="replace").splitlines():
            match = MAP_SYMBOL_RE.match(line.strip())
            if match:
                symbols[match.group(2)] = int(match.group(1), 16)
    return symbols


def symbol_values() -> dict[str, int]:
    global SYMBOL_VALUES_CACHE
    if SYMBOL_VALUES_CACHE is None:
        SYMBOL_VALUES_CACHE = load_symbol_values()
    return SYMBOL_VALUES_CACHE


def target_start_vram(target_s: Path) -> int:
    for raw in target_s.read_text(errors="replace").splitlines():
        match = ASM_COMMENT_RE.search(raw)
        if match:
            return int(match.group(2), 16)
    raise SystemExit(f"could not find target start vram in {target_s}")


def is_raw_alabel_target(target_s: Path) -> bool:
    for raw in target_s.read_text(errors="replace").splitlines():
        stripped = raw.strip()
        if stripped.startswith("alabel "):
            return True
        if stripped.startswith("glabel "):
            return False
    return False


def normalize_objdump_resolved_words(objdump: Path, func_vram: int) -> list[str]:
    symbols = load_symbol_values()
    out: list[dict[str, int]] = []
    last_addr: int | None = None
    for raw in objdump.read_text(errors="replace").splitlines():
        stripped = raw.strip()
        if not stripped:
            continue
        if "R_MIPS_" in stripped:
            if out:
                out[-1]["word"] = resolve_relocated_word(out[-1]["word"], stripped, func_vram, symbols)
            continue
        match = OBJDUMP_INS_RE.match(raw)
        if not match:
            continue
        addr = int(match.group(1), 16)
        if last_addr is not None:
            for _missing in range(last_addr + 4, addr, 4):
                out.append({"addr": _missing, "word": 0})
        last_addr = addr
        out.append({"addr": addr, "word": int(match.group(2), 16)})
    return [f"word  {item['word']:08x}" for item in out]


def resolve_relocated_word(word: int, reloc: str, func_vram: int, symbols: dict[str, int]) -> int:
    match = RELOC_RE.search(reloc)
    if not match:
        return word
    kind, symbol = match.groups()
    symbol = symbol.split("+")[0]
    if symbol == ".text":
        addr = func_vram + ((word & 0x03FFFFFF) * 4)
    else:
        addr = symbols.get(symbol)
        if addr is None:
            return word
    if kind == "26":
        return (word & 0xFC000000) | ((addr >> 2) & 0x03FFFFFF)
    if kind == "HI16":
        return (word & 0xFFFF0000) | (((addr + 0x8000) >> 16) & 0xFFFF)
    if kind == "LO16":
        return (word & 0xFFFF0000) | (addr & 0xFFFF)
    if kind == "GPREL16":
        return (word & 0xFFFF0000) | ((addr - GP_VALUE) & 0xFFFF)
    return word


def byteswap_word(word: str) -> str:
    word = word.lower()
    return "".join(reversed([word[i : i + 2] for i in range(0, len(word), 2)]))


def target_needs_reloc_key(asm: str) -> bool:
    if "%hi(" in asm or "%lo(" in asm or "%gp_rel(" in asm:
        return True
    if re.match(r"^(j|jal) [A-Za-z_][A-Za-z0-9_]*$", asm):
        return True
    return False


def apply_reloc(asm: str | bool, reloc: str, word: str | None = None) -> str:
    asm = str(asm)
    match = RELOC_RE.search(reloc)
    if not match:
        return asm
    kind, symbol = match.groups()
    symbol = symbol.split("+")[0]
    if kind == "HI16":
        return re.sub(r",[^,]+$", f",%hi({symbol})", asm)
    if kind == "LO16":
        if "(" in asm:
            return re.sub(r",[^,()]+(\([^)]*\))$", f",%lo({symbol})\\1", asm)
        return re.sub(r",[^,]+$", f",%lo({symbol})", asm)
    if kind == "26":
        if symbol == ".text":
            if word is not None:
                opcode = asm.split()[0]
                target = (int(word, 16) & 0x03FFFFFF) * 4
                return f"local  {opcode} 0x{target:X}"
            local = local_target_asm(asm, {})
            if local is not None:
                return local
        return re.sub(r"^(j|jal) .+$", rf"\1 {symbol}", asm)
    if kind == "GPREL16":
        if word is not None:
            addend = int(word, 16) & 0xFFFF
            if addend >= 0x8000:
                addend -= 0x10000
            if addend > 0:
                symbol = f"{symbol} + {addend}"
            elif addend < 0:
                symbol = f"{symbol} - {-addend}"
        if "(" in asm:
            return re.sub(r",[^,()]+(\([^)]*\))$", f",%gp_rel({symbol})\\1", asm)
        return re.sub(r",[^,]+$", f",%gp_rel({symbol})", asm)
    return asm


def local_target_asm(asm: str, label_offsets: dict[str, int]) -> str | None:
    match = re.match(r"^(j|jal) ([^ ]+)(?: <[^+>]+\+(0x[0-9a-fA-F]+|[0-9]+)>)?$", asm)
    if not match:
        return None
    op, target, objdump_offset = match.groups()
    if target in label_offsets:
        return f"local  {op} 0x{label_offsets[target]:X}"
    if objdump_offset is not None:
        base = 16 if objdump_offset.lower().startswith("0x") else 10
        return f"local  {op} 0x{int(objdump_offset, base):X}"
    return None


def canonical_asm(text: str) -> str:
    text = text.strip()
    text = re.sub(r"/\*.*?\*/", "", text)
    text = text.strip()
    text = text.replace("$", "")
    text = re.sub(r"\s+", " ", text)
    text = re.sub(r"\s*,\s*", ",", text)
    text = text.replace("zero", "0")
    text = re.sub(r"\bs8\b", "fp", text)  # $30 alias: objdump prints s8, asm-differ prints fp
    text = re.sub(r"^li ([^,]+),(.+)$", r"addiu \1,0,\2", text)
    text = re.sub(r"^move ([^,]+),(.+)$", r"addu \1,\2,0", text)
    text = normalize_numbers(text)
    return text


def normalize_numbers(text: str) -> str:
    def repl(match: re.Match[str]) -> str:
        raw = match.group(0)
        sign = -1 if raw.startswith("-") else 1
        body = raw[1:] if sign < 0 else raw
        if body.lower().startswith("0x"):
            value = int(body, 16) * sign
        else:
            value = int(body, 10) * sign
        return str(value)

    return re.sub(r"(?<![A-Za-z0-9_%_])-?0x[0-9A-Fa-f]+|(?<![A-Za-z0-9_%_])-?\d+", lambda m: repl(m), text)


def diff_func(func: str, source: str | None, name: str) -> int:
    objdump = build_func(func, source, name)
    outdir = proposal_dir(func)
    target_s = outdir / "target.s"
    target_lines = normalize_target(target_s)
    if is_raw_alabel_target(target_s):
        obj_lines = normalize_objdump_resolved_words(objdump, target_start_vram(target_s))
    else:
        obj_lines = normalize_objdump(objdump)
    diff = list(
        difflib.unified_diff(
            target_lines,
            obj_lines,
            fromfile=f"{func}.target",
            tofile=f"{func}.{name}",
            lineterm="",
        )
    )
    diff_path = outdir / "build" / f"{name}.diff"
    diff_path.write_text("\n".join(diff) + ("\n" if diff else ""))
    if diff:
        categories = classify_diff(target_lines, obj_lines)
        print(f"DIFF: {diff_path.relative_to(ROOT)}")
        if categories:
            print(f"CATEGORIES: {', '.join(categories)}")
        print("\n".join(diff[:120]))
        return 1
    print(f"MATCH: {func} ({objdump.relative_to(ROOT)})")
    return 0


def classify_diff(target_lines: list[str], obj_lines: list[str]) -> list[str]:
    joined = "\n".join(target_lines + obj_lines)
    categories: list[str] = []
    if "%gp_rel(" in joined and ("%hi(" in joined or "%lo(" in joined):
        categories.append("symbol-access:gp-rel-vs-absolute")
    elif "%gp_rel(" in joined:
        categories.append("symbol-access:gp-relative")
    elif "%hi(" in joined or "%lo(" in joined:
        categories.append("symbol-access:absolute-hi-lo")
    if "local  j " in joined or "local  jal " in joined:
        categories.append("local-jump-or-jumptable")
    if any("jr ra" in line for line in target_lines + obj_lines):
        target_tail = target_lines[-4:]
        obj_tail = obj_lines[-4:]
        if target_tail != obj_tail:
            categories.append("frame-or-epilogue")
    if len(target_lines) != len(obj_lines):
        categories.append(f"size:{len(target_lines) * 4}->{len(obj_lines) * 4}")
    if has_delay_slot_swap(target_lines, obj_lines):
        categories.append("delay-slot-or-scheduling")
    return categories


def has_delay_slot_swap(target_lines: list[str], obj_lines: list[str]) -> bool:
    target_pairs = {(target_lines[i], target_lines[i + 1]) for i in range(len(target_lines) - 1)}
    obj_pairs = {(obj_lines[i + 1], obj_lines[i]) for i in range(len(obj_lines) - 1)}
    return bool(target_pairs & obj_pairs)


def status_rows() -> list[dict[str, str]]:
    rows: list[dict[str, str]] = []
    if not PROPOSALS_DIR.exists():
        return rows
    for status_path in sorted(PROPOSALS_DIR.glob("func_*/status.json")):
        func = status_path.parent.name
        try:
            status = json.loads(status_path.read_text())
        except json.JSONDecodeError:
            rows.append({"function": func, "status": "bad-json", "attempts": "-", "best": "-", "notes": str(status_path)})
            continue
        rows.append(
            {
                "function": status.get("function", func),
                "status": status.get("status", "-"),
                "attempts": str(status.get("attempts", "-")),
                "best": status.get("best", "-") or "-",
                "target_size": status.get("target_size", "-") or "-",
                "best_size": status.get("best_size", "-") or "-",
                "notes": status.get("notes", "") or "",
            }
        )
    return rows


def print_table(rows: list[dict[str, str]]) -> None:
    columns = ["function", "status", "attempts", "best", "target_size", "best_size", "notes"]
    widths = {col: len(col) for col in columns}
    for row in rows:
        for col in columns:
            widths[col] = max(widths[col], len(row.get(col, "")))
    print("  ".join(col.ljust(widths[col]) for col in columns))
    print("  ".join("-" * widths[col] for col in columns))
    for row in rows:
        print("  ".join(row.get(col, "").ljust(widths[col]) for col in columns))


def list_smallest(limit: int, min_size: int, retry: str) -> None:
    existing: dict[str, str] = {}
    for row in status_rows():
        existing[row["function"]] = row["status"]
    funcs: list[tuple[int, str]] = []
    for asm_file in ASM_DIR.rglob("*.s"):
        for line in asm_file.read_text(errors="replace").splitlines():
            match = NONMATCHING_RE.match(line)
            if not match:
                continue
            if match.group(2) is None:
                continue
            func = match.group(1)
            size = int(match.group(2), 16)
            if size < min_size:
                continue
            status = existing.get(func)
            if retry == "missing" and status is not None:
                continue
            if retry == "failed" and status != "failed":
                continue
            if retry == "near" and status not in {"failed", "near"}:
                continue
            funcs.append((size, func))
    seen: set[str] = set()
    for _size, func in sorted(funcs):
        if func in seen:
            continue
        seen.add(func)
        print(func)
        if len(seen) >= limit:
            break


def main() -> int:
    parser = argparse.ArgumentParser(description="Per-function decompilation pipeline helpers.")
    sub = parser.add_subparsers(dest="cmd", required=True)

    target_p = sub.add_parser("target", help="Extract target asm for a function.")
    target_p.add_argument("function")

    build_p = sub.add_parser("build", help="Compile one proposed C source for a function.")
    build_p.add_argument("function")
    build_p.add_argument("--source", "-s")
    build_p.add_argument("--name", default="candidate")

    diff_p = sub.add_parser("diff", help="Compile and normalized-diff against target asm.")
    diff_p.add_argument("function")
    diff_p.add_argument("--source", "-s")
    diff_p.add_argument("--name", default="candidate")

    sub.add_parser("status", help="Show proposal status summary.")

    smallest_p = sub.add_parser("list-smallest", help="List smallest nonmatching functions for workers.")
    smallest_p.add_argument("-n", "--limit", type=int, default=40)
    smallest_p.add_argument("--min-size", type=int, default=8)
    smallest_p.add_argument("--retry", choices=["missing", "failed", "near", "all"], default="missing")

    args = parser.parse_args()
    if args.cmd == "target":
        path = extract_target(args.function)
        print(path.relative_to(ROOT))
        return 0
    if args.cmd == "build":
        path = build_func(args.function, args.source, args.name)
        print(path.relative_to(ROOT))
        return 0
    if args.cmd == "diff":
        return diff_func(args.function, args.source, args.name)
    if args.cmd == "status":
        print_table(status_rows())
        return 0
    if args.cmd == "list-smallest":
        list_smallest(args.limit, args.min_size, args.retry)
        return 0
    raise AssertionError(args.cmd)


if __name__ == "__main__":
    try:
        sys.exit(main())
    except BrokenPipeError:
        try:
            sys.stdout.close()
        finally:
            sys.exit(0)
