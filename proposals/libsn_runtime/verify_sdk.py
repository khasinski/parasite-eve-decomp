#!/usr/bin/env python3
"""Verify the retail SN startup trio against a PsyQ LIBSN SNMAIN module."""

import hashlib
from pathlib import Path
import struct
import sys


RETAIL_SHA1 = "452fb033f2eaa4b18aa20a5bca60b8125af3a37b"
FILE_OFFSET = 0x62D34
TEXT_SIZE = 0x180

# Relocations in SNMAIN's text. PsyQ stores zero in each relocation field;
# the linked executable contains the resolved low 16 or 26 bits.
RELOC16 = {
    0x000, 0x004, 0x008, 0x00C, 0x024, 0x028, 0x03C, 0x040,
    0x04C, 0x050, 0x060, 0x064, 0x06C, 0x070, 0x074, 0x078,
    0x07C, 0x080, 0x090, 0x094, 0x0A8, 0x0AC, 0x0C8, 0x0CC,
    0x0D0, 0x118, 0x11C, 0x138,
}
RELOC26 = {0x088, 0x09C}


def words(data):
    return [word[0] for word in struct.iter_unpack("<I", data)]


if len(sys.argv) != 2:
    raise SystemExit("usage: verify_sdk.py /path/to/LIBSN.LIB")

exe = Path("build/USA/main.exe").read_bytes()
assert hashlib.sha1(exe).hexdigest() == RETAIL_SHA1
retail = exe[FILE_OFFSET:FILE_OFFSET + TEXT_SIZE]

archive = Path(sys.argv[1]).read_bytes()
# The first eight unrelocated SN entry instructions uniquely identify SNMAIN
# in both the PsyQ 4.6 v1 and 4.7 v2 archive formats.
signature = bytes.fromhex(
    "0000023c000042240000033c00006324000040ac040042242b084300fcff2014"
)
locations = []
start = 0
while True:
    start = archive.find(signature, start)
    if start < 0:
        break
    locations.append(start)
    start += 1
assert len(locations) == 1, locations
sdk = archive[locations[0]:locations[0] + TEXT_SIZE]
assert len(sdk) == TEXT_SIZE

differences = set()
for offset, (sdk_word, retail_word) in enumerate(zip(words(sdk), words(retail))):
    byte_offset = offset * 4
    if sdk_word == retail_word:
        continue
    differences.add(byte_offset)
    if byte_offset in RELOC16:
        assert sdk_word & 0xFFFF == 0
        assert sdk_word >> 16 == retail_word >> 16
    elif byte_offset in RELOC26:
        assert sdk_word & 0x03FFFFFF == 0
        assert sdk_word >> 26 == retail_word >> 26
    else:
        raise AssertionError(
            f"non-relocation mismatch at {byte_offset:#x}: "
            f"SDK {sdk_word:08x}, retail {retail_word:08x}"
        )

expected = RELOC16 | RELOC26
assert differences == expected, (sorted(differences), sorted(expected))
print(
    "PASS: LIBSN SNMAIN is the complete 384-byte retail TU; "
    "all 67 non-relocated words match and 29 relocation fields resolve"
)
