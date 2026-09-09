#!/usr/bin/env python3
"""Compare ordinary-C CdRom_ReadProgressCallback against retail, modeling external APIs."""
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
with tempfile.TemporaryDirectory(prefix='cd-progress-') as work:
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

def run(flags, remaining, status, clock_case, callback_present, mutate_cleanup, candidate):
    cpu = Uc(UC_ARCH_MIPS, UC_MODE_MIPS32 | UC_MODE_LITTLE_ENDIAN)
    cpu.mem_map(0, 0x200000)
    start, size = struct.unpack_from('<II', exe, 0x18)
    cpu.mem_write(start & 0x1FFFFFFF, exe[0x800:0x800 + size])
    if candidate:
        for address, data in sections: cpu.mem_write(address, data)
    def word(a,v): cpu.mem_write(a & 0x1FFFFFFF, struct.pack('<I',v & 0xFFFFFFFF))
    def snapshot(): return bytes(cpu.mem_read(0x9B6A4, 48))
    values = (0xA5A5A5A5,0x5A5A5A5A,512,0x80180000,remaining,flags,0x12345678,0x87654321,100,99,1)
    for i,v in enumerate(values): word(0x9B6A4+i*4,v)
    callback,replacement = 0x801C0000,0x801C0020
    word(0x9B6D0,callback if callback_present else 0)
    names = ('VSync','CdRom_IsBusy','CdRom_IsBusy2','Save_ProcessDataCallback')
    apis = {int(symbols[n],16)+4:n for n in names}
    apis.update({callback+4:'complete',replacement+4:'replacement'})
    trapped=[];trace=[]
    def trap(u,a,z,d): trapped.append(a);u.emu_stop()
    for a in apis:
        cpu.mem_write((a-4)&0x1FFFFFFF,bytes(8))
        cpu.hook_add(UC_HOOK_CODE,trap,begin=a,end=a)
    cpu.reg_write(UC_MIPS_REG_A0,status)
    cpu.reg_write(UC_MIPS_REG_A1,0x80190000)
    cpu.reg_write(UC_MIPS_REG_A2,0xABCDEF00)
    cpu.reg_write(UC_MIPS_REG_SP,0x801F0000)
    cpu.reg_write(UC_MIPS_REG_RA,0x801E0000)
    pc=entries['CdRom_ReadProgressCallback'] if candidate else int(symbols['CdRom_ReadProgressCallback'],16)
    clocks=((500,1299,1299),(500,1300,1300),(500,1301,1301),(500,1300,1301))[clock_case]
    ticks=0
    for _ in range(12):
        cpu.emu_start(pc,0x801E0000,count=2000)
        if not trapped: break
        name=apis[trapped.pop()]
        a0,a1=cpu.reg_read(UC_MIPS_REG_A0),cpu.reg_read(UC_MIPS_REG_A1)
        if name=='VSync':
            assert a0==0xFFFFFFFF and ticks<3
            value=clocks[ticks];ticks+=1
            trace.append((name,value,snapshot()))
        elif name in ('CdRom_IsBusy','CdRom_IsBusy2'):
            assert a0==0x80180000 and a1==512
            trace.append((name,a0,a1,snapshot()));value=0xDEADBEEF
        elif name=='Save_ProcessDataCallback':
            trace.append((name,snapshot()));value=0xDEADBEEF
            if mutate_cleanup:
                word(0x9B6B4, -1 if remaining>=0 else 0)
                word(0x9B6D0,replacement)
        else:
            assert a1==0x80190000 and a0 in (1,2,5)
            assert name==('replacement' if mutate_cleanup else 'complete')
            trace.append((name,a0,a1,snapshot()));value=0xDEADBEEF
        cpu.reg_write(UC_MIPS_REG_V0,value)
        pc=cpu.reg_read(UC_MIPS_REG_RA)
    assert cpu.reg_read(UC_MIPS_REG_PC)==0x801E0000
    assert cpu.reg_read(UC_MIPS_REG_SP)==0x801F0000
    assert int.from_bytes(snapshot()[36:40],'little')==500
    assert ticks==(1 if flags&1 else (2 if remaining in (0,1) and clocks[1]<=1300 else 3))
    return trace,snapshot()

for count,case in enumerate(itertools.product((0,1),(-1,0,1,2),(1,0x102,5),range(4),(False,True),(False,True)),1):
    assert run(*case,False)==run(*case,True),case
print(f'PASS {count} cases: transfer branches, deadline boundaries/crossing, state at API calls, event truncation, cleanup mutation and callback reload')
