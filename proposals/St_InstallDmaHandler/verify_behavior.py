#!/usr/bin/env python3
"""Compare guarded exception-handler patch installation with retail."""
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

ORIGINAL=exe[0x6A9E0:0x6A9F8]
REPLACEMENT=exe[0x6A9F8:0x6AA10]
assert len(ORIGINAL)==len(REPLACEMENT)==24 and ORIGINAL!=REPLACEMENT

def run(mismatch,kernel,table,alter,api_result,return_pc,candidate):
    cpu=Uc(UC_ARCH_MIPS,UC_MODE_MIPS32|UC_MODE_LITTLE_ENDIAN)
    cpu.mem_map(0,0x200000)
    start,size=struct.unpack_from('<II',exe,0x18)
    cpu.mem_write(start&0x1FFFFFFF,exe[0x800:0x800+size])
    if candidate:
        for address,payload in sections:cpu.mem_write(address,payload)
    initial=bytearray((i*37+19)&255 for i in range(96))
    initial[40:64]=REPLACEMENT if mismatch==6 else ORIGINAL
    if mismatch in range(6):initial[40+mismatch*4]^=1
    expected=bytearray(initial)
    if alter:expected[60]^=0x80
    if expected[40:64]==ORIGINAL:expected[40:64]=REPLACEMENT
    cpu.mem_write(kernel&0x1FFFFFFF,bytes(initial))
    table_data=struct.pack('<7I',*(0x801A0000+i*16 for i in range(6)),kernel)
    cpu.mem_write((table&0x1FFFFFFF)-8,bytes([0xA5])*8+table_data+bytes([0x5A])*8)
    saved=0xA3450
    cpu.mem_write(saved-12,bytes([0xC3])*28)
    apis={int(symbols[name],16):name for name in ('EnterCriticalSection','FlushCache','ExitCriticalSection')}
    apis[0xB0]='GetC0Table'
    for address in apis:cpu.mem_write(address&0x1FFFFFFF,bytes(8))
    pending=[]
    calls=[]
    def hook(cpu,address,size,user):
        if address-4 in apis:
            name=apis[address-4]
            assert struct.unpack('<I',cpu.mem_read(saved,4))[0]==return_pc
            if name=='GetC0Table':assert cpu.reg_read(UC_MIPS_REG_0+9)==0x56
            calls.append((name,bytes(cpu.mem_read(kernel&0x1FFFFFFF,96))))
            pending.append(name)
            cpu.emu_stop()
    cpu.hook_add(UC_HOOK_CODE,hook)
    cpu.reg_write(UC_MIPS_REG_SP,0x801F0000)
    cpu.reg_write(UC_MIPS_REG_RA,return_pc)
    cpu.reg_write(UC_MIPS_REG_A3,0xDEADBEEF)
    for reg in range(16,24):cpu.reg_write(UC_MIPS_REG_0+reg,0x12340000+reg)
    pc=entries['St_InstallDmaHandler'] if candidate else int(symbols['St_InstallDmaHandler'],16)
    for _ in range(8):
        cpu.emu_start(pc,return_pc,count=4000)
        if not pending:break
        name=pending.pop()
        result=api_result
        if name=='GetC0Table':
            result=table
            if alter:
                byte=cpu.mem_read((kernel&0x1FFFFFFF)+60,1)[0]^0x80
                cpu.mem_write((kernel&0x1FFFFFFF)+60,bytes([byte]))
        cpu.reg_write(UC_MIPS_REG_V0,result)
        pc=cpu.reg_read(UC_MIPS_REG_RA)
    else:raise AssertionError('CPU did not return')
    assert cpu.reg_read(UC_MIPS_REG_PC)==return_pc
    assert cpu.reg_read(UC_MIPS_REG_SP)==0x801F0000
    for reg in range(16,24):assert cpu.reg_read(UC_MIPS_REG_0+reg)==0x12340000+reg
    assert calls==[('EnterCriticalSection',bytes(initial)),('GetC0Table',bytes(initial)),('FlushCache',bytes(expected)),('ExitCriticalSection',bytes(expected))],calls
    assert cpu.mem_read(kernel&0x1FFFFFFF,96)==expected
    assert cpu.mem_read(saved-12,12)==bytes([0xC3])*12
    assert cpu.mem_read(saved+4,12)==bytes([0xC3])*12
    assert struct.unpack('<I',cpu.mem_read(saved,4))[0]==return_pc
    assert cpu.mem_read((table&0x1FFFFFFF)-8,44)==bytes([0xA5])*8+table_data+bytes([0x5A])*8
    assert cpu.mem_read(0x7A1E0,24)==ORIGINAL
    assert cpu.mem_read(0x7A1F8,24)==REPLACEMENT
    return calls,bytes(expected)

count=0
for args in itertools.product(range(-1,7),(0x80180000,0x80181000),
                              (0x80190000,0x80190100),(False,True),
                              (0,1,0xFFFFFFFF),(0x801E0000,0x801E0040)):
    assert run(*args,False)==run(*args,True)
    count+=1
print(f'PASS {count} cases: complete/six partial/already-patched patterns, BIOS selector/table offsets, BIOS-time mutation, saved RA, flush/critical-section order, preserved templates/table/canaries and callee-saved registers; BIOS/cache internals not emulated')
