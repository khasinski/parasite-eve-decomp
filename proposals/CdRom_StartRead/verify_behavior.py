#!/usr/bin/env python3
"""Compare ordinary-C CdRom_StartRead against retail, modeling external APIs."""
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
with tempfile.TemporaryDirectory(prefix='cd-start-') as work:
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

def run(progress,busy,has_position,mode,command_result,flags,change_flags,candidate):
    cpu=Uc(UC_ARCH_MIPS,UC_MODE_MIPS32|UC_MODE_LITTLE_ENDIAN)
    cpu.mem_map(0,0x200000)
    start,size=struct.unpack_from('<II',exe,0x18)
    cpu.mem_write(start&0x1FFFFFFF,exe[0x800:0x800+size])
    if candidate:
        for address,data in sections: cpu.mem_write(address,data)
    def word(a,v):cpu.mem_write(a&0x1FFFFFFF,struct.pack('<I',v&0xFFFFFFFF))
    def read(a):return int.from_bytes(cpu.mem_read(a&0x1FFFFFFF,4),'little')
    def snapshot():return bytes(cpu.mem_read(0x9B6A4,44))
    cpu.mem_write(0x9B6A4,bytes([0xA5])*44)
    word(0x9B6B8,flags);word(0x9B6CC,progress)
    initial=snapshot()
    position=0x80180001 if has_position else 0
    cpu.mem_write(0x180001,b'\x01\x23\x45\x67')
    cpu.mem_write(0x180101,b'\x02\x34\x56\x78')
    location=b'\x01\x23\x45\x67' if has_position else b'\x02\x34\x56\x78'
    sectors=(-1,0,3)[mode%3]
    names=('DsRead_IsBusy','CdRom_GetCurrentPos','Render_BuildParticleFrame','VSync','DsDataCallback')
    apis={int(symbols[n],16)+4:n for n in names}
    trace=[];trapped=[]
    def trap(u,a,z,d):trapped.append(a);u.emu_stop()
    for a in apis:
        cpu.mem_write((a-4)&0x1FFFFFFF,bytes(8));cpu.hook_add(UC_HOOK_CODE,trap,begin=a,end=a)
    for reg,value in ((UC_MIPS_REG_A0,position),(UC_MIPS_REG_A1,sectors&0xFFFFFFFF),(UC_MIPS_REG_A2,0x80190000),(UC_MIPS_REG_A3,mode),(UC_MIPS_REG_SP,0x801F0000),(UC_MIPS_REG_RA,0x801E0000)):
        cpu.reg_write(reg,value)
    pc=entries['CdRom_StartRead'] if candidate else int(symbols['CdRom_StartRead'],16)
    for _ in range(10):
        cpu.emu_start(pc,0x801E0000,count=2000)
        if not trapped:break
        name=apis[trapped.pop()]
        args=tuple(cpu.reg_read(r) for r in (UC_MIPS_REG_A0,UC_MIPS_REG_A1,UC_MIPS_REG_A2,UC_MIPS_REG_A3))
        if name=='DsRead_IsBusy':trace.append((name,snapshot()));value=busy
        elif name=='CdRom_GetCurrentPos':
            assert args[0]==0
            trace.append((name,0,snapshot()));value=0x80180101
        elif name=='Render_BuildParticleFrame':
            data=bytes(cpu.mem_read(args[1]&0x1FFFFFFF,4))
            assert data==location and args[0]==((mode|0x20)&255) and args[2]==6
            assert args[3]==int(symbols['CdRom_SetMode2Callback'],16)
            assert read(cpu.reg_read(UC_MIPS_REG_SP)+16)==0xFFFFFFFF
            trace.append((name,args[0],data,args[2],args[3],snapshot()));value=command_result
        elif name=='VSync':
            assert args[0]==0xFFFFFFFF
            trace.append((name,snapshot()));value=12345
            if change_flags:word(0x9B6B8,flags^1)
        else:
            assert args[0]==int(symbols['Render_StepParticleCallback'],16)
            trace.append((name,args[0],snapshot()));value=0x801C0000
        cpu.reg_write(UC_MIPS_REG_V0,value)
        pc=cpu.reg_read(UC_MIPS_REG_RA)
    assert cpu.reg_read(UC_MIPS_REG_PC)==0x801E0000 and cpu.reg_read(UC_MIPS_REG_SP)==0x801F0000
    blocked=progress==1 or busy!=0
    expected=bytearray(initial)
    expected_names=[] if progress==1 else ['DsRead_IsBusy']
    if not blocked:
        for offset,value in ((8,512),(12,0x80190000),(16,sectors)):
            struct.pack_into('<I',expected,offset,value&0xFFFFFFFF)
        if not has_position:expected_names.append('CdRom_GetCurrentPos')
        expected_names.append('Render_BuildParticleFrame')
        if command_result:
            expected_names.append('VSync')
            final_flags=flags^int(change_flags)
            struct.pack_into('<I',expected,20,final_flags)
            struct.pack_into('<I',expected,32,12345)
            struct.pack_into('<I',expected,40,1)
            if final_flags&1:
                expected_names.append('DsDataCallback');struct.pack_into('<I',expected,28,0x801C0000)
    assert [t[0] for t in trace]==expected_names
    assert snapshot()==bytes(expected)
    assert cpu.reg_read(UC_MIPS_REG_V0)==(0 if blocked else command_result)
    return trace,snapshot()

for count,case in enumerate(itertools.product((0,1,2),(0,1),(False,True),(0,1,0x101),(0,1,0xFFFFFFFF),(0,1),(False,True)),1):
    assert run(*case,False)==run(*case,True),case
print(f'PASS {count} cases: busy short circuit, unaligned position copy/fallback, command arguments, failure state, DMA callback save, flag reload and return')
