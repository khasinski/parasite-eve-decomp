#!/usr/bin/env python3
"""Compare CD_sync C with retail MIPS under finite modeled CD FIFO traces."""
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
with tempfile.TemporaryDirectory(prefix='cd-sync-') as work:
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


def run(mode, initial, null_result, scenario, callbacks, candidate):
    cpu = Uc(UC_ARCH_MIPS, UC_MODE_MIPS32 | UC_MODE_LITTLE_ENDIAN)
    cpu.mem_map(0, 0x200000)
    start, size = struct.unpack_from('<II', exe, 0x18)
    cpu.mem_write(start & 0x1FFFFFFF, exe[0x800:0x800 + size])
    if candidate:
        for address, data in sections:
            cpu.mem_write(address, data)
    def word(address, value):
        cpu.mem_write(address, struct.pack('<I', value & 0xFFFFFFFF))
    word(0x9B27C, 0x80130000)
    word(0x9AFB4, 0x801D0000 if callbacks & 1 else 0)
    word(0x9AFB8, 0x801D0100 if callbacks & 2 else 0)
    cpu.mem_write(0x130000, b'\x03')
    cpu.mem_write(0x9AFD5, b'\x01')
    cpu.mem_write(0x9B294, bytes([initial, 1, 4]))
    cpu.mem_write(0xA3460, bytes(range(16)))
    cpu.mem_write(0x140100, b'\xA5' * 8)
    trace = []
    def access(u, kind, address, size, value, user):
        address &= 0x1FFFFFFF
        if address != 0x130000:
            return
        assert size == 1
        trace.append(('write' if kind == UC_MEM_WRITE else 'read', value if kind == UC_MEM_WRITE else u.mem_read(address, 1)[0]))
    cpu.hook_add(UC_HOOK_MEM_READ | UC_HOOK_MEM_WRITE, access)
    api = {(entries.get(name, int(symbols[name], 16)) if candidate else int(symbols[name], 16)): name for name in ('VSync', 'CheckCallback', 'getintr', 'CD_flush', 'puts', 'printf')}
    api.update({0x801D0000: 'sync_callback', 0x801D0100: 'ready_callback'})
    state = {'vsync': 0, 'interrupt': 0}
    def call(u, address):
        name = api[address]
        args = [u.reg_read(r) for r in (UC_MIPS_REG_A0, UC_MIPS_REG_A1, UC_MIPS_REG_A2, UC_MIPS_REG_A3)]
        argc = {'VSync': 1, 'CheckCallback': 0, 'getintr': 0, 'CD_flush': 0, 'puts': 1, 'printf': 4, 'sync_callback': 2, 'ready_callback': 2}[name]
        recorded = tuple(args[:argc])
        if name == 'printf':
            recorded += (struct.unpack('<I', u.mem_read((u.reg_read(UC_MIPS_REG_SP) & 0x1FFFFFFF) + 16, 4))[0],)
        trace.append((name, recorded))
        value = 0
        if name == 'VSync':
            assert args[0] == 0xFFFFFFFF
            state['vsync'] += 1
            value = 100
            if scenario == 'deadline' and state['vsync'] >= 2:
                value = 1061
            if scenario == 'deadline_boundary' and state['vsync'] >= 2:
                value = 1060 if state['vsync'] == 2 else 1061
            if scenario in ('counter', 'counter_boundary') and state['vsync'] == 2:
                word(0xA347C, 0x3C0001 if scenario == 'counter' else 0x3C0000)
            if scenario == 'poll' and state['vsync'] == 3:
                u.mem_write(0x9B294, b'\x02')
        elif name == 'CheckCallback':
            value = int(scenario in ('dispatch', 'error'))
        elif name == 'getintr':
            sequence = (6, 0) if scenario == 'error' else (4, 2, 0)
            value = sequence[state['interrupt']]
            state['interrupt'] += 1
            if value & 4:
                u.mem_write(0x9B295, b'\x01')
                u.mem_write(0xA3468, b'READY123')
            if value & 2:
                u.mem_write(0x9B294, b'\x05' if scenario == 'error' else b'\x02')
                u.mem_write(0xA3460, b'RESULT12')
        elif name == 'CD_flush':
            u.mem_write(0x9B294, b'\x02\0\0')
        u.reg_write(UC_MIPS_REG_V0, value)
        u.reg_write(UC_MIPS_REG_PC, u.reg_read(UC_MIPS_REG_RA))
    trapped = []
    def trap(u, address, size, user):
        trapped.append(address)
        u.emu_stop()
    for address in api:
        cpu.hook_add(UC_HOOK_CODE, trap, begin=address, end=address)
    cpu.reg_write(UC_MIPS_REG_A0, mode & 0xFFFFFFFF)
    cpu.reg_write(UC_MIPS_REG_A1, 0 if null_result else 0x80140100)
    cpu.reg_write(UC_MIPS_REG_SP, 0x801F0000)
    cpu.reg_write(UC_MIPS_REG_RA, 0x801E0000)
    pc = entries['CD_sync'] if candidate else 0x8007B010
    for step in range(50):
        cpu.emu_start(pc, 0x801E0000, count=10000)
        if not trapped:
            break
        call(cpu, trapped.pop())
        pc = cpu.reg_read(UC_MIPS_REG_PC)
    assert cpu.reg_read(UC_MIPS_REG_PC) == 0x801E0000
    result = cpu.reg_read(UC_MIPS_REG_V0)
    assert result in (0, 2, 5, 0xFFFFFFFF)
    assert state['vsync'] >= 2  # Even nonblocking and already-completed calls check timeout first.
    assert any(item[0] == 'CD_flush' for item in trace) == (result == 0xFFFFFFFF)
    if result in (2, 5):
        assert cpu.mem_read(0x9B294, 1)[0] == 2
        if not null_result:
            assert cpu.mem_read(0x140100, 8) == cpu.mem_read(0xA3460, 8)
    else:
        assert cpu.mem_read(0x140100, 8) == b'\xA5' * 8
    memory = tuple(bytes(cpu.mem_read(address, size)) for address, size in ((0x9B294, 3), (0xA3460, 16), (0xA3478, 12), (0x140100, 8)))
    return result, trace, memory

cases = itertools.product((0, 1, -1), (0, 1, 2, 3, 5), (False, True), ('poll', 'dispatch', 'error', 'deadline', 'counter', 'deadline_boundary', 'counter_boundary'), range(4))
for count, case in enumerate(cases, 1):
    assert run(*case, False) == run(*case, True), case
print(f'PASS {count} cases: status consumption, responses, callback/MMIO traces, mode and timeout boundaries')
