#!/usr/bin/env python3
"""Compare ordinary-C CompMatrix CPU-side behavior against retail with scripted GTE results."""
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

def patch_cop2(cpu,pc,size):
    operations={}
    for address in range(pc,pc+size,4):
        word=struct.unpack('<I',cpu.mem_read(address&0x1FFFFFFF,4))[0]
        if word>>26 in (0x12,0x32,0x3A):
            operations[address]=word
            cpu.mem_write(address&0x1FFFFFFF,bytes(4))
    return operations

def stages(data,results,layout):
    left,right,out=layout
    flat=[results[col][row] for row in range(3) for col in range(3)]
    early=bytearray(data)
    for i in (0,1,6,7,8):struct.pack_into('<h',early,out+i*2,flat[i])
    struct.pack_into('<h',early,out+18,results[2][2]>>16)
    full=bytearray(early)
    for i in (2,3,4,5):struct.pack_into('<h',full,out+i*2,flat[i])
    return early,full

def run(data,results,mac,layout,candidate):
    cpu=Uc(UC_ARCH_MIPS,UC_MODE_MIPS32|UC_MODE_LITTLE_ENDIAN)
    cpu.mem_map(0,0x200000)
    start,size=struct.unpack_from('<II',exe,0x18)
    cpu.mem_write(start&0x1FFFFFFF,exe[0x800:0x800+size])
    if candidate:
        for address,payload in sections:cpu.mem_write(address,payload)
    pc=entries['CompMatrix'] if candidate else int(symbols['CompMatrix'],16)
    code_size=next(len(payload) for address,payload in sections if address==0x150000) if candidate else 0x160
    operations=patch_cop2(cpu,pc,code_size)
    gte={0:0,1:0,9:0,10:0,11:0,25:0,26:0,27:0}
    early,full=stages(data,results,layout)
    controls={}
    control_writes=[]
    commands=[]
    reads=[]
    def signed16(x):return (x&65535)-65536 if x&32768 else x&65535
    def hook(cpu,address,size,user):
        if address not in operations:return
        word=operations[address];opcode=word>>26
        rt=(word>>16)&31;rd=(word>>11)&31;rs=(word>>21)&31
        if word==0x4A486012:
            assert len(commands)<4
            commands.append((signed16(gte[0]),signed16(gte[0]>>16),signed16(gte[1])))
            assert controls==expected_controls,(controls,expected_controls)
            if len(commands)<=3:
                assert cpu.mem_read(0x180010,len(data))==data
                for r,value in zip((9,10,11),results[len(commands)-1]):gte[r]=value&0xFFFFFFFF
            else:
                assert cpu.mem_read(0x180010,len(data))==early
                for r,value in zip((25,26,27),mac):gte[r]=value&0xFFFFFFFF
                for r,value in zip((9,10,11),mac):gte[r]=max(-32768,min(32767,value))&0xFFFFFFFF
        elif opcode==0x12 and rs==6:
            assert not commands
            assert rd in range(5)
            value=cpu.reg_read(UC_MIPS_REG_0+rt)
            controls[rd]=value if rd!=4 else signed16(value)&0xFFFFFFFF
            control_writes.append(rd)
        elif opcode==0x12 and rs==4:
            assert rd in (0,1)
            value=cpu.reg_read(UC_MIPS_REG_0+rt)
            gte[rd]=value if rd==0 else signed16(value)&0xFFFFFFFF
        elif opcode==0x12 and rs==0:
            assert rd in (9,10,11,25,26,27)
            reads.append((len(commands),rd))
            cpu.reg_write(UC_MIPS_REG_0+rt,gte[rd])
        elif opcode==0x3A:
            assert rt==11
            offset=word&65535
            if offset&32768:offset-=65536
            destination=(cpu.reg_read(UC_MIPS_REG_0+rs)+offset)&0x1FFFFFFF
            reads.append((len(commands),rt))
            cpu.mem_write(destination,struct.pack('<I',gte[rt]))
        else:raise AssertionError(hex(word))
    cpu.hook_add(UC_HOOK_CODE,hook)
    left,right,out=layout
    original_left=struct.unpack_from('<9h',data,left)
    expected_controls={r:(original_left[2*r]&65535)|((original_left[2*r+1]&65535)<<16) for r in range(4)}
    expected_controls[4]=original_left[8]&0xFFFFFFFF
    cpu.mem_write(0x180000,bytes([0xA5])*16+data+bytes([0x5A])*16)
    cpu.reg_write(UC_MIPS_REG_A0,0x80180010+left)
    cpu.reg_write(UC_MIPS_REG_A1,0x80180010+right)
    cpu.reg_write(UC_MIPS_REG_A2,0x80180010+out)
    cpu.reg_write(UC_MIPS_REG_SP,0x801F0000)
    cpu.reg_write(UC_MIPS_REG_RA,0x801E0000)
    cpu.emu_start(pc,0x801E0000,count=4000)
    assert cpu.reg_read(UC_MIPS_REG_PC)==0x801E0000
    assert cpu.reg_read(UC_MIPS_REG_SP)==0x801F0000
    assert cpu.reg_read(UC_MIPS_REG_V0)==0x80180010+out
    assert cpu.mem_read(0x180000,16)==bytes([0xA5])*16
    assert cpu.mem_read(0x180010+len(data),16)==bytes([0x5A])*16
    source=struct.unpack_from('<9h',data,right)
    expected_commands=[tuple(source[row*3+col] for row in range(3)) for col in range(3)]
    expected_commands.append(tuple(signed16(value) for value in struct.unpack_from('<3i',early,right+20)))
    assert commands==expected_commands,(commands,expected_commands)
    assert control_writes==list(range(5)),control_writes
    assert reads==[(step,r) for step in (1,2,3) for r in (9,10,11)]+[(4,r) for r in (25,26,27)],reads
    translation=struct.unpack_from('<3i',full,left+20)
    sums=tuple(a+b for a,b in zip(mac,translation))
    assert all(-2147483648<=value<=2147483647 for value in sums)
    expected=bytearray(full)
    struct.pack_into('<3i',expected,out+20,*sums)
    output=bytes(cpu.mem_read(0x180010,len(data)))
    assert output==expected,(layout,output,expected)
    return output,commands,gte,controls

