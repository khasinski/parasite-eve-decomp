#!/usr/bin/env python3
"""Replace baked assembly bodies in C files with INCLUDE_ASM stubs.

Handles both carrier shapes: top-level `asm("...")` transcription blocks
and `__maspsx_include_asm_hack_*` wrapper functions. Each replaced block
becomes one INCLUDE_ASM stub per exported .globl label, pointing at the
splat-generated nonmatchings asm for the owning translation unit.
"""
from __future__ import annotations

import pathlib
import re
import sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
SRC = ROOT / "src/main"

GLOBL = re.compile(r'\.globl\s+(\w+)')

# a top-level or wrapper asm block: from a line starting the construct to
# the closing `);` (optionally wrapped in a hack function)
WRAPPER = re.compile(
    r'(?:void\s+__maspsx_include_asm_hack_\w+\s*\(void\)\s*;\s*)?'
    r'void\s+__maspsx_include_asm_hack_\w+\s*\(void\)\s*\{\s*'
    r'__asm__\s*\((?:\s*"(?:[^"\\]|\\.)*"\s*)+\)\s*;\s*\}',
    re.S)
TOPLEVEL = re.compile(
    r'(?:__asm__|asm)\s*\(\s*(?:"(?:[^"\\]|\\.)*"\s*)+\)\s*;',
    re.S)


def tu_of(path: pathlib.Path) -> str:
    return str(path.relative_to(SRC).with_suffix(""))


def convert(path: pathlib.Path) -> list[str] | None:
    text = path.read_text(errors="ignore")
    tu = tu_of(path)
    converted = []

    def repl(m):
        block = m.group(0)
        labels = GLOBL.findall(block)
        has_body = ('.word' in block or 'maspsx-keep' in block
                    or '.ent' in block)
        if not labels or not has_body:
            return block  # alias exports / includes: keep
        stubs = "\n\n".join(
            f'INCLUDE_ASM("asm/USA/main/nonmatchings/{tu}", {name});'
            for name in labels)
        converted.extend(labels)
        return stubs

    text = WRAPPER.sub(repl, text)
    text = TOPLEVEL.sub(repl, text)
    if not converted:
        return None
    if '#include "include_asm.h"' not in text:
        m = re.search(r'\A(?:\s*/\*.*?\*/\s*)*', text, re.S)
        text = text[:m.end()] + '\n#include "include_asm.h"\n' + text[m.end():]
    path.write_text(text)
    return converted


def main():
    total_files = total_funcs = 0
    for f in sorted(SRC.rglob("*.c")):
        if '.inc.s' in f.read_text(errors='ignore'):
            continue  # data carriers keep their include mechanism
        got = convert(f)
        if got:
            total_files += 1
            total_funcs += len(got)
    print(f"converted {total_funcs} asm bodies in {total_files} files")


if __name__ == "__main__":
    main()
