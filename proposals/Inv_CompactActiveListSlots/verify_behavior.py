#!/usr/bin/env python3
"""Compare the inventory slot pair with retail and independent state models."""
import argparse
import hashlib
import random
import struct
import subprocess
import tempfile
from pathlib import Path
from elftools.elf.elffile import ELFFile
from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN, UC_HOOK_CODE
from unicorn.mips_const import UC_MIPS_REG_A0, UC_MIPS_REG_SP, UC_MIPS_REG_RA, UC_MIPS_REG_PC, UC_MIPS_REG_V0, UC_MIPS_REG_S0, UC_MIPS_REG_GP

ENTRY = {'find': 0x8005382C, 'compact': 0x80054CF8}
LIMIT_HELPER, BONUS_HELPER = 0x80052F70, 0x80051E58
STACK, RETURN, GP = 0x801FF000, 0x8000F000, 0x8009CD70
AYA, ALT, BASE_CAPACITY, TRACKED = 0x800C0E48, 0x80180020, 0x800C0E0C, 0x800C0E20
POINTER, LIMIT, BITS, STORAGE, WORDS, PENDING = 0x8009D048, 0x8009D050, 0x8009D058, 0x8009D05C, 0x8009D064, 0x8009D06C


def link(path, name):
    definitions = {'Inv_GetAyaSlotLimit': LIMIT_HELPER, 'Inv_GetBonusSlotCount': BONUS_HELPER,
                   '_gp': GP, 'D_800C0E48': AYA, 'D_800C0E0C': BASE_CAPACITY,
                   'D_800C0E20': TRACKED, 'D_8009D048': POINTER, 'D_8009D050': LIMIT,
                   'D_8009D058': BITS, 'D_8009D05C': STORAGE, 'D_8009D064': WORDS, 'D_8009D06C': PENDING}
    layout = '\n'.join(f'{key} = {value};' for key, value in definitions.items())
    layout += f'\nSECTIONS {{ .text {ENTRY[name]} : SUBALIGN(4) {{ *(.text .text.*) }} /DISCARD/ : {{ *(.reginfo) *(.mdebug) }} }}'
    with tempfile.TemporaryDirectory() as d:
        d = Path(d)
        (d / 'code.ld').write_text(layout)
        subprocess.run(['mipsel-none-elf-ld', '-T', str(d / 'code.ld'), str(path), '-o', str(d / 'code.elf')], check=True)
        with (d / 'code.elf').open('rb') as f:
            return ELFFile(f).get_section_by_name('.text').data()


def expected_find(values, capacity, count):
    # Retail requires an initial empty slot even for a nonpositive count.
    for index in range(max(0, capacity - count + 1)):
        if values[index] == 0 and all(v == 0 for v in values[index + 1:index + max(1, count)]):
            return index
    return -1


def expected_compact(values, capacity, pending, tracked, base, bonus_script):
    values, tracked = list(values), list(tracked)
    trace = []
    while pending > 0:
        index = next((i for i in range(capacity) if values[i] == 0), -1)
        if index >= 0:
            bonus, new_base = bonus_script[len(trace) % len(bonus_script)]
            base = base if new_base is None else new_base
            trace.append((tuple(values), tuple(tracked), pending, bonus, base))
            limit = 50
            if base + bonus < 51:
                bonus, new_base = bonus_script[len(trace) % len(bonus_script)]
                base = base if new_base is None else new_base
                trace.append((tuple(values), tuple(tracked), pending, bonus, base))
                limit = base + bonus
            source = limit - pending
            assert 0 <= source < len(values)
            values[index] = values[source]
            values[source] = 0
            if tracked[0] == source: tracked[0] = index
            elif tracked[1] == source: tracked[1] = index
        pending -= 1
    return values, pending, tracked, base, trace


