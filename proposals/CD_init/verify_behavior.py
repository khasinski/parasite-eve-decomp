#!/usr/bin/env python3
"""Compare CD_init C with retail MIPS under finite modeled CD FIFO traces."""
import hashlib
import itertools
from pathlib import Path
import re
import struct
import subprocess
import sys
import tempfile
from elftools.elf.elffile import ELFFile
from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN, UC_HOOK_CODE, UC_HOOK_MEM_READ, UC_HOOK_MEM_WRITE, UC_MEM_WRITE
from unicorn.mips_const import UC_MIPS_REG_A0, UC_MIPS_REG_A1, UC_MIPS_REG_A2, UC_MIPS_REG_A3, UC_MIPS_REG_V0, UC_MIPS_REG_SP, UC_MIPS_REG_RA, UC_MIPS_REG_PC

exe = Path('build/USA/main.exe').read_bytes()
assert hashlib.sha1(exe).hexdigest() == '452fb033f2eaa4b18aa20a5bca60b8125af3a37b'
with tempfile.TemporaryDirectory(prefix='cd-init-') as work:
    script, linked = Path(work) / 'candidate.ld', Path(work) / 'candidate.elf'
    undefined = subprocess.check_output(['mipsel-none-elf-nm', '-u', sys.argv[1]], text=True).split()[1::2]
    symbols = dict(re.findall(r'^(\w+) = (0x[0-9A-Fa-f]+);', Path('configs/USA/sym.main.txt').read_text(), re.M))
    symbols.update(re.findall(r'^(\w+) = (0x[0-9A-Fa-f]+);', Path('linkers/USA/undefined_syms_manual.txt').read_text(), re.M))
    definitions = [f'{name} = ' + ('0x' + name[2:] if name.startswith('D_') else symbols[name]) + ';' for name in undefined]
    script.write_text('\n'.join(definitions) + '\nSECTIONS { .text 0x80150000 : { *(.text) } .rodata 0x80140000 : { *(.rodata*) } /DISCARD/ : { *(.reginfo) *(.MIPS.abiflags) *(.pdr) *(.comment) *(.gnu.attributes) } }')
    subprocess.run(['mipsel-none-elf-ld', '-T', str(script), sys.argv[1], '-o', str(linked)], check=True)
    with linked.open('rb') as stream:
        elf = ELFFile(stream)
        entries = {symbol.name: symbol['st_value'] for symbol in elf.get_section_by_name('.symtab').iter_symbols()}
        sections = [(s['sh_addr'] & 0x1FFFFFFF, s.data()) for s in elf.iter_sections() if s.name in ('.text', '.rodata')]

