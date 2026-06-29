#!/usr/bin/env python3
"""Inventory PSYQ .LIB files: list member source-file paths and exported symbols.

PSYQ libraries are Sony-format archives (header `LIB\\x01`) containing OBJ
members. Each OBJ embeds its `C:\\PSX\\SRC\\<dir>\\<file>.C` source path and
exports a set of public symbols. This script extracts both.

Usage:
  psyq_lib_members.py <LIB_FILE_OR_DIR> [--symbols] [--paths] [--grep SYMBOL]

Examples:
  # List source-file paths in every LIBCD/LIBDS member:
  psyq_lib_members.py sdk/psyq-4.0/PSX/LIB/LIBCD.LIB --paths
  psyq_lib_members.py sdk/psyq-4.0/PSX/LIB/LIBDS.LIB --paths

  # Search for which library exports a given symbol:
  psyq_lib_members.py sdk/psyq-4.0/PSX/LIB --grep CdSearchFile
  psyq_lib_members.py sdk/psyq-4.0/PSX/LIB --grep DS_newmedia

  # Full inventory (paths + first 60 exported symbols per lib):
  psyq_lib_members.py sdk/psyq-4.0/PSX/LIB --paths --symbols
"""
from __future__ import annotations
import argparse
import re
from pathlib import Path


def extract_source_paths(data: bytes) -> list[str]:
    """Return embedded C:\\PSX\\SRC\\*.C paths in member-occurrence order."""
    paths = re.findall(rb"C:\\PSX\\SRC\\[A-Z0-9_]+\\[A-Z0-9_]+\.C", data)
    seen: set[str] = set()
    out: list[str] = []
    for p in paths:
        s = p.decode("latin-1")
        if s not in seen:
            seen.add(s)
            out.append(s)
    return out


def extract_exported_symbols(data: bytes) -> list[str]:
    """Walk the length-prefixed identifier records in the .LIB header symbol table.

    The format is `[u8 len][name bytes]...`. Accepts identifiers that look
    like C symbols (alphanum + `_` + `$`, 2..64 chars). Parser is forgiving:
    scans from byte 0x10 to end, skipping bytes that don't decode cleanly.
    """
    if data[:4] != b"LIB\x01":
        return []
    syms: list[str] = []
    seen: set[str] = set()
    pos = 0x10
    end = len(data)
    while pos < end:
        n = data[pos]
        if 2 <= n <= 64 and pos + 1 + n <= end:
            name = data[pos + 1 : pos + 1 + n]
            if all(0x21 <= b < 0x7F for b in name):
                t = name.decode("latin-1")
                if re.fullmatch(r"[A-Za-z_$][A-Za-z0-9_$]+", t) and t not in seen:
                    seen.add(t)
                    syms.append(t)
                pos += 1 + n
                continue
        pos += 1
    return syms


def lib_internal_name(data: bytes) -> str:
    if data[:4] != b"LIB\x01":
        return "(not a PSYQ .LIB)"
    return data[4:12].rstrip(b" \x00").decode("latin-1")


def collect_libs(target: Path) -> list[Path]:
    if target.is_dir():
        return sorted(target.glob("*.LIB"))
    return [target]


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("path", type=Path, help="Library file or directory containing .LIB files")
    ap.add_argument("--paths", action="store_true", help="List embedded source-file paths per library")
    ap.add_argument("--symbols", action="store_true", help="List exported symbols per library")
    ap.add_argument("--grep", help="Find libraries that export this symbol name (case-sensitive)")
    args = ap.parse_args()

    libs = collect_libs(args.path)
    if not libs:
        print(f"No .LIB files found at {args.path}")
        return 1

    if args.grep:
        hits: list[tuple[Path, str]] = []
        for lib in libs:
            data = lib.read_bytes()
            syms = extract_exported_symbols(data)
            for s in syms:
                if s == args.grep:
                    hits.append((lib, lib_internal_name(data)))
                    break
        if not hits:
            print(f"Symbol {args.grep!r} not exported by any library in {args.path}")
            return 1
        print(f"Symbol {args.grep!r} found in:")
        for lib, internal in hits:
            print(f"  {lib.name}  (internal name: {internal})")
            paths = extract_source_paths(lib.read_bytes())
            if paths:
                print(f"    Source files in this library:")
                for p in paths:
                    print(f"      {p}")
        return 0

    if not (args.paths or args.symbols):
        args.paths = True

    for lib in libs:
        data = lib.read_bytes()
        internal = lib_internal_name(data)
        print(f"\n=== {lib.name}  (internal: {internal}, {len(data)} bytes) ===")
        if args.paths:
            paths = extract_source_paths(data)
            print(f"  Source files ({len(paths)}):")
            for p in paths:
                print(f"    {p}")
        if args.symbols:
            syms = extract_exported_symbols(data)
            print(f"  Exported symbols ({len(syms)}):")
            for s in syms[:60]:
                print(f"    {s}")
            if len(syms) > 60:
                print(f"    ... and {len(syms)-60} more")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
