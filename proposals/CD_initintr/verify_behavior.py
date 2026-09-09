#!/usr/bin/env python3
"""Compare CD_initintr C with retail MIPS under finite modeled CD FIFO traces."""
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
with tempfile.TemporaryDirectory(prefix='getintr-') as work:
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


def run(initial, reset_return, callback_return, reset_changes_status, candidate):
    cpu = Uc(UC_ARCH_MIPS, UC_MODE_MIPS32 | UC_MODE_LITTLE_ENDIAN)
    cpu.mem_map(0, 0x200000)
    start, size = struct.unpack_from('<II', exe, 0x18)
    cpu.mem_write(start & 0x1FFFFFFF, exe[0x800:0x800 + size])
    if candidate:
        for address, data in sections:
            cpu.mem_write(address, data)
    words = (0x9AFB8, 0x9AFB4, 0x9AFC8, 0x9AFC4)
    def word(address, value):
        cpu.mem_write(address, struct.pack('<I', value & 0xFFFFFFFF))
    for address in words:
        word(address, initial)
    api = {int(symbols[name], 16): name for name in ('ResetCallback', 'InterruptCallback')}
    trace = []
    trapped = []
    def trap(u, address, size, user):
        trapped.append(address)
        u.emu_stop()
    for address in api:
        cpu.hook_add(UC_HOOK_CODE, trap, begin=address, end=address)
    cpu.reg_write(UC_MIPS_REG_SP, 0x801F0000)
    cpu.reg_write(UC_MIPS_REG_RA, 0x801E0000)
    pc = entries['CD_initintr'] if candidate else 0x8007BBB0
    for step in range(4):
        cpu.emu_start(pc, 0x801E0000, count=1000)
        if not trapped:
            break
        name = api[trapped.pop()]
        if name == 'ResetCallback':
            assert all(cpu.mem_read(address, 4) == b'\0' * 4 for address in words)
            trace.append((name,))
            value = reset_return
            if reset_changes_status:
                word(0x9AFC4, 0x12345678)
        else:
            args = tuple(cpu.reg_read(reg) for reg in (UC_MIPS_REG_A0, UC_MIPS_REG_A1))
            assert args == (2, int(symbols['Cd_SetIntrMask'], 16))
            trace.append((name, args))
            value = callback_return
        cpu.reg_write(UC_MIPS_REG_V0, value & 0xFFFFFFFF)
        pc = cpu.reg_read(UC_MIPS_REG_RA)
    assert cpu.reg_read(UC_MIPS_REG_PC) == 0x801E0000
    assert cpu.reg_read(UC_MIPS_REG_SP) == 0x801F0000
    assert [item[0] for item in trace] == ['ResetCallback', 'InterruptCallback']
    assert cpu.mem_read(0x9AFC4, 4) == struct.pack('<I', 0x12345678 if reset_changes_status else 0)
    return trace, tuple(bytes(cpu.mem_read(address, 4)) for address in words)

cases = itertools.product((0, 0xFFFFFFFF, 0x12345678), (0, -1), (0, 0x80123400), (False, True))
for count, case in enumerate(cases, 1):
    assert run(*case, False) == run(*case, True), case
print(f'PASS {count} cases: full-word resets, callback installation, call order and stack restoration')
