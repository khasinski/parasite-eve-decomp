#!/usr/bin/env python3
"""Compare ordinary-C RotMatrixZYX against retail and an independent integer model."""
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

def trig(angle):
    packed=struct.unpack_from('<I',exe,0x800966EC-0x8000F800+(abs(angle)&4095)*4)[0]
    sine=packed&65535
    if sine>=32768:sine-=65536
    cosine=packed>>16
    if cosine>=32768:cosine-=65536
    return (-sine if angle<0 else sine),cosine

def expected(angles,data):
    sx,cx=trig(angles[0]);sy,cy=trig(angles[1]);sz,cz=trig(angles[2])
    def q(a,b):return (a*b)//4096
    a=q(sx,sy);b=q(sy,cx)
    rows=(q(cy,cz),q(a,cz)-q(sz,cx),q(b,cz)+q(sx,sz),
          q(sz,cy),q(a,sz)+q(cx,cz),q(b,sz)-q(sx,cz),
          -sy,q(sx,cy),q(cx,cy))
    return struct.pack('<9H',*(x&65535 for x in rows))+data[18:]

def run(angles,data,alias,candidate):
    cpu=Uc(UC_ARCH_MIPS,UC_MODE_MIPS32|UC_MODE_LITTLE_ENDIAN)
    cpu.mem_map(0,0x200000)
    start,size=struct.unpack_from('<II',exe,0x18)
    cpu.mem_write(start&0x1FFFFFFF,exe[0x800:0x800+size])
    if candidate:
        for address,payload in sections:cpu.mem_write(address,payload)
    packed=struct.pack('<3h',*angles)
    if alias:data=packed+data[6:]
    cpu.mem_write(0x180000,bytes([0xA5])*16+data+bytes([0x5A])*16)
    cpu.mem_write(0x181000,packed)
    cpu.reg_write(UC_MIPS_REG_A0,0x80180010 if alias else 0x80181000)
    cpu.reg_write(UC_MIPS_REG_A1,0x80180010)
    cpu.reg_write(UC_MIPS_REG_SP,0x801F0000)
    cpu.reg_write(UC_MIPS_REG_RA,0x801E0000)
    pc=entries['RotMatrixZYX'] if candidate else int(symbols['RotMatrixZYX'],16)
    cpu.emu_start(pc,0x801E0000,count=4000)
    assert cpu.reg_read(UC_MIPS_REG_PC)==0x801E0000
    assert cpu.reg_read(UC_MIPS_REG_SP)==0x801F0000
    assert cpu.reg_read(UC_MIPS_REG_V0)==0x80180010
    assert cpu.mem_read(0x180000,16)==bytes([0xA5])*16
    assert cpu.mem_read(0x180030,16)==bytes([0x5A])*16
    assert cpu.mem_read(0x181000,6)==packed
    output=bytes(cpu.mem_read(0x180010,32))
    assert output==expected(angles,data),(angles,alias,output,expected(angles,data))
    return output

import random
rng=random.Random(0x799E4)
values=(-32768,-8192,-4096,-2048,-1024,-1,0,1,1024,2048,3072,4095,4096,8192,32767)
angles=list(itertools.product(values,repeat=3))
angles += [tuple(rng.randrange(-32768,32768) for _ in range(3)) for _ in range(1024)]
count=0
for triple in angles:
    data=bytes(rng.randrange(256) for _ in range(32))
    for alias in (False,True):
        assert run(triple,data,alias,False)==run(triple,data,alias,True),(triple,alias)
        count+=1
print(f'PASS {count} cases: angle extrema/wraps, mixed-axis combinations, staged Q12 rounding, aliased inputs, preserved translation/padding, pointer return and canaries')
