#!/usr/bin/env python3
"""Compare CD_initvol C with retail MIPS under finite modeled CD FIFO traces."""
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


def run(left, right, redirect, fill, candidate):
    cpu = Uc(UC_ARCH_MIPS, UC_MODE_MIPS32 | UC_MODE_LITTLE_ENDIAN)
    cpu.mem_map(0, 0x200000)
    start, size = struct.unpack_from('<II', exe, 0x18)
    cpu.mem_write(start & 0x1FFFFFFF, exe[0x800:0x800 + size])
    if candidate:
        for address, data in sections:
            cpu.mem_write(address, data)
    def word(address, value):
        cpu.mem_write(address, struct.pack('<I', value))
    word(0x9B290, 0x80131000)
    for offset, address in enumerate((0x9B27C, 0x9B280, 0x9B284, 0x9B288)):
        word(address, 0x80130000 + offset)
    for address in (0x131000, 0x132000):
        cpu.mem_write(address, bytes([fill]) * 0x200)
    cpu.mem_write(0x1311B8, struct.pack('<HH', left, right))
    trace = []
    def access(u, kind, address, size, value, user):
        address &= 0x1FFFFFFF
        if 0x130000 <= address < 0x130004:
            assert kind == UC_MEM_WRITE and size == 1
        elif 0x131000 <= address < 0x131200 or 0x132000 <= address < 0x132200:
            assert size == 2
        else:
            return
        if kind == UC_MEM_WRITE:
            trace.append(('write', address, value))
            if redirect and address == 0x131182:
                word(0x9B290, 0x80132000)
        else:
            trace.append(('read', address, struct.unpack('<H', u.mem_read(address, 2))[0]))
    cpu.hook_add(UC_HOOK_MEM_READ | UC_HOOK_MEM_WRITE, access)
    cpu.reg_write(UC_MIPS_REG_SP, 0x801F0000)
    cpu.reg_write(UC_MIPS_REG_RA, 0x801E0000)
    cpu.emu_start(entries['CD_initvol'] if candidate else 0x8007BAC0, 0x801E0000, count=10000)
    assert cpu.reg_read(UC_MIPS_REG_PC) == 0x801E0000
    assert cpu.reg_read(UC_MIPS_REG_SP) == 0x801F0000
    assert cpu.reg_read(UC_MIPS_REG_V0) == 0
    expected = [('read', 0x1311B8, left)]
    if not left:
        expected.append(('read', 0x1311BA, right))
    if not left and not right:
        expected += [('write', 0x131180, 0x3FFF), ('write', 0x131182, 0x3FFF)]
    base = 0x132000 if redirect and not left and not right else 0x131000
    expected += [('write', base + 0x1B0, 0x3FFF), ('write', base + 0x1B2, 0x3FFF), ('write', base + 0x1AA, 0xC001)]
    expected += [('write', 0x130000 + port, value) for port, value in ((0, 2), (2, 0x80), (3, 0), (0, 3), (1, 0x80), (2, 0), (3, 0x20))]
    assert trace == expected
    return trace, bytes(cpu.mem_read(0x131000, 0x200)), bytes(cpu.mem_read(0x132000, 0x200))

cases = itertools.product((0, 1, 0x7FFF, 0x8000, 0xFFFF), repeat=2)
count = 0
for left, right in cases:
    for redirect, fill in itertools.product((False, True), (0, 0xA5)):
        assert run(left, right, redirect, fill, False) == run(left, right, redirect, fill, True)
        count += 1
print(f'PASS {count} cases: current-volume gating, SPU pointer reload, every register access, output and stack restoration')
