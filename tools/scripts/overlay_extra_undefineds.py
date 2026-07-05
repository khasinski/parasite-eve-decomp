#!/usr/bin/env python3
"""Generate linker definitions for address-like unresolved overlay symbols."""

from __future__ import annotations

import argparse
import re
import subprocess
from pathlib import Path


NM = "mipsel-none-elf-nm"
OBJDUMP = "mipsel-none-elf-objdump"
UNDEFINED_RE = re.compile(r"^\s*U\s+(\S+)$")
RELOCATION_RE = re.compile(r"^[0-9A-Fa-f]+\s+R_MIPS_\S+\s+(\S+)$")
SYMBOL_ADDR_RE = re.compile(
    r"^(?:"
    r"\.L(?P<label>[0-9A-Fa-f]{8})"
    r"|func_(?P<func>[0-9A-Fa-f]{8})"
    r"|D_(?P<data>[0-9A-Fa-f]{8})"
    r"|jlabel_(?P<jlabel>[0-9A-Fa-f]{8})"
    r"|jtbl_(?P<jtbl>[0-9A-Fa-f]{8})"
    r"|jpt_(?P<jpt>[0-9A-Fa-f]{8})"
    r")$"
)
DEFINED_RE = re.compile(r"^\s*([A-Za-z_.$][A-Za-z0-9_.$]*)\s*=")


def undefined_symbols(obj: Path) -> set[str]:
    proc = subprocess.run(
        [NM, "-u", str(obj)],
        check=True,
        text=True,
        stdout=subprocess.PIPE,
    )
    symbols: set[str] = set()
    for line in proc.stdout.splitlines():
        match = UNDEFINED_RE.match(line)
        if match:
            symbols.add(match.group(1))
    relocs = subprocess.run(
        [OBJDUMP, "-r", str(obj)],
        check=True,
        text=True,
        stdout=subprocess.PIPE,
    )
    for line in relocs.stdout.splitlines():
        match = RELOCATION_RE.match(line.strip())
        if match:
            symbols.add(match.group(1))
    return symbols


def existing_definitions(paths: list[Path]) -> set[str]:
    defined: set[str] = set()
    for path in paths:
        if not path.exists():
            continue
        for line in path.read_text().splitlines():
            match = DEFINED_RE.match(line)
            if match:
                defined.add(match.group(1))
    return defined


def symbol_address(symbol: str) -> int | None:
    match = SYMBOL_ADDR_RE.match(symbol)
    if match is None:
        return None
    value = next(group for group in match.groups() if group is not None)
    return int(value, 16)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("build_dir", type=Path)
    parser.add_argument("--out", type=Path, required=True)
    parser.add_argument("--existing", type=Path, action="append", default=[])
    parser.add_argument("--symbols", type=Path, default=None,
                        help="splat symbol_addrs file; unresolved symbols "
                             "listed there are emitted as PROVIDE() pins")
    args = parser.parse_args()

    symbols: set[str] = set()
    for obj in sorted(args.build_dir.rglob("*.o")):
        symbols.update(undefined_symbols(obj))

    known: dict[str, int] = {}
    if args.symbols and args.symbols.exists():
        for line in args.symbols.read_text().splitlines():
            match = re.match(r"^(\w+)\s*=\s*(0x[0-9A-Fa-f]+);", line)
            if match:
                known[match.group(1)] = int(match.group(2), 16)

    already_defined = existing_definitions(args.existing)
    rows: list[tuple[int, str]] = []
    provides: list[tuple[int, str]] = []
    for symbol in symbols:
        if symbol in already_defined:
            continue
        address = symbol_address(symbol)
        if address is not None:
            rows.append((address, symbol))
        elif symbol in known:
            provides.append((known[symbol], symbol))

    args.out.parent.mkdir(parents=True, exist_ok=True)
    lines = [f"{symbol} = 0x{address:08X};" for address, symbol in sorted(rows)]
    lines += [f"PROVIDE({symbol} = 0x{address:08X});"
              for address, symbol in sorted(provides)]
    args.out.write_text("\n".join(lines) + ("\n" if lines else ""))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
