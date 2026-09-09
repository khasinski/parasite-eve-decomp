#!/usr/bin/env python3
"""Verify the DSFILE object's text against the corresponding retail TU.

Usage: verify_sdk_tu.py /path/to/DSFILE.OBJ
Requires psyk on PATH. Does not modify either input.
"""
from collections import Counter
import hashlib
from pathlib import Path
import re
import struct
import subprocess
import sys

retail = Path('build/USA/main.exe').read_bytes()
assert hashlib.sha1(retail).hexdigest() == '452fb033f2eaa4b18aa20a5bca60b8125af3a37b'
dump = subprocess.check_output(['psyk', 'list', '-r', '-c', sys.argv[1]], text=True)
assert "'DsSearchFile' at offset 0 in section f002" in dump
text_dump = dump.split('2 : Code 2400 bytes', 1)[1].split('6 : Switch to section f001', 1)[0]
chunks = []
for line in text_dump.splitlines():
    match = re.fullmatch(r'[0-9a-f]{4}: ((?:[0-9a-f]{2} ?)+)', line)
    if match:
        chunks.append(bytes.fromhex(match.group(1)))
original = b''.join(chunks)
assert len(original) == 2400
patches = {int(offset, 16): int(kind) for kind, offset in
           re.findall(r'Patch type (\d+) at offset ([0-9a-f]+)', text_dump)}
expected = retail[0x71C14:0x72574]
differences = []
for offset in range(0, len(original), 4):
    left = struct.unpack_from('<I', original, offset)[0]
    right = struct.unpack_from('<I', expected, offset)[0]
    if left == right:
        continue
    assert offset in patches, f'non-relocated instruction differs at {offset:#x}'
    kind = patches[offset]
    assert kind in (74, 82, 84), f'unsupported relocation type {kind}'
    mask = 0x03FFFFFF if kind == 74 else 0x0000FFFF
    assert not ((left ^ right) & ~mask), f'opcode/register bits differ at {offset:#x}'
    differences.append(offset)
assert set(differences) == set(patches)
assert Counter(patches.values()) == {74: 47, 82: 62, 84: 62}
print('PASS: 2400-byte DSFILE TU; 429 identical words and 171 differences confined to relocation fields')
