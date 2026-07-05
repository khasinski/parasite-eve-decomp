#!/usr/bin/env python3
"""Compare one function in a built object against retail bytes.

Usage: func_cmp.py <obj> <func> [--vram 0x...]
Relocation-affected fields (hi/lo immediates, jal targets) are masked on
both sides; everything else must match word for word.
"""
from __future__ import annotations

import pathlib
import re
import struct
import subprocess
import sys

ROOT = pathlib.Path(__file__).resolve().parents[2]


def mask(w: int) -> int:
    op = w >> 26
    if op in (2, 3):                      # j/jal
        return op << 26
    if op == 0x0F:                        # lui
        return w & 0xFFFF0000
    if op in (8, 9, 0x0C, 0x0D, 0x23, 0x2B, 0x20, 0x24, 0x21, 0x25,
              0x28, 0x29, 0x22, 0x26, 0x2A, 0x2E, 0x30, 0x38):
        return w & 0xFFFF0000             # imm16 loads/stores/addiu/ori...
    return w


def main():
    obj, func = sys.argv[1], sys.argv[2]
    vram = None
    if '--vram' in sys.argv:
        vram = int(sys.argv[sys.argv.index('--vram') + 1], 16)
    else:
        sym = (ROOT / 'configs/USA/sym.main.txt').read_text()
        m = re.search(rf'^{func}\s*=\s*(0x[0-9A-Fa-f]+)', sym, re.M)
        vram = int(m.group(1), 16)
    out = subprocess.run(['mipsel-none-elf-objdump', '-dr', obj],
                         capture_output=True, text=True).stdout
    sec = re.search(rf'<{func}>:\n((?:\s.*\n)+?)\n', out + '\n')
    words = []
    reloc_idx = set()
    for l in sec.group(1).split('\n'):
        if ':\t' in l and re.match(r'\s+[0-9a-f]+:\t[0-9a-f]{8}', l):
            words.append(int(l.split('\t')[1], 16))
        elif 'R_MIPS' in l:
            reloc_idx.add(len(words) - 1)
    exe = (ROOT / 'assets/USA/main.exe').read_bytes()
    off = vram - 0x80010000 + 0x800
    tgt = [struct.unpack_from('<I', exe, off + i * 4)[0]
           for i in range(len(words))]
    bad = []
    for i, (a, b) in enumerate(zip(words, tgt)):
        if i in reloc_idx:
            if mask(a) != mask(b):
                bad.append((i, a, b))
        elif a != b:
            bad.append((i, a, b))
    # also report size vs next function padding-insensitively
    print(f'{func}: {len(words)} insns, {len(bad)} mismatched')
    for i, a, b in bad[:12]:
        print(f'  +0x{i*4:03X}: built {a:08X}  target {b:08X}')
    sys.exit(0 if not bad else 1)


if __name__ == '__main__':
    main()
