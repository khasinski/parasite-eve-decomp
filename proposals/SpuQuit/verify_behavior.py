#!/usr/bin/env python3
"""Compare ordinary-C SpuQuit against retail, modeling external APIs."""
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
with tempfile.TemporaryDirectory(prefix='spu-quit-') as work:
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

def run(called, callback, event, replace_event, candidate):
    cpu = Uc(UC_ARCH_MIPS, UC_MODE_MIPS32 | UC_MODE_LITTLE_ENDIAN)
    cpu.mem_map(0, 0x200000)
    start, size = struct.unpack_from('<II', exe, 0x18)
    cpu.mem_write(start & 0x1FFFFFFF, exe[0x800:0x800 + size])
    if candidate:
        for address, data in sections: cpu.mem_write(address, data)
    def word(address, value):
        cpu.mem_write(address, struct.pack('<I', value & 0xFFFFFFFF))
    def read(address): return int.from_bytes(cpu.mem_read(address, 4), 'little')
    word(0x9B3EC, called)
    word(0x9B434, callback)
    word(0x9B438, callback ^ 0xFFFFFFFF)
    word(0x9B384, event)
    trace = []
    def access(u, kind, address, size, value, user):
        address &= 0x1FFFFFFF
        if address in (0x9B3EC, 0x9B434, 0x9B438, 0x9B384):
            assert size == 4
            trace.append(('write' if kind == UC_MEM_WRITE else 'read', address,
                          value if kind == UC_MEM_WRITE else read(address)))
    cpu.hook_add(UC_HOOK_MEM_READ | UC_HOOK_MEM_WRITE, access)
    names = ('EnterCriticalSection', '_SpuDataCallback', 'CloseEvent', 'DisableEvent', 'ExitCriticalSection')
    api = {int(symbols[name], 16) + 4: name for name in names}
    trapped = []
    def trap(u, address, size, user):
        trapped.append(address); u.emu_stop()
    for address in api:
        cpu.mem_write((address - 4) & 0x1FFFFFFF, bytes(8))
        cpu.hook_add(UC_HOOK_CODE, trap, begin=address, end=address)
    cpu.reg_write(UC_MIPS_REG_SP, 0x801F0000)
    cpu.reg_write(UC_MIPS_REG_RA, 0x801E0000)
    pc = entries['SpuQuit'] if candidate else int(symbols['SpuQuit'], 16)
    for step in range(10):
        cpu.emu_start(pc, 0x801E0000, count=10000)
        if not trapped: break
        name = api[trapped.pop()]
        arg = cpu.reg_read(UC_MIPS_REG_A0)
        if name == 'EnterCriticalSection': assert read(0x9B3EC) == 0
        if name == '_SpuDataCallback':
            assert arg == 0 and read(0x9B434) == read(0x9B438) == 0
        if name == 'CloseEvent': assert arg == event
        if name == 'DisableEvent': assert arg == (event ^ 0x12345678 if replace_event else event)
        trace.append((name, arg) if name in ('_SpuDataCallback', 'CloseEvent', 'DisableEvent') else (name,))
        if name == 'CloseEvent' and replace_event: word(0x9B384, event ^ 0x12345678)
        cpu.reg_write(UC_MIPS_REG_V0, 0xDEADBEEF)
        pc = cpu.reg_read(UC_MIPS_REG_RA)
    assert cpu.reg_read(UC_MIPS_REG_PC) == 0x801E0000
    assert cpu.reg_read(UC_MIPS_REG_SP) == 0x801F0000
    assert [t[0] for t in trace if t[0] in names] == (list(names) if called == 1 else [])
    return trace, (read(0x9B3EC), read(0x9B434), read(0x9B438), read(0x9B384))

for count, case in enumerate(itertools.product((0, 1, 2, 0xFFFFFFFF), (0, 0x80123400), (0, 0xF1000001), (False, True)), 1):
    assert run(*case, False) == run(*case, True), case
print(f'PASS {count} cases: called guard, callback reset ordering, event reload, external-call order and stack')
