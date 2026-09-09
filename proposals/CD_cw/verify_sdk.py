#!/usr/bin/env python3
"""Verify retail CD_cw against LIBCD BIOS_1.OBJ, allowing relocation fields only."""
import hashlib
from pathlib import Path
import re
import struct
import subprocess
import sys

retail = Path('build/USA/main.exe').read_bytes()
assert hashlib.sha1(retail).hexdigest() == '452fb033f2eaa4b18aa20a5bca60b8125af3a37b'
dump = subprocess.check_output(['psyk', 'list', '-r', '-c', sys.argv[1]], text=True)
assert "'CD_cw' at offset aac in section f002" in dump
assert "'CD_vol' at offset eb8 in section f002" in dump
text_dump = dump.split('2 : Code 5504 bytes', 1)[1].split('6 : Switch to section f001', 1)[0]
chunks = []
for line in text_dump.splitlines():
    match = re.fullmatch(r'[0-9a-f]{4}: ((?:[0-9a-f]{2} ?)+)', line)
    if match:
        chunks.append(bytes.fromhex(match.group(1)))
original = b''.join(chunks)
assert len(original) == 5504
patches = {int(offset, 16): int(kind) for kind, offset in re.findall(r'Patch type (\d+) at offset ([0-9a-f]+)', text_dump)}
expected = retail[0x6BD58:0x6C164]
relocated = []
for offset in range(0, len(expected), 4):
    left = struct.unpack_from('<I', original, 0xAAC + offset)[0]
    right = struct.unpack_from('<I', expected, offset)[0]
    if left == right:
        continue
    kind = patches.get(0xAAC + offset)
    assert kind in (74, 82, 84), (offset, kind)
    mask = 0x03FFFFFF if kind == 74 else 0xFFFF
    assert not ((left ^ right) & ~mask), hex(offset)
    relocated.append(offset)
assert len(expected) == 1036 and len(relocated) == 89
print('PASS: CD_cw, 1036 bytes; 170 identical words, 89 differences confined to SDK relocation fields')