import random
rng=random.Random(0x787D4)
values=(-32768,-4096,-1,0,1,4096,32767)
cases=[]
for triple in itertools.product(values,repeat=3):
    results=(triple,triple[1:]+triple[:1],triple[2:]+triple[:2])
    cases.append((bytes(rng.randrange(256) for _ in range(192)),results))
for _ in range(1024):
    cases.append((bytes(rng.randrange(256) for _ in range(192)),tuple(tuple(rng.randrange(-32768,32768) for _ in range(3)) for _ in range(3))))
layouts=((0,64,128),(0,64,0),(0,64,64),(0,0,0),(0,0,128),
         (0,4,8),(8,4,0),(0,32,16),(16,0,32))
count=0
for data,results in cases:
    for layout in layouts:
        _,full=stages(data,results,layout)
        translation=struct.unpack_from('<3i',full,layout[0]+20)
        magnitudes=[rng.choice((0,1,32767,32768,0x7FFFFFFF,0x80000000,rng.randrange(0x80000001))) for _ in range(3)]
        mac=tuple(-magnitude if value>=0 else min(magnitude,0x7FFFFFFF) for value,magnitude in zip(translation,magnitudes))
        assert run(data,results,mac,layout,False)==run(data,results,mac,layout,True)
        count+=1
print(f'PASS {count} cases: four GTE inputs, rotation controls, staged writes, signed-low-halfword translation input, MAC sums without signed overflow, nine overlap layouts, full arena, return and canaries; GTE arithmetic/timing not emulated')
