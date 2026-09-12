#!/usr/bin/env python3
"""Run the arithmetic shift and firstfile hook against retail and semantic oracles."""
import argparse
import hashlib
from pathlib import Path
import random
import struct
import subprocess
import tempfile
from elftools.elf.elffile import ELFFile
from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN, UC_HOOK_CODE
from unicorn.mips_const import *

ENTRY = {'shift': 0x80072EA4, 'hook': 0x80072950}
STACK, RETURN, OUTPUT, TABLE = 0x801FF000, 0x8000F000, 0x80180010, 0x80150000
OLD, NEW, GLOBAL, NAME, STRCMP = 0x80170000, 0x80170010, 0x800A32D0, 0x800A32D8, 0x80072A54
MASK = (1 << 64) - 1
ARGS = [UC_MIPS_REG_A0, UC_MIPS_REG_A1, UC_MIPS_REG_A2, UC_MIPS_REG_A3]


def linked(path, name):
    with tempfile.TemporaryDirectory() as d:
        d = Path(d)
        (d / 'code.ld').write_text(f'''D_800A32D0 = {GLOBAL}; D_800A32D8 = {NAME}; strcmp = {STRCMP};
SECTIONS {{ .text {ENTRY[name]} : SUBALIGN(4) {{ *(.text .text.*) }} /DISCARD/ : {{ *(.reginfo) *(.mdebug) }} }}''')
        subprocess.run(['mipsel-none-elf-ld', '-T', str(d / 'code.ld'), str(path), '-o', str(d / 'code.elf')], check=True)
        with (d / 'code.elf').open('rb') as f:
            return ELFFile(f).get_section_by_name('.text').data()


def execute(exe, name, case, code=None):
    cpu = Uc(UC_ARCH_MIPS, UC_MODE_MIPS32 | UC_MODE_LITTLE_ENDIAN)
    cpu.mem_map(0, 0x200000)
    def write(p, data): cpu.mem_write(p & 0x1FFFFFFF, data)
    def read(p, n): return bytes(cpu.mem_read(p & 0x1FFFFFFF, n))
    def word(p, v): write(p, struct.pack('<I', v & 0xFFFFFFFF))
    def string(p):
        out = b''
        while read(p, 1) != b'\0':
            out += read(p, 1)
            p += 1
        return out
    address, size = struct.unpack_from('<II', exe, 0x18)
    write(address, exe[0x800:0x800 + size])
    if code is not None: write(ENTRY[name], code)
    write(OUTPUT - 16, b'\xA5' * 40)
    saved = {UC_MIPS_REG_S0 + i: 0xCAFE0000 + i for i in range(8)}
    for r, v in [(UC_MIPS_REG_SP, STACK), (UC_MIPS_REG_RA, RETURN), *saved.items()]: cpu.reg_write(r, v)
    trace = []
    if name == 'shift':
        value, direction, count = case
        params = [OUTPUT, direction, value & 0xFFFFFFFF, value >> 32]
        word(STACK + 16, count)
        signed = value - (1 << 64) if value >> 63 else value
        expected = ((signed >> max(0, count)) if direction else (value << max(0, count))) & MASK
    else:
        pattern, flags, remainder, result, mutate = case
        params = [OUTPUT, 0xDEADBEEF, 0x80012345]
        word(OUTPUT, flags)
        write(NAME, b'bu\0')
        word(GLOBAL, OLD)
        word(0x150, TABLE)
        word(0x154, len(pattern) * 80 + remainder)
        table = bytearray(b'\xA5' * ((len(pattern) + 1) * 80))
        for i, text in enumerate([*pattern, 'bu']):
            ptr = 0x80181000 + i * 32 if text is not None else 0
            if ptr: write(ptr, text.encode() + b'\0')
            struct.pack_into('<I', table, i * 80, ptr)
            struct.pack_into('<I', table, i * 80 + 0x34, 0x12340000 + i)
        write(TABLE, bytes(table))
        expected_table = bytearray(table)
        if 'bu' in pattern: struct.pack_into('<I', expected_table, pattern.index('bu') * 80 + 0x34, OLD)
        comparisons = []
        for text in pattern:
            if text is not None: comparisons.append(text.encode())
            if text == 'bu': break
        expected_callback = NEW if mutate and comparisons else OLD
        def hook(cpu, address, size, _):
            actual_args = tuple(cpu.reg_read(r) for r in ARGS[:3])
            if address == STRCMP:
                left, right = string(actual_args[0]), string(actual_args[1])
                trace.append(('strcmp', left, right))
                if mutate: word(GLOBAL, NEW)
                cpu.reg_write(UC_MIPS_REG_V0, 0 if left == right else 1)
            else:
                assert address == expected_callback
                assert actual_args == tuple(params)
                assert read(OUTPUT, 4) == struct.pack('<I', flags or 1)
                assert read(TABLE, len(table)) == bytes(expected_table)
                trace.append(('callback', address, actual_args))
                word(OUTPUT, 0x13572468)
                cpu.reg_write(UC_MIPS_REG_V0, result)
            cpu.reg_write(UC_MIPS_REG_PC, cpu.reg_read(UC_MIPS_REG_RA))
        for address in [STRCMP, OLD, NEW]: cpu.hook_add(UC_HOOK_CODE, hook, begin=address, end=address)
    for r, v in zip(ARGS, params): cpu.reg_write(r, v & 0xFFFFFFFF)
    cpu.emu_start(ENTRY[name], RETURN, count=10000)
    assert cpu.reg_read(UC_MIPS_REG_PC) == RETURN, (name, case)
    assert cpu.reg_read(UC_MIPS_REG_SP) == STACK
    assert all(cpu.reg_read(r) == v for r, v in saved.items())
    memory = read(OUTPUT - 16, 40)
    if name == 'shift':
        assert memory[16:24] == struct.pack('<Q', expected), case
        assert memory[:16] == memory[24:] == b'\xA5' * 16
        assert cpu.reg_read(UC_MIPS_REG_V0) == OUTPUT
        assert read(STACK + 16, 4) == struct.pack('<I', count & 0xFFFFFFFF)
        return memory
    assert memory == b'\xA5' * 16 + struct.pack('<I', 0x13572468) + b'\xA5' * 20
    assert trace == [('strcmp', s, b'bu') for s in comparisons] + [('callback', expected_callback, tuple(params))]
    assert cpu.reg_read(UC_MIPS_REG_V0) == result
    assert read(TABLE, len(table)) == bytes(expected_table)
    assert read(GLOBAL, 4) == struct.pack('<I', expected_callback)
    return memory, read(TABLE, len(table)), trace, result


