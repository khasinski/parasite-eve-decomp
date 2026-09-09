#!/usr/bin/env python3
"""Finite retail-MIPS VSync comparisons with modeled counters, GPU status and v_wait."""
import hashlib
import itertools
from pathlib import Path
import struct
import subprocess
import sys
import tempfile
from elftools.elf.elffile import ELFFile
from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN, UC_HOOK_CODE, UC_HOOK_MEM_READ
from unicorn.mips_const import UC_MIPS_REG_A0, UC_MIPS_REG_A1, UC_MIPS_REG_V0, UC_MIPS_REG_SP, UC_MIPS_REG_RA, UC_MIPS_REG_PC

exe = Path('build/USA/main.exe').read_bytes()
assert hashlib.sha1(exe).hexdigest() == '452fb033f2eaa4b18aa20a5bca60b8125af3a37b'
with tempfile.TemporaryDirectory(prefix='vsync-behavior-') as work:
    script = Path(work) / 'candidate.ld'
    linked = Path(work) / 'candidate.elf'
    script.write_text('''D_80094574 = 0x80094574; D_80094578 = 0x80094578;
D_8009457C = 0x8009457C; D_80094580 = 0x80094580;
g_VSyncCount = 0x800956AC; v_wait = 0x80073BBC;
SECTIONS { .text 0x80073A44 : { *(.text) } /DISCARD/ : { *(.reginfo) *(.MIPS.abiflags) *(.pdr) *(.comment) *(.gnu.attributes) } }
''')
    subprocess.run(['mipsel-none-elf-ld', '-T', str(script), sys.argv[1], '-o', str(linked)], check=True)
    with linked.open('rb') as stream:
        code = ELFFile(stream).get_section_by_name('.text').data()


def run(mode, status, unstable, previous, candidate):
    cpu = Uc(UC_ARCH_MIPS, UC_MODE_MIPS32 | UC_MODE_LITTLE_ENDIAN)
    cpu.mem_map(0, 0x200000)
    start, size = struct.unpack_from('<II', exe, 0x18)
    cpu.mem_write(start & 0x1FFFFFFF, exe[0x800:0x800 + size])
    if candidate:
        cpu.mem_write(0x73A44, code)
    def write(address, value):
        cpu.mem_write(address, struct.pack('<I', value & 0xFFFFFFFF))
    def read(address):
        return struct.unpack('<I', cpu.mem_read(address, 4))[0]
    write(0x94574, 0x80130000)
    write(0x94578, 0x80130004)
    write(0x9457C, previous)
    write(0x94580, 8)
    write(0x956AC, 10)
    trace = []
    accesses = [0, 0]
    samples = [100, 101, 102, 102, 110, 111, 111, 111] if unstable else [102] * 8
    def counter(u, kind, address, size, value, user):
        address &= 0x1FFFFFFF
        if address not in (0x130000, 0x130004):
            return
        index = (address - 0x130000) // 4
        assert size == 4
        if index == 0:
            value = status if accesses[0] < 2 else status ^ 0x80000000
        else:
            value = samples[min(accesses[1], len(samples) - 1)]
        accesses[index] += 1
        write(address, value)
        trace.append(('read', index, value))
    cpu.hook_add(UC_HOOK_MEM_READ, counter)
    def wait(u, address, size, user):
        target, frames = [u.reg_read(reg) for reg in (UC_MIPS_REG_A0, UC_MIPS_REG_A1)]
        trace.append(('wait', target, frames))
        write(0x956AC, max(read(0x956AC), target))
        u.reg_write(UC_MIPS_REG_V0, 0)
        u.reg_write(UC_MIPS_REG_PC, u.reg_read(UC_MIPS_REG_RA))
    cpu.hook_add(UC_HOOK_CODE, wait, begin=0x80073BBC, end=0x80073BBC)
    cpu.reg_write(UC_MIPS_REG_A0, mode & 0xFFFFFFFF)
    cpu.reg_write(UC_MIPS_REG_SP, 0x801F0000)
    cpu.reg_write(UC_MIPS_REG_RA, 0x801E0000)
    cpu.emu_start(0x80073A44, 0x801E0000, count=10000)
    assert cpu.reg_read(UC_MIPS_REG_PC) == 0x801E0000
    assert accesses[0] >= 1 and accesses[1] >= 2
    assert sum(item[0] == 'wait' for item in trace) == (0 if mode < 0 or mode == 1 else 2)
    return cpu.reg_read(UC_MIPS_REG_V0), read(0x9457C), read(0x94580), trace

cases = itertools.product((-1, 0, 1, 2, 5), (0, 0x400000, 0x80400000, 0x80000), (False, True), (90, 65530))
for count, case in enumerate(cases, 1):
    expected = run(*case, False)
    assert expected == run(*case, True), case
    assert expected[0] == (10 if case[0] < 0 else (102 - case[3]) & 0xFFFF)
print(f'PASS {count} cases: return values, counter state, GPU/counter reads and wait calls')
