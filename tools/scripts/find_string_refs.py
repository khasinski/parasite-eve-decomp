#!/usr/bin/env python3
"""Map every PSYQ/PE string in the EXE rodata to the function(s) that reference it.

Walks asm/USA/main/**/*.s, tracks the current function (most recent `glabel`),
and matches every `lui $rN, %hi(D_8001XXXX)` or `lui $rN, %hi(0x8001XXXX)`
against a known-string address table built from the live EXE.

Emits a Markdown table to stdout. The table is the bridge from raw asm to
named functions: every line says "function X uses string Y", which lets the
decomp agent rename X to something like `SetGrapQue` for free.
"""
from __future__ import annotations
import os, re, struct, sys
from collections import defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
EXE = ROOT / "assets" / "USA" / "main.exe"
ASM_DIR = ROOT / "asm" / "USA" / "main"

# Strings worth flagging — anything ASCII >= 4 chars in the rodata window of the EXE.
# Build the map from the live EXE so generated reports stay tied to the local target.
def build_string_map() -> dict[int, str]:
    data = EXE.read_bytes()
    table: dict[int, str] = {}
    # Walk every byte; collect ASCII runs of >= 4 chars followed by NUL.
    i = 0; n = len(data)
    while i < n:
        if 0x20 <= data[i] < 0x7F:
            j = i
            while j < n and 0x20 <= data[j] < 0x7F:
                j += 1
            if j - i >= 4 and j < n and data[j] == 0:
                # file offset i -> vram
                if i >= 0x800:
                    vram = 0x80010000 + (i - 0x800)
                    table[vram] = data[i:j].decode("latin-1")
            i = j + 1
        else:
            i += 1
    return table

STRINGS = build_string_map()

# Regex for the splat-emitted instructions of interest
RE_GLABEL = re.compile(r"^\s*glabel\s+(\S+)")
RE_LUI    = re.compile(r"lui\s+\$\w+,\s*%hi\((?:D_)?([0-9A-Fa-fx]+)\)")
RE_ADDIU  = re.compile(r"addiu\s+\$\w+,\s*\$\w+,\s*%lo\((?:D_)?([0-9A-Fa-fx]+)\)")
RE_LX_LO  = re.compile(r"(?:l[bhw]u?|s[bhw])\s+\$\w+,\s*%lo\((?:D_)?([0-9A-Fa-fx]+)\)\s*\(")

def parse_addr(tok: str) -> int | None:
    """Parse a splat operand like '800118D4' (D_xxxx stripped) or '0x80011840' to int."""
    try:
        if tok.lower().startswith("0x"):
            return int(tok, 16)
        # When splat emits D_800118D4 we already stripped to '800118D4'.
        return int(tok, 16)
    except ValueError:
        return None

def collect_refs() -> dict[str, set[tuple[int, str]]]:
    """function_name -> set of (string_vram, string_value)"""
    refs: dict[str, set[tuple[int, str]]] = defaultdict(set)
    for s in sorted(ASM_DIR.rglob("*.s")):
        cur = "<top-of-file>"
        for line in s.open(encoding="latin-1"):
            m = RE_GLABEL.search(line)
            if m:
                cur = m.group(1)
                continue
            for rx in (RE_LUI, RE_ADDIU, RE_LX_LO):
                m = rx.search(line)
                if not m:
                    continue
                addr = parse_addr(m.group(1))
                if addr is None:
                    continue
                if addr in STRINGS:
                    refs[cur].add((addr, STRINGS[addr]))
    return refs

def main() -> int:
    refs = collect_refs()
    print(f"# String → Function Cross-Reference\n")
    print(f"Generated from `asm/USA/main/**/*.s` against the live string table in")
    print(f"`assets/USA/main.exe`. Each row says \"this function loads this string\";")
    print(f"in PSYQ debug code that's a near-certain name match. Hand-confirm before renaming.\n")
    print(f"Total functions with string refs: **{len(refs)}**.")
    print(f"Total unique (function, string) edges: **{sum(len(v) for v in refs.values())}**.\n")
    print("| Function | String VRAM | String |")
    print("|----------|------------:|--------|")
    seen = set()
    for fn in sorted(refs):
        for vram, val in sorted(refs[fn]):
            # Escape pipes in the string content
            cell = val.replace("|", "\\|").replace("\n", " ")
            if len(cell) > 80:
                cell = cell[:77] + "..."
            print(f"| `{fn}` | `0x{vram:08X}` | `{cell}` |")
            seen.add(vram)
    print(f"\nDistinct strings referenced: **{len(seen)}** of {len(STRINGS)} known strings.")
    return 0

if __name__ == "__main__":
    raise SystemExit(main())