def run(exe, name, case, code=None):
    cpu = Uc(UC_ARCH_MIPS, UC_MODE_MIPS32 | UC_MODE_LITTLE_ENDIAN)
    cpu.mem_map(0, 0x200000)
    def read(p, n): return bytes(cpu.mem_read(p & 0x1FFFFFFF, n))
    def write(p, b): cpu.mem_write(p & 0x1FFFFFFF, b)
    def word(p, v): write(p, struct.pack('<I', v & 0xFFFFFFFF))
    def get(p): return struct.unpack('<i', read(p, 4))[0]
    def slots(p): return tuple(struct.unpack('<64h', read(p, 128)))
    def tracked(): return struct.unpack('<b', read(TRACKED, 1))[0], struct.unpack('<b', read(TRACKED + 2, 1))[0]
    start, size = struct.unpack_from('<II', exe, 0x18)
    write(start, exe[0x800:0x800 + size])
    if code is not None: write(ENTRY[name], code)
    for p in [AYA, ALT]: write(p - 16, b'\xA5' * 160)
    # Selection globals have gaps and storage that must remain untouched.
    write(POINTER, b'\x5A' * 40)
    write(TRACKED, b'\x7E' * 4)
    saved = {UC_MIPS_REG_S0 + i: 0xCAFE0000 + i for i in range(8)}
    for r, v in [(UC_MIPS_REG_SP, STACK), (UC_MIPS_REG_RA, RETURN), (UC_MIPS_REG_GP, GP), *saved.items()]: cpu.reg_write(r, v)
    trace = []
    if name == 'find':
        values, capacity, count, redirect = case
        pointer = ALT if redirect else AYA
        write(pointer, struct.pack('<64h', *values))
        cpu.reg_write(UC_MIPS_REG_A0, count & 0xFFFFFFFF)
        def helper(cpu, address, size, _):
            assert get(POINTER) & 0xFFFFFFFF == AYA
            trace.append(('limit', capacity))
            if redirect: word(POINTER, ALT)
            cpu.reg_write(UC_MIPS_REG_V0, capacity)
            cpu.reg_write(UC_MIPS_REG_PC, cpu.reg_read(UC_MIPS_REG_RA))
        cpu.hook_add(UC_HOOK_CODE, helper, begin=LIMIT_HELPER, end=LIMIT_HELPER)
    else:
        values, capacity, pending, initial_tracked, base, script = case
        pointer = ALT
        write(pointer, struct.pack('<64h', *values))
        word(POINTER, pointer)
        word(LIMIT, capacity)
        word(PENDING, pending)
        write(BASE_CAPACITY, bytes([base]))
        write(TRACKED, struct.pack('<b', initial_tracked[0]))
        write(TRACKED + 2, struct.pack('<b', initial_tracked[1]))
        expected_values, expected_pending, expected_tracked, expected_base, expected_trace = expected_compact(values, capacity, pending, initial_tracked, base, script)
        def helper(cpu, address, size, _):
            bonus, new_base = script[len(trace) % len(script)]
            if new_base is not None: write(BASE_CAPACITY, bytes([new_base]))
            trace.append((slots(pointer), tracked(), get(PENDING), bonus, read(BASE_CAPACITY, 1)[0]))
            cpu.reg_write(UC_MIPS_REG_V0, bonus)
            cpu.reg_write(UC_MIPS_REG_PC, cpu.reg_read(UC_MIPS_REG_RA))
        cpu.hook_add(UC_HOOK_CODE, helper, begin=BONUS_HELPER, end=BONUS_HELPER)
    before = read(POINTER, 40)
    tables_before = {p: read(p - 16, 160) for p in [AYA, ALT]}
    cpu.emu_start(ENTRY[name], RETURN, count=50000)
    assert cpu.reg_read(UC_MIPS_REG_PC) == RETURN, (name, case)
    assert cpu.reg_read(UC_MIPS_REG_SP) == STACK
    assert cpu.reg_read(UC_MIPS_REG_GP) == GP
    assert all(cpu.reg_read(r) == v for r, v in saved.items())
    expected_globals = bytearray(before)
    if name == 'find':
        result = cpu.reg_read(UC_MIPS_REG_V0)
        assert result == expected_find(values, capacity, count) & 0xFFFFFFFF, case
        assert trace == [('limit', capacity)]
        for addr, value in [(POINTER, pointer), (LIMIT, capacity), (BITS, STORAGE), (WORDS, 2)]:
            struct.pack_into('<I', expected_globals, addr - POINTER, value)
        assert all(read(p - 16, 160) == b for p, b in tables_before.items())
    else:
        assert slots(pointer) == tuple(expected_values), case
        assert tracked() == tuple(expected_tracked), case
        assert read(BASE_CAPACITY, 1) == bytes([expected_base])
        assert trace == expected_trace, case
        struct.pack_into('<i', expected_globals, PENDING - POINTER, expected_pending)
        for p, b in tables_before.items():
            if p == pointer:
                assert read(p - 16, 16) == b[:16] and read(p + 128, 16) == b[-16:]
            else: assert read(p - 16, 160) == b
        assert read(TRACKED + 1, 1) == read(TRACKED + 3, 1) == b'\x7E'
        result = None  # This routine has a void interface.
    assert read(POINTER, 40) == bytes(expected_globals)
    return result, trace, read(POINTER, 40), read(pointer - 16, 160), read(TRACKED, 4)


def cases():
    rng = random.Random(0x54CF8)
    layouts = [[0] * 64, [1] * 64, [i % 2 for i in range(64)], [1] * 63 + [0]]
    for zero in [0, 1, 2, 4, 9, 49]:
        values = [1] * 64
        values[zero:zero + 3] = [0] * 3
        layouts.append(values)
    layouts += [[rng.choice([0, 0, 1, -1, 0x7FFF, -0x8000]) for _ in range(64)] for _ in range(24)]
    for values in layouts:
        for capacity in [0, 1, 2, 5, 10, 50]:
            for count in sorted({-2, 0, 1, 2, 3, capacity, capacity + 1}):
                for redirect in [False, True]:
                    yield 'find', (values, capacity, count, redirect)
    for values in layouts:
        for capacity in [0, 1, 5, 50]:
            for pending in [-1, 0, 1, 3]:
                for initial_tracked in [(-1, -1), (49, 48), (47, 47), (0, 1)]:
                    for base, script in [(50, [(0, None)]), (49, [(2, None)]), (10, [(1, 9), (3, 12)])]:
                        yield 'compact', (values, capacity, pending, initial_tracked, base, script)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('find', type=Path)
    parser.add_argument('compact', type=Path)
    parser.add_argument('--retail', type=Path, default=Path('assets/USA/main.exe'))
    args = parser.parse_args()
    exe = args.retail.read_bytes()
    assert hashlib.sha1(exe).hexdigest() == '452fb033f2eaa4b18aa20a5bca60b8125af3a37b'
    code = {name: link(getattr(args, name), name) for name in ENTRY}
    counts = dict.fromkeys(ENTRY, 0)
    for name, case in cases():
        assert run(exe, name, case) == run(exe, name, case, code[name]), (name, case)
        counts[name] += 1
    print(f'PASS: {counts}; state models, helper traces, memory bounds, SP, GP and saved registers agree')


if __name__ == '__main__': main()
