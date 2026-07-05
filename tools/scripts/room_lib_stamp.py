#!/usr/bin/env python3
"""Stamp a decompiled room-library shape into every room that carries it.

Usage: room_lib_stamp.py <clusters.json> <shape_hash> <MACRO_NAME> <BaseName>
For each instance (room, offset, size): split the room's asm subsegment,
add a per-overlay symbol, and write a one-line TU expanding the shared
body macro from src/overlays/room_lib/room_lib.h.
"""
from __future__ import annotations

import json
import pathlib
import re
import sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
VRAM_BASE = 0x8018EFE8


def stamp(clusters_path: str, shape: str, macro: str, base: str,
          params=()) -> None:
    """params: (sym_name, lui_idx, lo_idx) triples decoded per instance."""
    import struct
    inst = json.load(open(clusters_path))[shape]
    bins = {}
    wired = 0
    for room, off, size in sorted(inst):
        cfg = ROOT / f'configs/USA/overlays/{room}.yaml'
        t = cfg.read_text()
        vram = VRAM_BASE + off
        name = f'{base}_{vram:08X}'
        if name in t:
            continue
        if 'symbol_addrs_path' not in t:
            t = t.replace('  undefined_syms_auto_path:',
                          f'  symbol_addrs_path: configs/USA/overlays/sym.{room}.txt\n'
                          '  undefined_syms_auto_path:', 1)
        rows = sorted((int(m.group(1), 16), m.group(2), m.group(3) or '')
                      for m in re.finditer(
                          r'- \[(0x[0-9A-Fa-f]+), ([\w.]+)(?:, ([^\]]+))?\]', t))
        cover = None
        for (o1, ty, n1), (o2, _, _) in zip(rows, rows[1:] + [(1 << 30, '', '')]):
            if ty == 'asm' and o1 <= off and off + size <= o2:
                cover = (o1, n1, o2)
                break
        if not cover:
            print(f'SKIP {room}+0x{off:X}: no covering asm row')
            continue
        o1, n1, o2 = cover
        old = f'- [0x{o1:X}, asm, {n1}]'
        new = ''
        if o1 < off:
            new += f'- [0x{o1:X}, asm, {n1}]\n      '
        new += f'- [0x{off:X}, c, {name}]'
        if off + size < o2:
            new += f'\n      - [0x{off + size:X}, asm, {n1}_{off + size:X}]'
        t = t.replace(old, new, 1)
        cfg.write_text(t)
        symp = ROOT / f'configs/USA/overlays/sym.{room}.txt'
        sym = symp.read_text() if symp.exists() else ''
        if name not in sym:
            sym += f'{name} = 0x{vram:08X}; // type:func\n'
        for pname, hi_i, lo_i in params:
            if room not in bins:
                bins[room] = (ROOT / f'original/USA/overlays/{room}.bin').read_bytes()
            b = bins[room]
            if lo_i < 0:  # jal-kind param: hi_i is the jal instruction index
                w = struct.unpack_from('<I', b, off + hi_i * 4)[0]
                val = 0x80000000 | ((w & 0x3FFFFFF) << 2)
                line = f'{pname} = 0x{val:08X};'
                if pname not in sym:
                    sym += line + '\n'
                elif line not in sym:
                    print(f'PARAM CONFLICT {room}: {line}')
                continue
            hi = struct.unpack_from('<I', b, off + hi_i * 4)[0] & 0xFFFF
            lo = struct.unpack_from('<I', b, off + lo_i * 4)[0] & 0xFFFF
            # the encoded %hi already carries the +1 carry adjustment
            val = (hi << 16) + (lo - 0x10000 if lo >= 0x8000 else lo)
            line = f'{pname} = 0x{val & 0xFFFFFFFF:08X};'
            if pname not in sym:
                sym += line + '\n'
            elif line not in sym:
                print(f'PARAM CONFLICT {room}: {line} vs existing')
        symp.write_text(sym)
        src = ROOT / f'src/overlays/{room}/{name}.c'
        src.parent.mkdir(parents=True, exist_ok=True)
        src.write_text(f'#include "../room_lib/room_lib.h"\n\n{macro}({name})\n')
        wired += 1
    print(f'{base}: wired {wired} instances')


if __name__ == '__main__':
    params = []
    for a in sys.argv[5:]:
        n, hi, lo = a.split(':')
        params.append((n, int(hi), int(lo)))
    stamp(*sys.argv[1:5], params=tuple(params))
