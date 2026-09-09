#!/usr/bin/env python3
"""Compare ordinary-C InitGeom initialization against retail with modeled CP0/COP2 transfers and installer."""
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

def run(initial_status,modify,api_return,return_pc,fill,candidate):
    cpu=Uc(UC_ARCH_MIPS,UC_MODE_MIPS32|UC_MODE_LITTLE_ENDIAN)
    cpu.mem_map(0,0x200000)
    start,size=struct.unpack_from('<II',exe,0x18)
    cpu.mem_write(start&0x1FFFFFFF,exe[0x800:0x800+size])
    if candidate:
        for address,payload in sections:cpu.mem_write(address,payload)
    pc=entries['InitGeom'] if candidate else int(symbols['InitGeom'],16)
    code_size=next(len(payload) for address,payload in sections if address==0x150000) if candidate else 0x80
    operations={}
    for address in range(pc,pc+code_size,4):
        word=struct.unpack('<I',cpu.mem_read(address&0x1FFFFFFF,4))[0]
        if word>>26 in (0x10,0x12):
            operations[address]=word
            cpu.mem_write(address&0x1FFFFFFF,bytes(4))
    installer=int(symbols['St_InstallDmaHandler'],16)
    cpu.mem_write(installer&0x1FFFFFFF,bytes(8))
    status=initial_status
    controls={i:((i+1)*0x1234567)^fill for i in range(32)}
    initial_controls=dict(controls)
    events=[]
    pending=False
    saved=0x960AC
    cpu.mem_write(saved-16,bytes([fill])*36)
    cpu.mem_write(0x1EFFC0,bytes([fill])*64)
    def hook(cpu,address,size,user):
        nonlocal pending,status
        if address==installer+4:
            events.append(('install',struct.unpack('<I',cpu.mem_read(saved,4))[0],status,dict(controls)))
            pending=True
            cpu.emu_stop()
            return
        if address not in operations:return
        word=operations[address];opcode=word>>26
        rt=(word>>16)&31;rd=(word>>11)&31;rs=(word>>21)&31
        if opcode==0x10 and rs==0:
            assert rd==12
            events.append(('read_status',status))
            cpu.reg_write(UC_MIPS_REG_0+rt,status)
        elif opcode==0x10 and rs==4:
            assert rd==12
            status=cpu.reg_read(UC_MIPS_REG_0+rt)
            events.append(('write_status',status))
        elif opcode==0x12 and rs==6:
            value=cpu.reg_read(UC_MIPS_REG_0+rt)
            events.append(('write_control',rd,value))
            controls[rd]=value
        else:raise AssertionError(hex(word))
    cpu.hook_add(UC_HOOK_CODE,hook)
    cpu.reg_write(UC_MIPS_REG_SP,0x801F0000)
    cpu.reg_write(UC_MIPS_REG_RA,return_pc)
    while True:
        cpu.emu_start(pc,return_pc,count=4000)
        if not pending:break
        pending=False
        if modify:
            status^=0xA5000311
            controls[0]^=0xDEADBEEF
        cpu.reg_write(UC_MIPS_REG_V0,api_return)
        pc=cpu.reg_read(UC_MIPS_REG_RA)
    assert cpu.reg_read(UC_MIPS_REG_PC)==return_pc
    assert cpu.reg_read(UC_MIPS_REG_SP)==0x801F0000
    expected_status=(initial_status^(0xA5000311 if modify else 0))
    writes=((29,0x155),(30,0x100),(26,1000),(27,0xFFFFEF9E),(28,0x1400000),(24,0),(25,0))
    expected_events=[('install',return_pc,initial_status,initial_controls),('read_status',expected_status),('write_status',expected_status|0x40000000)]
    expected_events += [('write_control',r,v) for r,v in writes]
    assert events==expected_events,(events,expected_events)
    expected_controls=dict(initial_controls)
    if modify:expected_controls[0]^=0xDEADBEEF
    expected_controls.update(writes)
    assert controls==expected_controls
    assert status==expected_status|0x40000000
    assert cpu.mem_read(saved-16,16)==bytes([fill])*16
    assert cpu.mem_read(saved+4,16)==bytes([fill])*16
    assert struct.unpack('<I',cpu.mem_read(saved,4))[0]==return_pc
    return events,controls,status

count=0
for args in itertools.product((0,1,0xFFFFFFFF,0x80000000,0x40000000,0x00400000,0x12345678,0xDEADBEEF),
                              (False,True),(0,1,0xFFFFFFFF),(0x801E0000,0x801E0040),(0,0xA5)):
    assert run(*args,False)==run(*args,True)
    count+=1
print(f'PASS {count} cases: saved caller RA, installer order, post-installer status, CU2 bit preservation, seven GTE writes, unrelated control preservation, return/SP and global canaries; hardware timing and installer internals not emulated')
