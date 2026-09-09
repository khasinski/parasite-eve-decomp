#!/usr/bin/env python3
"""Compare ordinary-C RotAverageNclip3 CPU-side behavior against retail with scripted GTE results."""
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
from unicorn.mips_const import UC_MIPS_REG_0, UC_MIPS_REG_A0, UC_MIPS_REG_A1, UC_MIPS_REG_A2, UC_MIPS_REG_A3, UC_MIPS_REG_V0, UC_MIPS_REG_SP, UC_MIPS_REG_RA, UC_MIPS_REG_PC


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

def run(data,layout,clip,flags,screen,depth,average,candidate):
    cpu=Uc(UC_ARCH_MIPS,UC_MODE_MIPS32|UC_MODE_LITTLE_ENDIAN)
    cpu.mem_map(0,0x200000)
    start,size=struct.unpack_from('<II',exe,0x18)
    cpu.mem_write(start&0x1FFFFFFF,exe[0x800:0x800+size])
    if candidate:
        for address,payload in sections:cpu.mem_write(address,payload)
    pc=entries['RotAverageNclip3'] if candidate else int(symbols['RotAverageNclip3'],16)
    code_size=next(len(payload) for address,payload in sections if address==0x150000) if candidate else 0x88
    operations={}
    for address in range(pc,pc+code_size,4):
        word=struct.unpack('<I',cpu.mem_read(address&0x1FFFFFFF,4))[0]
        if word>>26 in (0x12,0x32,0x3A):
            operations[address]=word
            cpu.mem_write(address&0x1FFFFFFF,bytes(4))
    vertices,outputs=layout
    expected=bytearray(data)
    struct.pack_into('<I',expected,outputs[5],flags)
    after_flags=bytes(expected)
    if clip>0:
        for offset,value in zip(outputs[:5],(*screen,depth,average)):
            struct.pack_into('<I',expected,offset,value)
    regs={i:0 for i in range(32)}
    control_flags=0
    commands=[]
    stores=[]
    reads=[]
    sequence=0
    pending=[]
    def signed16(x):return (x&65535)-65536 if x&32768 else x&65535
    def hook(cpu,address,size,user):
        nonlocal sequence,control_flags
        sequence+=1
        for due,reg,value in list(pending):
            if due<=sequence:
                cpu.reg_write(UC_MIPS_REG_0+reg,value)
                pending.remove((due,reg,value))
        if address not in operations:return
        word=operations[address];opcode=word>>26
        rt=(word>>16)&31;rd=(word>>11)&31;rs=(word>>21)&31
        if word==0x4A280030:
            assert not commands
            wanted=[]
            for offset in vertices:
                xy,z=struct.unpack_from('<II',data,offset)
                wanted.extend((xy,signed16(z)&0xFFFFFFFF))
            assert [regs[i] for i in range(6)]==wanted
            assert cpu.mem_read(0x180010,len(data))==data
            commands.append('rtpt')
            control_flags=flags
            for r,v in zip((12,13,14,8),(*screen,depth)):regs[r]=v
        elif word==0x4B400006:
            assert commands==['rtpt']
            assert cpu.mem_read(0x180010,len(data))==after_flags
            commands.append('nclip')
            regs[24]=clip&0xFFFFFFFF
            control_flags=flags^0x80000000
        elif word==0x4B58002D:
            assert commands==['rtpt','nclip'] and clip>0
            commands.append('avsz3')
            regs[7]=average
            regs[24]=(~clip)&0xFFFFFFFF
            control_flags=flags^0x40000000
        elif opcode==0x12 and rs in (0,2):
            assert (rs,rd) in ((2,31),(0,24),(0,7))
            value=control_flags if rs==2 else regs[rd]
            reads.append((rs,rd))
            pending.append((sequence+2,rt,value))
        elif opcode in (0x32,0x3A):
            offset=word&65535
            if offset&32768:offset-=65536
            pointer=(cpu.reg_read(UC_MIPS_REG_0+rs)+offset)&0x1FFFFFFF
            if opcode==0x32:
                assert rt in range(6) and not commands
                value=struct.unpack('<I',cpu.mem_read(pointer,4))[0]
                regs[rt]=value if rt%2==0 else signed16(value)&0xFFFFFFFF
            else:
                assert rt in (12,13,14,8) and commands==['rtpt','nclip'] and clip>0
                stores.append((rt,pointer-0x180010))
                cpu.mem_write(pointer,struct.pack('<I',regs[rt]))
        else:raise AssertionError(hex(word))
    cpu.hook_add(UC_HOOK_CODE,hook)
    cpu.mem_write(0x180000,bytes([0xA5])*16+data+bytes([0x5A])*16)
    args=[0x80180010+offset for offset in (*vertices,*outputs)]
    for reg,value in zip((UC_MIPS_REG_A0,UC_MIPS_REG_A1,UC_MIPS_REG_A2,UC_MIPS_REG_A3),args):cpu.reg_write(reg,value)
    cpu.mem_write(0x1F0010,struct.pack('<5I',*args[4:]))
    cpu.reg_write(UC_MIPS_REG_SP,0x801F0000)
    cpu.reg_write(UC_MIPS_REG_RA,0x801E0000)
    cpu.emu_start(pc,0x801E0000,count=4000)
    assert cpu.reg_read(UC_MIPS_REG_PC)==0x801E0000
    assert cpu.reg_read(UC_MIPS_REG_SP)==0x801F0000
    assert cpu.reg_read(UC_MIPS_REG_V0)==clip&0xFFFFFFFF
    assert not pending
    assert cpu.mem_read(0x180000,16)==bytes([0xA5])*16
    assert cpu.mem_read(0x180010+len(data),16)==bytes([0x5A])*16
    assert cpu.mem_read(0x1F0010,20)==struct.pack('<5I',*args[4:])
    assert cpu.mem_read(0x180010,len(data))==expected,(clip,layout)
    assert commands==['rtpt','nclip']+(['avsz3'] if clip>0 else [])
    assert reads==[(2,31),(0,24)]+([(0,7)] if clip>0 else [])
    assert stores==(list(zip((12,13,14,8),outputs[:4])) if clip>0 else [])
    return bytes(expected),commands,regs,control_flags

import random
rng=random.Random(0x79384)
layouts=(((0,8,16),(48,52,56,60,64,68)),
         ((0,8,16),(48,48,48,48,48,48)),
         ((0,8,16),(0,4,8,12,16,20)),
         ((0,8,16),(24,20,16,12,8,4)),
         ((0,0,0),(0,4,8,12,16,20)))
count=0
for clip,flags,depth,average,layout in itertools.product(
        (-2147483648,-1,0,1,2147483647),(0,0xFFFFFFFF,0x81234567),
        (0,1,4096),(0,1,65535),layouts):
    data=bytes(rng.randrange(256) for _ in range(96))
    screen=tuple(rng.randrange(0x100000000) for _ in range(3))
    assert run(data,layout,clip,flags,screen,depth,average,False)==run(data,layout,clip,flags,screen,depth,average,True)
    count+=1
print(f'PASS {count} cases: projection flags before clipping, positive/zero/negative area, conditional output writes, AVSZ3 order, CPU read delays, shared and input-aliased outputs, return and canaries; GTE arithmetic/timing beyond transfer delays not emulated')
