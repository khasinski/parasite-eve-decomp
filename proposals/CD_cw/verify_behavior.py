#!/usr/bin/env python3
"""Compare CD_cw C with retail MIPS under finite modeled CD FIFO traces."""
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

real_intr = '--real-intr' in sys.argv[2:]
real_sync = '--real-sync' in sys.argv[2:] or real_intr
exe = Path('build/USA/main.exe').read_bytes()
assert hashlib.sha1(exe).hexdigest() == '452fb033f2eaa4b18aa20a5bca60b8125af3a37b'
with tempfile.TemporaryDirectory(prefix='cd-cw-') as work:
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
        text_section = elf.get_section_by_name('.text')
        for required in (['CD_sync'] if real_sync else []) + (['getintr'] if real_intr else []):
            assert text_section['sh_addr'] <= entries[required] < text_section['sh_addr'] + text_section['sh_size'], required
        sections = [(s['sh_addr'] & 0x1FFFFFFF, s.data()) for s in elf.iter_sections() if s.name in ('.text', '.rodata')]
def run(command, null_parameter, null_result, mode, scenario, callbacks, debug, candidate):
    cpu = Uc(UC_ARCH_MIPS, UC_MODE_MIPS32 | UC_MODE_LITTLE_ENDIAN)
    cpu.mem_map(0, 0x200000)
    start, size = struct.unpack_from('<II', exe, 0x18)
    cpu.mem_write(start & 0x1FFFFFFF, exe[0x800:0x800 + size])
    if candidate:
        for address, data in sections:
            cpu.mem_write(address, data)
    def word(address, value):
        cpu.mem_write(address, struct.pack('<I', value & 0xFFFFFFFF))
    count = {1: 0, 2: 3, 14: 1}[command & 255]
    word(0x9B1FC + (command & 255) * 4, count)
    word(0x9B0FC + (command & 255) * 4, command & 1)
    word(0x9B288, 0x80130003)
    word(0x9AFC0, debug)
    word(0x9AFB4, 0x801D0000 if callbacks & 1 else 0)
    word(0x9AFB8, 0x801D0100 if callbacks & 2 else 0)
    for offset, address in enumerate((0x9B27C, 0x9B280, 0x9B284)):
        word(address, 0x80130000 + offset)
    cpu.mem_write(0x9AFD0, bytes(range(0xA0, 0xA6)))
    cpu.mem_write(0x9B294, b'\x05\x04\x04' if real_sync else b'\x02\x04\x04')
    cpu.mem_write(0xA3460, bytes(range(16)))
    cpu.mem_write(0x140100, b'\xA5' * 8)
    cpu.mem_write(0x140200, b'\x12\x34\x56\x78')
    trace = []
    def access(u, kind, address, size, value, user):
        address &= 0x1FFFFFFF
        if not 0x130000 <= address <= 0x130003:
            return
        assert size == 1
        if kind == UC_MEM_WRITE:
            if real_sync and address == 0x130001:
                state.update(phase=1, vsync=0, interrupt=0, irq=0, fifo=0)
            if real_intr and address == 0x130003 and value == 7:
                state['irq'] += 1
                state['fifo'] = 0
            trace.append(('write', address, value))
        else:
            value = u.mem_read(address, 1)[0]
            if real_intr:
                queue = (5,) if scenario == 'error' else (1, 2)
                pending = state['phase'] and state['irq'] < len(queue)
                if address == 0x130000:
                    value = (value & 3) | (0x20 if pending and state['fifo'] < 8 else 0)
                elif address == 0x130001:
                    assert pending and state['fifo'] < 8
                    response = bytes([0x1D if scenario == 'error' else 0]) + bytes(range(1, 8))
                    value = response[state['fifo']]
                    state['fifo'] += 1
                elif address == 0x130003:
                    value = queue[state['irq']] if pending else 0
                u.mem_write(address, bytes([value]))
            trace.append(('read', address, value))
    cpu.hook_add(UC_HOOK_MEM_READ | UC_HOOK_MEM_WRITE, access)
    names = ('puts', 'printf', 'CD_sync', 'VSync', 'CheckCallback', 'getintr', 'CD_flush')
    if real_sync:
        names = tuple(name for name in names if name != 'CD_sync')
    if real_intr:
        names = tuple(name for name in names if name != 'getintr')
    api = {(entries.get(name, int(symbols[name], 16)) if candidate else int(symbols[name], 16)): name for name in names}
    api.update({0x801D0000: 'sync_callback', 0x801D0100: 'ready_callback'})
    state = {'vsync': 0, 'interrupt': 0, 'phase': 0, 'irq': 0, 'fifo': 0}
    def call(u, address):
        name = api[address]
        args = [u.reg_read(r) for r in (UC_MIPS_REG_A0, UC_MIPS_REG_A1, UC_MIPS_REG_A2, UC_MIPS_REG_A3)]
        argc = {'puts': 1, 'CD_sync': 2, 'VSync': 1, 'CheckCallback': 0, 'getintr': 0, 'CD_flush': 0, 'sync_callback': 2, 'ready_callback': 2}.get(name)
        if name == 'printf':
            argc = {0x80011B44: 1, 0x80011B50: 4, 0x80011B28: 4}.get(args[0], 2)
        recorded = tuple(args[:argc])
        if name == 'printf' and args[0] == 0x80011B28:
            recorded += (struct.unpack('<I', u.mem_read((u.reg_read(UC_MIPS_REG_SP) & 0x1FFFFFFF) + 16, 4))[0],)
        trace.append((name, recorded))
        value = 0
        if name == 'CD_sync':
            assert args[:2] == [0, 0]
            value = 5  # The initial synchronization result is deliberately ignored.
        elif name == 'VSync':
            assert args[0] == 0xFFFFFFFF
            value = 100
            if not real_sync or state['phase']:
                state['vsync'] += 1
                value = 100
                if scenario == 'deadline' and state['vsync'] == 2:
                    value = 1061
                if scenario == 'deadline_boundary' and state['vsync'] >= 2:
                    value = 1060 if state['vsync'] == 2 else 1061
                if scenario == 'counter_boundary' and state['vsync'] == 2:
                    word(0xA347C, 0x3C0000)
                if scenario == 'counter' and state['vsync'] == 2:
                    word(0xA347C, 0x3C0001)
                if scenario == 'immediate' or scenario == 'poll' and state['vsync'] == 3:
                    u.mem_write(0x9B294, b'\x02')
        elif name == 'CheckCallback':
            value = int((not real_sync or state['phase']) and scenario in ('dispatch', 'error'))
            if value:
                u.mem_write(0x130000, b'\x03')
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
    for reg, value in zip((UC_MIPS_REG_A0, UC_MIPS_REG_A1, UC_MIPS_REG_A2, UC_MIPS_REG_A3), (command, 0 if null_parameter else 0x80140200, 0 if null_result else 0x80140100, mode)):
        cpu.reg_write(reg, value)
    cpu.reg_write(UC_MIPS_REG_SP, 0x801F0000)
    cpu.reg_write(UC_MIPS_REG_RA, 0x801E0000)
    pc = entries['CD_cw'] if candidate else 0x8007B558
    for step in range(50):
        cpu.emu_start(pc, 0x801E0000, count=10000)
        if not trapped:
            break
        call(cpu, trapped.pop())
        pc = cpu.reg_read(UC_MIPS_REG_PC)
    assert cpu.reg_read(UC_MIPS_REG_PC) == 0x801E0000
    rejected = count and null_parameter
    if real_intr and not rejected and not mode and scenario in ('dispatch', 'error'):
        assert state['irq'] == (1 if scenario == 'error' else 2)
    expected = -2 if rejected else -1 if not mode and scenario in ('error', 'deadline', 'counter', 'deadline_boundary', 'counter_boundary') else 0
    assert cpu.reg_read(UC_MIPS_REG_V0) == expected & 0xFFFFFFFF
    if real_sync and not rejected:
        issued = next(i for i, item in enumerate(trace) if item[:2] == ('write', 0x130001))
        assert sum(item[0] == 'VSync' for item in trace[:issued]) == 2
        assert sum(item[0] == 'CheckCallback' for item in trace[:issued]) == 1
    before_command = trace if rejected else trace[:next(i for i, item in enumerate(trace) if item[:2] == ('write', 0x130001))]
    writes = [item for item in before_command if item[0] == 'write']
    assert len([item for item in writes if item[1] == 0x130002]) == (0 if rejected else count)
    assert any(item[0] == 'CD_flush' for item in trace) == (not rejected and not mode and scenario in ('deadline', 'counter', 'deadline_boundary', 'counter_boundary'))
    memory = tuple(bytes(cpu.mem_read(address, size)) for address, size in ((0x9AFD0, 6), (0x9B294, 3), (0x9AFC4, 12), (0xA3460, 24), (0xA3478, 12), (0x140100, 8)))
    return cpu.reg_read(UC_MIPS_REG_V0), trace, memory

cases = itertools.product((1, 2, 14, 0x102), (False, True), (False, True), (0, 1), ('immediate', 'poll', 'dispatch', 'error', 'deadline', 'counter', 'deadline_boundary', 'counter_boundary'), range(4), (0, 1, 2))
for count, case in enumerate(cases, 1):
    assert run(*case, False) == run(*case, True), case
print(f"PASS {count} cases ({'executed CD_sync/getintr' if real_intr else 'executed CD_sync' if real_sync else 'modeled CD_sync'}): parameter validation, state, FIFO, callbacks, timeout paths and results")
