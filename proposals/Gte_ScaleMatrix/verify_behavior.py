#!/usr/bin/env python3
"""Compare ordinary-C Gte_ScaleMatrix against retail and an independent integer model."""
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
with tempfile.TemporaryDirectory(prefix='rotation-matrix-') as work:
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

def expected(scales,data):
    values=struct.unpack_from('<9h',data)
    results=[]
    for i,value in enumerate(values):
        product=(value*scales[i%3])&0xFFFFFFFF
        if product>=0x80000000:product-=0x100000000
        results.append(product//4096)
    return struct.pack('<8H',*(v&65535 for v in results[:8]))+struct.pack('<i',results[8])+data[20:]

def run(scales,data,alias,candidate):
    cpu=Uc(UC_ARCH_MIPS,UC_MODE_MIPS32|UC_MODE_LITTLE_ENDIAN)
    cpu.mem_map(0,0x200000)
    start,size=struct.unpack_from('<II',exe,0x18)
    cpu.mem_write(start&0x1FFFFFFF,exe[0x800:0x800+size])
    if candidate:
        for address,payload in sections:cpu.mem_write(address,payload)
    packed=struct.pack('<3i',*scales)
    if alias is not None:data=data[:alias]+packed+data[alias+12:]
    cpu.mem_write(0x180000,bytes([0xA5])*16+data+bytes([0x5A])*16)
    cpu.mem_write(0x181000,packed)
    cpu.reg_write(UC_MIPS_REG_A0,0x80180010)
    cpu.reg_write(UC_MIPS_REG_A1,0x80181000 if alias is None else 0x80180010+alias)
    cpu.reg_write(UC_MIPS_REG_SP,0x801F0000)
    cpu.reg_write(UC_MIPS_REG_RA,0x801E0000)
    pc=entries['Gte_ScaleMatrix'] if candidate else int(symbols['Gte_ScaleMatrix'],16)
    cpu.emu_start(pc,0x801E0000,count=4000)
    assert cpu.reg_read(UC_MIPS_REG_PC)==0x801E0000
    assert cpu.reg_read(UC_MIPS_REG_SP)==0x801F0000
    assert cpu.reg_read(UC_MIPS_REG_V0)==0x80180010
    assert cpu.mem_read(0x180000,16)==bytes([0xA5])*16
    assert cpu.mem_read(0x180030,16)==bytes([0x5A])*16
    assert cpu.mem_read(0x181000,12)==packed
    output=bytes(cpu.mem_read(0x180010,32))
    assert output==expected(scales,data),(scales,alias,output,expected(scales,data))
    return output

import random
rng=random.Random(0x78CC4)
values=(-2147483648,-65536,-4097,-4096,-1,0,1,4095,4096,4097,65536,2147483647)
cases=[]
for scale in itertools.product(values,repeat=3):
    cases.append((scale,bytes(rng.randrange(256) for _ in range(32))))
for _ in range(1024):
    cases.append((tuple(rng.randrange(-2147483648,2147483648) for _ in range(3)),bytes(rng.randrange(256) for _ in range(32))))
for coefficient in (-32768,-4096,-1,0,1,4096,32767):
    for scale in values:
        cases.append(((scale,scale,scale),struct.pack('<9h',*([coefficient]*9))+bytes([0xA5])*14))
count=0
for scales,data in cases:
    for alias in (None,0,4,8,12,16,20):
        assert run(scales,data,alias,False)==run(scales,data,alias,True)
        count+=1
print(f'PASS {count} cases: signed scale extrema, low-word overflow, Q12 rounding, matrix/scale overlap, padding overwrite, preserved translation, return and canaries')
