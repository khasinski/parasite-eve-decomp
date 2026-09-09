#!/usr/bin/env python3
"""Verify a reconstructed LIBCD function against BIOS_1.OBJ relocation fields."""
import hashlib
from pathlib import Path
import re
import struct
import subprocess
import sys

name = sys.argv[2] if len(sys.argv) > 2 else 'CD_cw'
start, end, next_name, file_start, relocation_count = {
    'CD_cw': (0xAAC, 0xEB8, 'CD_vol', 0x6BD58, 89),
    'CD_sync': (0x564, 0x7E4, 'CD_ready', 0x6B810, 52),
}[name]
retail = Path('build/USA/main.exe').read_bytes()
assert hashlib.sha1(retail).hexdigest() == '452fb033f2eaa4b18aa20a5bca60b8125af3a37b'
dump = subprocess.check_output(['psyk', 'list', '-r', '-c', sys.argv[1]], text=True)
assert f"'{name}' at offset {start:x} in section f002" in dump
assert f"'{next_name}' at offset {end:x} in section f002" in dump
text_dump = dump.split('2 : Code 5504 bytes', 1)[1].split('6 : Switch to section f001', 1)[0]
chunks = []
for line in text_dump.splitlines():
    match = re.fullmatch(r'[0-9a-f]{4}: ((?:[0-9a-f]{2} ?)+)', line)
    if match:
        chunks.append(bytes.fromhex(match.group(1)))
original = b''.join(chunks)
assert len(original) == 5504
patches = {int(offset, 16): int(kind) for kind, offset in re.findall(r'Patch type (\d+) at offset ([0-9a-f]+)', text_dump)}
expected = retail[file_start:file_start + end - start]
relocated = []
for offset in range(0, len(expected), 4):
    left = struct.unpack_from('<I', original, start + offset)[0]
    right = struct.unpack_from('<I', expected, offset)[0]
    if left == right:
        continue
    kind = patches.get(start + offset)
    assert kind in (74, 82, 84), (offset, kind)
    mask = 0x03FFFFFF if kind == 74 else 0xFFFF
    assert not ((left ^ right) & ~mask), hex(offset)
    relocated.append(offset)
assert len(expected) == end - start and len(relocated) == relocation_count
print(f"PASS: {name}, {len(expected)} bytes; {len(expected) // 4 - len(relocated)} identical words, {len(relocated)} differences confined to SDK relocation fields")
