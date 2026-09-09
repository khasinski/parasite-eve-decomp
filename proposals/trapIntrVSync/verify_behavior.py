#!/usr/bin/env python3
"""Compare the VSync dispatcher with retail and a callback-mutation model."""
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

TABLE=0x9568C
CALLBACKS=[0x80160000+i*16 for i in range(9)]

def mutate(slots,count,callback,mode):
    if mode and callback<8:
        if callback<7:slots[callback+1]=0 if mode==1 else CALLBACKS[8]
        slots[7]=CALLBACKS[8]
        if mode==2:
            slots[0]=CALLBACKS[8]
            count=(count-1)&0xFFFFFFFF
    return count

def model(mask,count,mode):
    slots=[CALLBACKS[i] if mask&(1<<i) else 0 for i in range(8)]
    count=(count+1)&0xFFFFFFFF
    calls=[]
    for i in range(8):
        callback=slots[i]
        if callback:
            index=CALLBACKS.index(callback)
            calls.append((index,struct.pack('<9I',*slots,count)))
            count=mutate(slots,count,index,mode)
    return struct.pack('<9I',*slots,count),calls

def run(mask,count,mode,candidate):
    cpu=Uc(UC_ARCH_MIPS,UC_MODE_MIPS32|UC_MODE_LITTLE_ENDIAN)
    cpu.mem_map(0,0x200000)
    start,size=struct.unpack_from('<II',exe,0x18)
    cpu.mem_write(start&0x1FFFFFFF,exe[0x800:0x800+size])
    if candidate:
        for address,payload in sections:cpu.mem_write(address,payload)
    slots=[CALLBACKS[i] if mask&(1<<i) else 0 for i in range(8)]
    cpu.mem_write(TABLE-8,bytes([0xA5])*8+struct.pack('<9I',*slots,count)+bytes([0x5A])*8)
    for callback in CALLBACKS:cpu.mem_write(callback&0x1FFFFFFF,bytes(8))
    pending=[]
    calls=[]
    def hook(cpu,address,size,user):
        if address-4 in CALLBACKS:
            index=CALLBACKS.index(address-4)
            snapshot=bytes(cpu.mem_read(TABLE,36))
            calls.append((index,snapshot))
            pending.append(index)
            cpu.emu_stop()
    cpu.hook_add(UC_HOOK_CODE,hook)
    cpu.reg_write(UC_MIPS_REG_SP,0x801F0000)
    cpu.reg_write(UC_MIPS_REG_RA,0x801E0000)
    for reg in range(16,24):cpu.reg_write(UC_MIPS_REG_0+reg,0x12340000+reg)
    pc=entries['trapIntrVSync'] if candidate else int(symbols['trapIntrVSync'],16)
    while True:
        cpu.emu_start(pc,0x801E0000,count=4000)
        if not pending:break
        index=pending.pop()
        state=list(struct.unpack('<9I',cpu.mem_read(TABLE,36)))
        slots=state[:8]
        count=mutate(slots,state[8],index,mode)
        cpu.mem_write(TABLE,struct.pack('<9I',*slots,count))
        cpu.reg_write(UC_MIPS_REG_V0,(0x80000000+index)&0xFFFFFFFF)
        pc=cpu.reg_read(UC_MIPS_REG_RA)
    assert cpu.reg_read(UC_MIPS_REG_PC)==0x801E0000
    assert cpu.reg_read(UC_MIPS_REG_SP)==0x801F0000
    for reg in range(16,24):assert cpu.reg_read(UC_MIPS_REG_0+reg)==0x12340000+reg
    assert cpu.mem_read(TABLE-8,8)==bytes([0xA5])*8
    assert cpu.mem_read(TABLE+36,8)==bytes([0x5A])*8
    return bytes(cpu.mem_read(TABLE,36)),calls

count=0
for mask,initial,mode in itertools.product(range(256),(0,0x7FFFFFFF,0x80000000,0xFFFFFFFF),range(3)):
    expected=model(mask,initial,mode)
    assert run(mask,initial,mode,False)==expected,(mask,initial,mode,'retail')
    assert run(mask,initial,mode,True)==expected,(mask,initial,mode,'candidate')
    count+=1
print(f'PASS {count} cases: all callback masks, counter wraps, ordered callback snapshots, future-slot removal/replacement, callback counter changes, callee-saved registers, SP and canaries')
