#!/usr/bin/env python3
"""Compare retail SpuSetIRQ with candidate C under finite SPU control-bit traces."""
import hashlib
import itertools
from pathlib import Path
import struct
import subprocess
import sys
import tempfile
from elftools.elf.elffile import ELFFile
from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN, UC_HOOK_CODE, UC_HOOK_MEM_READ, UC_HOOK_MEM_WRITE, UC_MEM_WRITE
from unicorn.mips_const import UC_MIPS_REG_A0, UC_MIPS_REG_A1, UC_MIPS_REG_V0, UC_MIPS_REG_SP, UC_MIPS_REG_RA, UC_MIPS_REG_PC

exe = Path('build/USA/main.exe').read_bytes()
assert hashlib.sha1(exe).hexdigest() == '452fb033f2eaa4b18aa20a5bca60b8125af3a37b'
with tempfile.TemporaryDirectory(prefix='spu-irq-') as work:
    script = Path(work) / 'candidate.ld'
    linked = Path(work) / 'candidate.elf'
    script.write_text('''_spu_RXX = 0x8009B3FC;
D_8001208C = 0x8001208C; D_8001209C = 0x8001209C; D_800120AC = 0x800120AC;
printf = 0x80071A74;
SECTIONS { .text 0x80085C44 : { *(.text) } /DISCARD/ : { *(.reginfo) *(.MIPS.abiflags) *(.pdr) *(.comment) *(.gnu.attributes) } }
''')
    subprocess.run(['mipsel-none-elf-ld', '-T', str(script), sys.argv[1], '-o', str(linked)], check=True)
    with linked.open('rb') as stream:
        code = ELFFile(stream).get_section_by_name('.text').data()


def run(mode, delays, initial, candidate):
    cpu = Uc(UC_ARCH_MIPS, UC_MODE_MIPS32 | UC_MODE_LITTLE_ENDIAN)
    cpu.mem_map(0, 0x200000)
    start, size = struct.unpack_from('<II', exe, 0x18)
    cpu.mem_write(start & 0x1FFFFFFF, exe[0x800:0x800 + size])
    if candidate:
        cpu.mem_write(0x85C44, code)
    cpu.mem_write(0x9B3FC, struct.pack('<I', 0x80130000))
    cpu.mem_write(0x1301AA, struct.pack('<H', initial))
    trace = []
    state = {'phase': -1, 'reads': 0, 'written': initial}
    def access(u, kind, address, size, value, user):
        if address & 0x1FFFFFFF != 0x1301AA:
            return
        assert size == 2
        if kind == UC_MEM_WRITE:
            state['phase'] += 1
            state['reads'] = 0
            state['written'] = value
            trace.append(('write', value))
        else:
            value = state['written']
            if state['phase'] >= 0:
                if state['reads'] < delays[min(state['phase'], 1)]:
                    value ^= 0x40
                state['reads'] += 1
            u.mem_write(address & 0x1FFFFFFF, struct.pack('<H', value))
            trace.append(('read', value))
    cpu.hook_add(UC_HOOK_MEM_READ | UC_HOOK_MEM_WRITE, access)
    def text(address):
        result = bytearray()
        while True:
            byte = cpu.mem_read(address & 0x1FFFFFFF, 1)[0]
            if not byte:
                return bytes(result)
            result.append(byte)
            address += 1
            assert len(result) < 256
    def call(u, address, size, user):
        trace.append(('printf', text(u.reg_read(UC_MIPS_REG_A0)), text(u.reg_read(UC_MIPS_REG_A1))))
        u.reg_write(UC_MIPS_REG_V0, 0)
        u.reg_write(UC_MIPS_REG_PC, u.reg_read(UC_MIPS_REG_RA))
    cpu.hook_add(UC_HOOK_CODE, call, begin=0x80071A74, end=0x80071A74)
    cpu.reg_write(UC_MIPS_REG_A0, mode & 0xFFFFFFFF)
    cpu.reg_write(UC_MIPS_REG_SP, 0x801F0000)
    cpu.reg_write(UC_MIPS_REG_RA, 0x801E0000)
    cpu.emu_start(0x80085C44, 0x801E0000, count=200000)
    assert cpu.reg_read(UC_MIPS_REG_PC) == 0x801E0000
    if mode in (0, 1, 3):
        assert trace and any(item[0] == "write" for item in trace)
    return cpu.reg_read(UC_MIPS_REG_V0), trace

cases = itertools.product((0, 1, 3, 2, -1), ((0, 0), (1, 2), (5, 7), (3840, 0), (3841, 0), (0, 3841)), (0, 0xFFFF))
for count, (mode, delays, initial) in enumerate(cases, 1):
    expected = run(mode, delays, initial, False)
    assert expected == run(mode, delays, initial, True), (mode, delays, initial)
    timeout = (mode in (0, 1, 3) and delays[0] > 3840) or (mode == 3 and delays[1] > 3840)
    assert expected[0] == (0xFFFFFFFF if timeout else mode & 0xFFFFFFFF)
    assert any(item[0] == 'printf' for item in expected[1]) == timeout
print(f'PASS {count} cases: return values, every control-register read/write, and diagnostics')
