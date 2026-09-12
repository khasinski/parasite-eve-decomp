#!/usr/bin/env python3
"""Check Gte_RotateVec CPU splitting/recombination with scripted GTE MAC results."""
import hashlib
import itertools
from pathlib import Path
import random
import struct
import subprocess
import sys
import tempfile
from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN, UC_HOOK_CODE
from unicorn.mips_const import UC_MIPS_REG_0, UC_MIPS_REG_A0, UC_MIPS_REG_A1, UC_MIPS_REG_A2, UC_MIPS_REG_V0, UC_MIPS_REG_SP, UC_MIPS_REG_RA, UC_MIPS_REG_PC

retail = Path('assets/USA/main.exe').read_bytes()
assert hashlib.sha1(retail).hexdigest() == '452fb033f2eaa4b18aa20a5bca60b8125af3a37b'
with tempfile.TemporaryDirectory() as work:
    work = Path(work)
    (work / 'code.ld').write_text('SECTIONS { .text 0x80078934 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) *(.mdebug) } }')
    subprocess.run(['mipsel-none-elf-ld', '-T', str(work / 'code.ld'), sys.argv[1], '-o', str(work / 'code.elf')], check=True)
    subprocess.run(['mipsel-none-elf-objcopy', '-O', 'binary', '--only-section=.text', str(work / 'code.elf'), str(work / 'code.bin')], check=True)
    candidate = (work / 'code.bin').read_bytes()

def signed(x, bits=32):
    x &= (1 << bits) - 1
    return x - (1 << bits) if x & (1 << (bits - 1)) else x

def split(x):
    if x < 0:
        magnitude = signed(-x)
        return signed(-(magnitude >> 15), 16), signed(-(magnitude & 0x7fff), 16)
    return signed(x >> 15, 16), x & 0x7fff

def run(code, data, output, results):
    cpu = Uc(UC_ARCH_MIPS, UC_MODE_MIPS32 | UC_MODE_LITTLE_ENDIAN)
    cpu.mem_map(0, 0x200000)
    cpu.mem_write(0x78934, code)
    operations = {}
    for i in range(0, len(code), 4):
        word = struct.unpack_from('<I', code, i)[0]
        if word >> 26 == 0x12:
            operations[0x80078934 + i] = word
            cpu.mem_write(0x78934 + i, bytes(4))
    cpu.mem_write(0x180000, bytes(data))
    gte, control, commands, pending = {}, {}, [], []
    sequence = 0
    def hook(cpu, address, size, user):
        nonlocal sequence
        sequence += 1
        for due, reg, value in list(pending):
            if due <= sequence:
                cpu.reg_write(UC_MIPS_REG_0 + reg, value & 0xffffffff)
                pending.remove((due, reg, value))
        if address not in operations:
            return
        word = operations[address]
        rt, rd, rs = (word >> 16) & 31, (word >> 11) & 31, (word >> 21) & 31
        if word in (0x4A41E012, 0x4A49E012):
            assert word == (0x4A41E012, 0x4A49E012)[len(commands)]
            commands.append(tuple(gte[r] for r in (9, 10, 11)))
            for r, value in zip((25, 26, 27), results[len(commands)-1]):
                gte[r] = value
        elif rs == 6:
            control[rd] = cpu.reg_read(UC_MIPS_REG_0 + rt)
        elif rs == 4:
            assert rd in (9, 10, 11)
            gte[rd] = signed(cpu.reg_read(UC_MIPS_REG_0 + rt), 16)
        elif rs == 0:
            assert rd in (25, 26, 27)
            pending.append((sequence + 2, rt, gte[rd]))
        else:
            raise AssertionError(hex(word))
    cpu.hook_add(UC_HOOK_CODE, hook)
    for reg, value in ((UC_MIPS_REG_A0, 0x80180010), (UC_MIPS_REG_A1, 0x80180040),
                       (UC_MIPS_REG_A2, 0x80180000 + output), (UC_MIPS_REG_SP, 0x801f0000),
                       (UC_MIPS_REG_RA, 0x801e0000)):
        cpu.reg_write(reg, value)
    cpu.emu_start(0x80078934, 0x801e0000, count=1000)
    assert cpu.reg_read(UC_MIPS_REG_PC) == 0x801e0000
    assert cpu.reg_read(UC_MIPS_REG_SP) == 0x801f0000
    assert cpu.reg_read(UC_MIPS_REG_V0) == 0x80180000 + output
    assert control == dict(enumerate(struct.unpack_from('<5I', data, 0x10)))
    parts = [split(x) for x in struct.unpack_from('<3i', data, 0x40)]
    assert commands == [tuple(p[0] for p in parts), tuple(p[1] for p in parts)]
    expected = bytearray(data)
    values = [signed((h << 3) + l) for h, l in zip(*results)]
    struct.pack_into('<3i', expected, output, *values)
    actual = bytes(cpu.mem_read(0x180000, len(data)))
    assert actual == expected
    return actual, commands

rng = random.Random(0x78934)
values = (-0x80000000, -32769, -32768, -1, 0, 1, 32767, 32768, 0x7fffffff)
vectors = list(itertools.product(values, repeat=3))
vectors += [tuple(signed(rng.getrandbits(32)) for _ in range(3)) for _ in range(128)]
count = 0
for i, vector in enumerate(vectors):
    data = bytearray(rng.getrandbits(8) for _ in range(160))
    struct.pack_into('<3i', data, 0x40, *vector)
    high = tuple(values[(i + n) % len(values)] for n in range(3))
    low = tuple(signed(rng.getrandbits(32)) for _ in range(3))
    for output in (0x70, 0x10, 0x40, 0x44):
        assert run(candidate, data, output, (high, low)) == run(retail[0x69134:0x69294], data, output, (high, low))
        count += 1
print(f'PASS {count} cases: signed splitting, scripted MAC overflow/recombination, GTE transfer delays, overlapping outputs, preserved bytes, pointer return and stack; GTE arithmetic/timing not emulated')
