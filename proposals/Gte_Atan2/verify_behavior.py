#!/usr/bin/env python3
"""Compare the C atan2 candidate with retail MIPS over non-INT_MIN coordinates."""
import hashlib
import itertools
from pathlib import Path
import random
import struct
import subprocess
import sys
import tempfile
from elftools.elf.elffile import ELFFile
from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN
from unicorn.mips_const import UC_MIPS_REG_A0, UC_MIPS_REG_A1, UC_MIPS_REG_V0, UC_MIPS_REG_RA, UC_MIPS_REG_PC

exe = Path('build/USA/main.exe').read_bytes()
assert hashlib.sha1(exe).hexdigest() == '452fb033f2eaa4b18aa20a5bca60b8125af3a37b'
with tempfile.TemporaryDirectory(prefix='atan-behavior-') as work:
    script = Path(work) / 'candidate.ld'
    linked = Path(work) / 'candidate.elf'
    script.write_text('D_8009A6EC = 0x8009A6EC;\nSECTIONS { .text 0x80079FB4 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) *(.MIPS.abiflags) *(.pdr) *(.comment) *(.gnu.attributes) } }')
    subprocess.run(['mipsel-none-elf-ld', '-T', str(script), sys.argv[1], '-o', str(linked)], check=True)
    with linked.open('rb') as stream:
        code = ELFFile(stream).get_section_by_name('.text').data()


def machine(candidate):
    cpu = Uc(UC_ARCH_MIPS, UC_MODE_MIPS32 | UC_MODE_LITTLE_ENDIAN)
    cpu.mem_map(0, 0x200000)
    start, size = struct.unpack_from('<II', exe, 0x18)
    cpu.mem_write(start & 0x1FFFFFFF, exe[0x800:0x800 + size])
    if candidate:
        cpu.mem_write(0x79FB4, code)
    return cpu


def run(cpu, y, x):
    cpu.reg_write(UC_MIPS_REG_A0, y & 0xFFFFFFFF)
    cpu.reg_write(UC_MIPS_REG_A1, x & 0xFFFFFFFF)
    cpu.reg_write(UC_MIPS_REG_RA, 0x801E0000)
    cpu.emu_start(0x80079FB4, 0x801E0000, count=1000)
    assert cpu.reg_read(UC_MIPS_REG_PC) == 0x801E0000
    value = cpu.reg_read(UC_MIPS_REG_V0)
    return value if value < 0x80000000 else value - 0x100000000

corners = (0, 1, 2, 1023, 1024, 1025, 0x1FFFFF, 0x200000, 0x200001, 0x7FFFFFFE, 0x7FFFFFFF)
values = sorted({sign * value for value in corners for sign in (-1, 1)})
cases = list(itertools.product(values, repeat=2))
rng = random.Random(0xA7A2)
cases += [(rng.randint(-0x7FFFFFFF, 0x7FFFFFFF), rng.randint(-0x7FFFFFFF, 0x7FFFFFFF)) for _ in range(2048)]
retail, candidate = machine(False), machine(True)
for y, x in cases:
    expected = run(retail, y, x)
    assert run(candidate, y, x) == expected, (y, x, expected)
    assert -2048 <= expected <= 2048
for y, x, angle in ((0, 0, 0), (0, 1, 0), (1, 0, 1024), (0, -1, 2048), (-1, 0, -1024)):
    assert run(candidate, y, x) == angle
print(f'PASS {len(cases)} pairs plus five axis checks; retail table and MIPS division are executed')
