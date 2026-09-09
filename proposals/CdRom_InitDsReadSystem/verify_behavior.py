#!/usr/bin/env python3
"""Compare ordinary-C CdRom_InitDsReadSystem against retail, modeling external APIs."""
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
with tempfile.TemporaryDirectory(prefix='ds-init-') as work:
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

def run(enabled, fill, api_result, candidate):
    cpu = Uc(UC_ARCH_MIPS, UC_MODE_MIPS32 | UC_MODE_LITTLE_ENDIAN)
    cpu.mem_map(0, 0x200000)
    start, size = struct.unpack_from('<II', exe, 0x18)
    cpu.mem_write(start & 0x1FFFFFFF, exe[0x800:0x800 + size])
    if candidate:
        for address, data in sections: cpu.mem_write(address, data)
    regions = ((0xB8AB0, 12), (0xA3510, 0x184))
    for address, size in regions: cpu.mem_write(address, bytes([fill]) * size)
    def snapshot(): return tuple(bytes(cpu.mem_read(a, n)) for a, n in regions)
    initial = snapshot()
    names = ('CdRom_IsDsReadSystemEnabled', 'CQ_clear_queue', 'CdRom_InitDsCallbacks',
             'CdRom_SetSyncCallback', 'CdRom_SetReadyCallback', 'CdRom_SetDispatchCallback',
             'CdRom_SetPollCallback', 'DS_read_cbready', 'DsReadCallback')
    callbacks = dict(zip(names[3:7], ('Render_StepParticleNode', 'Render_PlayParticleEffect',
                                      'CdRom_DispatchDsReadyCallback', 'CdRom_PollPendingDsRead')))
    apis = {int(symbols[name], 16) + 4: name for name in names}
    trapped = []; trace = []; queues = []
    def trap(u, address, size, user): trapped.append(address); u.emu_stop()
    for a in apis:
        cpu.mem_write((a - 4) & 0x1FFFFFFF, bytes(8))
        cpu.hook_add(UC_HOOK_CODE, trap, begin=a, end=a)
    cpu.reg_write(UC_MIPS_REG_SP, 0x801F0000)
    cpu.reg_write(UC_MIPS_REG_RA, 0x801E0000)
    pc = entries['CdRom_InitDsReadSystem'] if candidate else int(symbols['CdRom_InitDsReadSystem'], 16)
    for _ in range(20):
        cpu.emu_start(pc, 0x801E0000, count=2000)
        if not trapped: break
        name = apis[trapped.pop()]
        arg = cpu.reg_read(UC_MIPS_REG_A0)
        if name == 'CQ_clear_queue':
            assert arg == 0x800A3540 + len(queues) * 24
            queues.append(arg)
            trace.append((name, arg, snapshot()))
            cpu.mem_write(arg & 0x1FFFFFFF, bytes(24))
        elif name in callbacks:
            assert arg == int(symbols[callbacks[name]], 16)
            trace.append((name, arg, snapshot()))
        elif name == 'DsReadCallback':
            assert arg == 0
            trace.append((name, arg, snapshot()))
        else: trace.append((name, snapshot()))
        cpu.reg_write(UC_MIPS_REG_V0, enabled if name == names[0] else api_result)
        pc = cpu.reg_read(UC_MIPS_REG_RA)
    assert cpu.reg_read(UC_MIPS_REG_PC) == 0x801E0000
    assert cpu.reg_read(UC_MIPS_REG_SP) == 0x801F0000
    assert cpu.reg_read(UC_MIPS_REG_V0) == (0 if enabled else 1)
    expected_names = [names[0]] if enabled else [names[0]] + [names[1]] * 8 + list(names[2:])
    assert [t[0] for t in trace] == expected_names
    expected = [bytearray(x) for x in initial]
    if not enabled:
        expected[0][:] = bytes(12)
        for i in range(3):
            expected[1][i * 16:i * 16 + 13] = bytes(13)
        expected[1][0x30:0xF0] = bytes(0xC0)
        expected[1][0xF0:0xFC] = bytes(12)
        for i in range(8): expected[1][0x100+i*16:0x104+i*16] = bytes(4)
        expected[1][0x180:0x184] = bytes(4)
    assert snapshot() == tuple(bytes(x) for x in expected)
    return trace, snapshot()

for count, case in enumerate(itertools.product((0, 1, 0xFFFFFFFF), (0, 0xA5), (0, 1, 0xFFFFFFFF)), 1):
    assert run(*case, False) == run(*case, True), case
print(f'PASS {count} cases: enabled guard, all queue addresses, preserved padding, reset state at each API, callback identities, API order and return')