def run(pending, status, fail_command, sync_result, candidate):
    cpu = Uc(UC_ARCH_MIPS, UC_MODE_MIPS32 | UC_MODE_LITTLE_ENDIAN)
    cpu.mem_map(0, 0x200000)
    start, size = struct.unpack_from('<II', exe, 0x18)
    cpu.mem_write(start & 0x1FFFFFFF, exe[0x800:0x800 + size])
    if candidate:
        for address, data in sections:
            cpu.mem_write(address, data)
    def word(address, value):
        cpu.mem_write(address, struct.pack('<I', value & 0xFFFFFFFF))
    for offset, address in enumerate((0x9B27C, 0x9B284, 0x9B288)):
        word(address, 0x80130000 + offset)
    word(0x9B28C, 0x80130004)
    cpu.mem_write(0x9AFB4, b'\xA5' * 8)
    cpu.mem_write(0x9AFC4, b'\xA5' * 8)
    cpu.mem_write(0x9AFD4, b'\xA5' * 2)
    cpu.mem_write(0x9B294, b'\xA5' * 3)
    trace = []
    reads = [0]
    def access(u, kind, address, size, value, user):
        address &= 0x1FFFFFFF
        if address not in (0x130000, 0x130001, 0x130002, 0x130004):
            return
        assert size == (4 if address == 0x130004 else 1)
        if kind == UC_MEM_WRITE:
            trace.append(('write', address, value))
        else:
            assert address == 0x130002
            value = 7 if reads[0] < pending else 0
            reads[0] += 1
            u.mem_write(address, bytes([value]))
            trace.append(('read', address, value))
    cpu.hook_add(UC_HOOK_MEM_READ | UC_HOOK_MEM_WRITE, access)
    api = {(entries.get(name, int(symbols[name], 16)) if candidate else int(symbols[name], 16)): name for name in ('puts', 'printf', 'ResetCallback', 'InterruptCallback', 'CD_cw', 'CD_sync')}
    def call(u, address, size, user):
        name = api[address]
        args = [u.reg_read(r) for r in (UC_MIPS_REG_A0, UC_MIPS_REG_A1, UC_MIPS_REG_A2, UC_MIPS_REG_A3)]
        argc = {'puts': 1, 'printf': 2, 'ResetCallback': 0, 'InterruptCallback': 2, 'CD_cw': 4, 'CD_sync': 2}[name]
        trace.append((name, tuple(args[:argc])))
        result = 0
        if name == 'ResetCallback':
            assert bytes(u.mem_read(0x9AFB4, 8)) == b'\0' * 8
            assert bytes(u.mem_read(0x9AFC4, 8)) == b'\0' * 8
            assert bytes(u.mem_read(0x9AFD4, 2)) == b'\0' * 2
        if name == 'InterruptCallback':
            assert args[:2] == [2, int(symbols['Cd_SetIntrMask'], 16)]
        if name == 'CD_cw':
            assert args[1:] == [0, 0, 0]
            assert bytes(u.mem_read(0x9B294, 3)) == b'\x02\0\0'
            result = int(args[0] == fail_command)
            if args[0] == 1:
                word(0x9AFC4, status)
        if name == 'CD_sync':
            assert args[:2] == [0, 0]
            result = sync_result
        u.reg_write(UC_MIPS_REG_V0, result & 0xFFFFFFFF)
        u.reg_write(UC_MIPS_REG_PC, u.reg_read(UC_MIPS_REG_RA))
    trapped = []
    def trap(u, address, size, user):
        trapped.append(address)
        u.emu_stop()
    for address in api:
        cpu.hook_add(UC_HOOK_CODE, trap, begin=address, end=address)
    cpu.reg_write(UC_MIPS_REG_SP, 0x801F0000)
    cpu.reg_write(UC_MIPS_REG_RA, 0x801E0000)
    pc = entries['CD_init'] if candidate else 0x8007BBFC
    for step in range(32):
        cpu.emu_start(pc, 0x801E0000, count=10000)
        if not trapped:
            break
        call(cpu, trapped.pop(), 4, None)
        pc = cpu.reg_read(UC_MIPS_REG_PC)
    assert cpu.reg_read(UC_MIPS_REG_PC) == 0x801E0000
    assert reads[0] == pending + 1
    assert ('write', 0x130004, 0x1325) in trace
    commands = [args[0] for name, *rest in trace if name == 'CD_cw' for args in rest]
    expected_commands = [1] * (2 if status & 0x10 else 1) + [10]
    if fail_command != 10:
        expected_commands += [12]
    assert commands == expected_commands, (commands, expected_commands, trace)
    result = cpu.reg_read(UC_MIPS_REG_V0)
    assert result == (0 if fail_command not in (10, 12) and sync_result == 2 else 0xFFFFFFFF)
    return result, trace, bytes(cpu.mem_read(0x9AFB4, 8)), bytes(cpu.mem_read(0x9AFC4, 8)), bytes(cpu.mem_read(0x9AFD4, 2)), bytes(cpu.mem_read(0x9B294, 3))

cases = itertools.product((0, 1, 7), (0, 0x10, 0x12340000, 0x12340010), (0, 1, 10, 12), (-1, 0, 1, 2, 5))
for count, case in enumerate(cases, 1):
    assert run(*case, False) == run(*case, True), case
print(f'PASS {count} cases: return, reset state, interrupt registration, MMIO, command order and failure paths')
