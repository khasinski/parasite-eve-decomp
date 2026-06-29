#!/usr/bin/env python3
"""Generate linker aliases for address-based function labels.

Generated data asm can still refer to func_XXXXXXXX labels even when the
function's real object symbol has a descriptive name. These aliases keep those
tables linkable without committing generated linker files.
"""

from __future__ import annotations

import re
import sys
from pathlib import Path


SYMBOL_RE = re.compile(r"^([A-Za-z_][A-Za-z0-9_]*)\s*=\s*(0x[0-9A-Fa-f]+);")
SUBSEGMENT_RE = re.compile(r"^\s*-\s*\[(0x[0-9A-Fa-f]+),\s*(?:c|asm),\s*([^,\]]+)\]")


def main() -> int:
    if len(sys.argv) not in (2, 3, 4):
        print(f"usage: {Path(sys.argv[0]).name} sym.main.txt [main.yaml] [asm_dir]", file=sys.stderr)
        return 2

    emitted: set[str] = set()

    def provide(name: str, addr: int) -> None:
        if name in emitted:
            return
        emitted.add(name)
        print(f"PROVIDE({name} = 0x{addr:08X});")

    print("PROVIDE(func_80000000 = 0x80000000);")
    provide("CdRom_Init", 0x80080AC4)
    for line in Path(sys.argv[1]).read_text().splitlines():
        match = SYMBOL_RE.match(line)
        if not match:
            continue
        name, addr_text = match.groups()
        addr = int(addr_text, 16)
        provide(name, addr)
        if name.startswith("func_") or "type:func" not in line:
            continue
        provide(f"func_{addr:08X}", addr)

    if len(sys.argv) == 3:
        config_path = Path(sys.argv[2])
    elif len(sys.argv) == 4:
        config_path = Path(sys.argv[2])
    else:
        config_path = None

    if config_path is not None:
        for line in config_path.read_text().splitlines():
            match = SUBSEGMENT_RE.match(line)
            if not match:
                continue
            rom_text, path_text = match.groups()
            name = Path(path_text.strip()).name
            if not re.match(r"^[A-Za-z_][A-Za-z0-9_]*$", name):
                continue
            addr = 0x80010000 + int(rom_text, 16) - 0x800
            provide(name, addr)
            provide(f"func_{addr:08X}", addr)

    if len(sys.argv) == 4:
        asm_dir = Path(sys.argv[3])
        if asm_dir.exists():
            for asm_path in asm_dir.rglob("*.s"):
                for token in re.findall(r"\bfunc_(800[0-9A-Fa-f]{5})\b", asm_path.read_text(errors="ignore")):
                    provide(f"func_{token.upper()}", int(token, 16))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
