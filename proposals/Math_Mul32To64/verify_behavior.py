#!/usr/bin/env python3
"""Compare integer conversion and wide multiplication with retail and Python arithmetic."""
import argparse
import hashlib
from pathlib import Path
import random
import struct
import subprocess
import tempfile

from elftools.elf.elffile import ELFFile
from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN, UC_HOOK_CODE
from unicorn.mips_const import UC_MIPS_REG_A0, UC_MIPS_REG_A1, UC_MIPS_REG_A2, UC_MIPS_REG_A3, UC_MIPS_REG_SP, UC_MIPS_REG_RA, UC_MIPS_REG_PC, UC_MIPS_REG_V0, UC_MIPS_REG_V1, UC_MIPS_REG_S0

RETURN, STACK, OUTPUT = 0x8000F000, 0x801FF000, 0x80180010
ENTRIES = {'convert': 0x80073454, 'multiply': 0x800738D8}
HELPERS = {0x80073294: 'shift', 0x80072DF4: 'add'}
MASK32 = 0xFFFFFFFF


def link_object(path, name):
    layout = f'''Math_Shift64 = 0x80073294; Math_Add64 = 0x80072DF4;
SECTIONS {{ .text {ENTRIES[name]:#x} : SUBALIGN(4) {{ *(.text .text.*) }}
/DISCARD/ : {{ *(.reginfo) *(.mdebug) }} }}'''
    with tempfile.TemporaryDirectory() as directory:
        work = Path(directory)
        (work / 'code.ld').write_text(layout)
        subprocess.run(['mipsel-none-elf-ld', '-T', str(work / 'code.ld'), str(path),
                        '-o', str(work / 'code.elf')], check=True)
        with (work / 'code.elf').open('rb') as stream:
            return ELFFile(stream).get_section_by_name('.text').data()


def execute(exe, name, args, code=None):
    cpu = Uc(UC_ARCH_MIPS, UC_MODE_MIPS32 | UC_MODE_LITTLE_ENDIAN)
    cpu.mem_map(0, 0x200000)
    address, size = struct.unpack_from('<II', exe, 0x18)
    cpu.mem_write(address & 0x1FFFFFFF, exe[0x800:0x800 + size])
    if code is not None:
        cpu.mem_write(ENTRIES[name] & 0x1FFFFFFF, code)
    cpu.mem_write((OUTPUT - 16) & 0x1FFFFFFF, b'\xA5' * 40)
    registers = [(UC_MIPS_REG_SP, STACK), (UC_MIPS_REG_RA, RETURN)]
    call_args = args if name == 'convert' else [OUTPUT, *args]
    registers.extend(zip([UC_MIPS_REG_A0, UC_MIPS_REG_A1, UC_MIPS_REG_A2], call_args))
    saved = {UC_MIPS_REG_S0 + i: 0xCAFE0000 + i for i in range(8)}
    for register, value in registers + list(saved.items()):
        cpu.reg_write(register, value & MASK32)
    trace = []

    def hook(cpu, address, size, _):
        # Observe helper calls; their original bodies execute without stubs or patches.
        sp = cpu.reg_read(UC_MIPS_REG_SP)
        trace.append((HELPERS[address],
                      tuple(cpu.reg_read(r) for r in [UC_MIPS_REG_A0, UC_MIPS_REG_A1, UC_MIPS_REG_A2, UC_MIPS_REG_A3]),
                      bytes(cpu.mem_read((sp + 16) & 0x1FFFFFFF, 4))))

    for address in HELPERS:
        cpu.hook_add(UC_HOOK_CODE, hook, begin=address, end=address)
    nonreturn = name == 'convert' and args[0] == -0x80000000
    cpu.emu_start(ENTRIES[name], RETURN, count=3000 if nonreturn else 10000)
    pc = cpu.reg_read(UC_MIPS_REG_PC)
    if nonreturn:
        assert 0x800734AC <= pc <= 0x800734B8
        assert cpu.reg_read(UC_MIPS_REG_A0) == 0
        assert not trace
        return ('normalization loop', pc, cpu.reg_read(UC_MIPS_REG_S0 + 2))
    assert pc == RETURN, (name, args, hex(pc))
    assert cpu.reg_read(UC_MIPS_REG_SP) == STACK
    assert all(cpu.reg_read(r) == value for r, value in saved.items())
    memory = bytes(cpu.mem_read((OUTPUT - 16) & 0x1FFFFFFF, 40))
    if name == 'convert':
        actual = struct.pack('<II', cpu.reg_read(UC_MIPS_REG_V0), cpu.reg_read(UC_MIPS_REG_V1))
        assert actual == struct.pack('<d', float(args[0])), (args, actual.hex())
        assert memory == b'\xA5' * 40
        assert [event[0] for event in trace] == ([] if args[0] == 0 else ['shift'])
    else:
        actual = memory[16:24]
        assert actual == struct.pack('<Q', args[0] * args[1]), (args, actual.hex())
        assert memory[:16] == memory[24:] == b'\xA5' * 16
        assert cpu.reg_read(UC_MIPS_REG_V0) == OUTPUT
        assert [event[0] for event in trace] == ['add', 'add']
    return actual, trace, memory


def cases():
    rng = random.Random(0x738D8)
    values = {0, 1, -1, 0x7FFFFFFF, -0x7FFFFFFF, -0x80000000}
    for bit in range(31):
        for sign in [-1, 1]:
            for delta in [-1, 0, 1]:
                value = sign * (1 << bit) + delta
                if -0x80000000 <= value <= 0x7FFFFFFF:
                    values.add(value)
    values.update(rng.randint(-0x80000000, 0x7FFFFFFF) for _ in range(2048))
    for value in sorted(values):
        yield 'convert', [value]
    edges = [0, 1, 2, 0x7FFF, 0x8000, 0xFFFF, 0x10000, 0x10001,
             0x7FFFFFFF, 0x80000000, 0xFFFF0000, MASK32]
    for left in edges:
        for right in edges:
            yield 'multiply', [left, right]
    for bit in range(32):
        for other in range(32):
            yield 'multiply', [1 << bit, 1 << other]
    for _ in range(2048):
        yield 'multiply', [rng.getrandbits(32), rng.getrandbits(32)]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('convert', type=Path)
    parser.add_argument('multiply', type=Path)
    parser.add_argument('--retail', type=Path, default=Path('assets/USA/main.exe'))
    args = parser.parse_args()
    exe = args.retail.read_bytes()
    assert hashlib.sha1(exe).hexdigest() == '452fb033f2eaa4b18aa20a5bca60b8125af3a37b'
    code = {name: link_object(getattr(args, name), name) for name in ENTRIES}
    counts = dict.fromkeys(ENTRIES, 0)
    for name, values in cases():
        assert execute(exe, name, values) == execute(exe, name, values, code[name]), (name, values)
        counts[name] += 1
    print(f'PASS: {counts}; retail helpers execute unchanged; arithmetic, output bounds, SP and saved registers agree')
    print('INT_MIN: both bodies remain in the retail normalization loop during bounded execution.')


if __name__ == '__main__':
    main()