def cases():
    rng = random.Random(0x72EA4)
    values = [0, 1, 0xFFFFFFFF, 0x80000000, 0x7FFFFFFF00000000, 1 << 63, MASK]
    values += [rng.getrandbits(64) for _ in range(64)]
    for value in values:
        for direction in [0, 1, -1, 2]:
            for count in [-2, -1, 0, 1, 2, 31, 32, 33, 63, 64, 65, 127, 128]:
                yield 'shift', (value, direction, count)
    for pattern in [[], [None], ['other'], ['bu'], [None, 'other', 'bu'], ['bu', 'bu'], ['other', None, 'other']]:
        for flags in [0, 1, 0xFFFFFFFF, 42]:
            for remainder in [0, 1, 79]:
                for result in [0, 1, 0xFFFFFFFF]:
                    for mutate in [False, True]:
                        yield 'hook', (pattern, flags, remainder, result, mutate)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('shift', type=Path)
    parser.add_argument('hook', type=Path)
    parser.add_argument('--retail', type=Path, default=Path('assets/USA/main.exe'))
    args = parser.parse_args()
    exe = args.retail.read_bytes()
    assert hashlib.sha1(exe).hexdigest() == '452fb033f2eaa4b18aa20a5bca60b8125af3a37b'
    code = {name: linked(getattr(args, name), name) for name in ENTRY}
    counts = dict.fromkeys(ENTRY, 0)
    for name, case in cases():
        assert execute(exe, name, case) == execute(exe, name, case, code[name]), (name, case)
        counts[name] += 1
    print(f'PASS: {counts}; semantic results, memory bounds, callback trace, SP and saved registers agree')


if __name__ == '__main__': main()
