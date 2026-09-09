#!/usr/bin/env python3
"""Compare ordinary-C RotMatrixZ against retail, modeling external APIs."""
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
with tempfile.TemporaryDirectory(prefix='rotate-z-') as work:
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

def signed(value, bits):
    value &= (1 << bits) - 1
    return value - (1 << bits) if value & (1 << (bits - 1)) else value

def expected(angle, data):
    result = bytearray(data)
    table_index = (abs(signed(angle, 32)) & 4095)
    table_address = 0x800966EC
    packed = struct.unpack_from('<I', exe, table_address - 0x8000F800 + table_index * 4)[0]
    sine = signed(packed, 16)
    if signed(angle, 32) < 0: sine = -sine
    cosine = signed(packed >> 16, 16)
    rows = struct.unpack_from('<6h', data)
    for column in range(3):
        x,y = rows[column],rows[column+3]
        for row,word in ((0,cosine*x-sine*y),(1,sine*x+cosine*y)):
            struct.pack_into('<H', result, (row*3+column)*2, (signed(word,32)>>12)&65535)
    return bytes(result)

def run(angle, data, candidate):
    cpu=Uc(UC_ARCH_MIPS,UC_MODE_MIPS32|UC_MODE_LITTLE_ENDIAN)
    cpu.mem_map(0,0x200000)
    start,size=struct.unpack_from('<II',exe,0x18)
    cpu.mem_write(start&0x1FFFFFFF,exe[0x800:0x800+size])
    if candidate:
        for address,payload in sections:cpu.mem_write(address,payload)
    cpu.mem_write(0x180000,bytes([0xA5])*16+data+bytes([0x5A])*16)
    cpu.reg_write(UC_MIPS_REG_A0,angle&0xFFFFFFFF)
    cpu.reg_write(UC_MIPS_REG_A1,0x80180010)
    cpu.reg_write(UC_MIPS_REG_SP,0x801F0000)
    cpu.reg_write(UC_MIPS_REG_RA,0x801E0000)
    pc=entries['RotMatrixZ'] if candidate else int(symbols['RotMatrixZ'],16)
    cpu.emu_start(pc,0x801E0000,count=2000)
    assert cpu.reg_read(UC_MIPS_REG_PC)==0x801E0000
    assert cpu.reg_read(UC_MIPS_REG_SP)==0x801F0000
    assert cpu.reg_read(UC_MIPS_REG_V0)==0x80180010
    assert cpu.mem_read(0x180000,16)==bytes([0xA5])*16
    assert cpu.mem_read(0x180030,16)==bytes([0x5A])*16
    result=bytes(cpu.mem_read(0x180010,32))
    assert result==expected(angle,data),(angle,data,result,expected(angle,data))
    return result

import random
rng=random.Random(0x79E14)
angles=list(range(-4095,4096))+[-0x80000000,0x7FFFFFFF,-4096,4096,-8192,8192]
angles += [rng.randrange(-0x80000000,0x80000000) for _ in range(1024)]
cases=[(angle,bytes(rng.randrange(256) for _ in range(32))) for angle in angles]
for angle in (0,1,-1,1024,-1024,2048,3072,4095,-0x80000000):
    for value in (-32768,32767,0,4096):
        cases.append((angle,struct.pack('<9h',*[value]*9)+bytes([0xA5])*14))
for count,(angle,data) in enumerate(cases,1):
    assert run(angle,data,False)==run(angle,data,True),(angle,data)
print(f'PASS {count} cases: complete positive/negative angle cycle, INT_MIN/MAX, random/extreme matrices, independent fixed-point model, untouched row/translation/padding, return and canaries')
