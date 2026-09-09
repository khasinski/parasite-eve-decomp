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


def run(pending, lid_open, fail_command, candidate):
    cpu = Uc(UC_ARCH_MIPS, UC_MODE_MIPS32 | UC_MODE_LITTLE_ENDIAN)
    cpu.mem_map(0, 0x200000)
    start, size = struct.unpack_from('<II', exe, 0x18)
    cpu.mem_write(start & 0x1FFFFFFF, exe[0x800:0x800 + size])
    if candidate:
        for address, data in sections:
            cpu.mem_write(address, data)
        for name in ('CD_init', 'CD_cw', 'CD_sync', 'getintr'):
            assert 0x80150000 <= entries[name] < 0x80160000
    def word(address, value):
        cpu.mem_write(address, struct.pack('<I', value & 0xFFFFFFFF))
    for address, port in ((0x9B27C, 0), (0x9B280, 1), (0x9B284, 2), (0x9B288, 3), (0x9B28C, 4)):
        word(address, 0x80130000 + port)
    word(0x9AFC0, 0)
    word(0x9AFCC, 0)
    for address, size in ((0x9AFB4, 8), (0x9AFC4, 8), (0x9AFD4, 2), (0x9B294, 3), (0xA3460, 24)):
        cpu.mem_write(address, b'\xA5' * size)
    state = dict(bank=0, pending=pending, irq=0, fifo=0, response=b'')
    trace, commands = [], []
    def access(u, kind, address, size, value, user):
        address &= 0x1FFFFFFF
        if not 0x130000 <= address <= 0x130004:
            return
        port = address - 0x130000
        assert size == (4 if port == 4 else 1)
        if kind == UC_MEM_WRITE:
            if port == 0:
                state['bank'] = value & 3
            elif port == 1:
                assert state['bank'] == 0 and not state['irq']
                commands.append(value)
                status = int(value == fail_command)
                if lid_open and len(commands) == 1:
                    status |= 0x10
                state.update(irq=5 if value == fail_command else 2, fifo=0, response=bytes([status, 1, 2, 3, 4, 5, 6, 7]))
            elif port == 3 and value == 7:
                if state['irq']:
                    assert state['fifo'] == 8
                    state['irq'] = 0
                else:
                    assert state['pending'] > 0
                    state['pending'] -= 1
            trace.append(('write', port, value))
        else:
            if port == 0:
                value = state['bank'] | (0x20 if state['irq'] and state['fifo'] < 8 else 0)
            elif port == 1:
                assert state['irq'] and state['fifo'] < 8
                value = state['response'][state['fifo']]
                state['fifo'] += 1
            else:
                assert port == 3
                value = state['irq'] or int(state['pending'] > 0)
            u.mem_write(address, bytes([value]))
            trace.append(('read', port, value))
    cpu.hook_add(UC_HOOK_MEM_READ | UC_HOOK_MEM_WRITE, access)
    api = {int(symbols[name], 16): name for name in ('puts', 'printf', 'ResetCallback', 'InterruptCallback', 'VSync', 'CheckCallback')}
    trapped = []
    def trap(u, address, size, user):
        trapped.append(address)
        u.emu_stop()
    for address in api:
        cpu.hook_add(UC_HOOK_CODE, trap, begin=address, end=address)
    cpu.reg_write(UC_MIPS_REG_SP, 0x801F0000)
    cpu.reg_write(UC_MIPS_REG_RA, 0x801E0000)
    pc = entries['CD_init'] if candidate else 0x8007BBFC
    for step in range(200):
        cpu.emu_start(pc, 0x801E0000, count=100000)
        if not trapped:
            break
        name = api[trapped.pop()]
        args = [cpu.reg_read(r) for r in (UC_MIPS_REG_A0, UC_MIPS_REG_A1)]
        argc = {'puts': 1, 'printf': 2, 'ResetCallback': 0, 'InterruptCallback': 2, 'VSync': 1, 'CheckCallback': 0}[name]
        trace.append((name, tuple(args[:argc])))
        if name == 'ResetCallback':
            assert bytes(cpu.mem_read(0x9AFB4, 8)) == bytes(8)
            assert bytes(cpu.mem_read(0x9AFC4, 8)) == bytes(8)
        if name == 'InterruptCallback':
            assert args == [2, int(symbols['Cd_SetIntrMask'], 16)]
        if name == 'VSync':
            assert args[0] == 0xFFFFFFFF
        cpu.reg_write(UC_MIPS_REG_V0, 100 if name == 'VSync' else int(name == 'CheckCallback'))
        pc = cpu.reg_read(UC_MIPS_REG_RA)
    assert cpu.reg_read(UC_MIPS_REG_PC) == 0x801E0000
    assert cpu.reg_read(UC_MIPS_REG_SP) == 0x801F0000
    expected = [1] * (2 if lid_open else 1) + [10] + ([] if fail_command == 10 else [12])
    assert commands == expected, (commands, expected)
    assert state['pending'] == state['irq'] == 0
    result = cpu.reg_read(UC_MIPS_REG_V0)
    assert result == (0xFFFFFFFF if fail_command in (10, 12) else 0)
    memory = tuple(bytes(cpu.mem_read(a, n)) for a, n in ((0x9AFB4, 8), (0x9AFC4, 18), (0x9B294, 3), (0xA3460, 24), (0xA3478, 12)))
    return result, trace, memory

for count, case in enumerate(itertools.product((0, 1, 7), (False, True), (0, 1, 10, 12)), 1):
    assert run(*case, False) == run(*case, True), case
print(f'PASS {count} cases: real CD_init/CD_cw/CD_sync/getintr, FIFO, command failures, lid status and pending IRQs')
