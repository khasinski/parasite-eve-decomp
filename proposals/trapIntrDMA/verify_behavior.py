#!/usr/bin/env python3
"""Compare the DMA dispatcher with retail and a modeled interrupt register."""
import hashlib
import itertools
from pathlib import Path
import re
import struct
import subprocess
import sys
import tempfile
from elftools.elf.elffile import ELFFile
from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN, UC_HOOK_CODE
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

PORTS=(0x1F8010F4,0x1F8010FC)
BASES=(0x1F801080,0x80190000)
CALLBACKS=[0x80160000+i*16 for i in range(9)]
MADR=[[0x12340000+i*0x101 for i in range(7)],[0x89AB0000+i*0x1001 for i in range(7)]]
FORMATS=(0x8001177C,0x80011798)

def master(value):
    value&=0x7FFFFFFF
    if value&0x8000 or (value&0x800000 and value&0x7F000000):value|=0x80000000
    return value

class State:
    def __init__(self,flags,mask,error,mode):
        value=master((flags<<24)|0xFF0000|0x23|(0x8000 if error==1 else 0))
        if error==2:value|=0x80000000
        self.ports={PORTS[0]:value,PORTS[1]:master(0xFF0023)}
        self.pointer=PORTS[0]
        self.slots=[CALLBACKS[i] if mask&(1<<i) else 0 for i in range(7)]+[CALLBACKS[7]]
        self.base=BASES[0]
        self.mode=mode
        self.triggered=False
        self.events=[]
    def snapshot(self):return (self.pointer,tuple(self.ports.items()),tuple(self.slots),self.base)
    def write(self,pointer,value):
        self.events.append(('ack',pointer,value))
        previous=self.ports[pointer]
        written=(previous&~0x00FF807F)|(value&0x00FF807F)
        self.ports[pointer]=master(written&~(value&0x7F000000))
    def callback(self,index):
        self.events.append(('callback',index,self.snapshot()))
        if self.mode and not self.triggered:
            self.triggered=True
            self.slots[0]=0
            self.slots[6]=CALLBACKS[8]
            if self.mode==2:self.pointer=PORTS[1]
            self.ports[self.pointer]=master(self.ports[self.pointer]|0x41000000)
    def printing(self,args):
        self.events.append(('printf',*args))
        if self.mode:
            if args[0]==FORMATS[0]:self.base=BASES[1]
            elif self.mode==2:self.base=BASES[0] if self.base==BASES[1] else BASES[1]

def model(flags,mask,error,mode):
    state=State(flags,mask,error,mode)
    for _ in range(20):
        active=(state.ports[state.pointer]>>24)&127
        if not active:break
        for channel in range(7):
            if active&(1<<channel):
                state.write(state.pointer,state.ports[state.pointer]&(0xFFFFFF|(1<<(24+channel))))
                callback=state.slots[channel]
                if callback:state.callback(CALLBACKS.index(callback))
    else:raise AssertionError('model failed to drain')
    value=state.ports[state.pointer]
    if value&0xFF000000==0x80000000 or value&0x8000:
        state.printing((FORMATS[0],value))
        for channel in range(7):state.printing((FORMATS[1],channel,MADR[BASES.index(state.base)][channel]))
    return state.events,state.snapshot()

def run(flags,mask,error,mode,candidate):
    state=State(flags,mask,error,mode)
    cpu=Uc(UC_ARCH_MIPS,UC_MODE_MIPS32|UC_MODE_LITTLE_ENDIAN)
    cpu.mem_map(0,0x200000)
    cpu.mem_map(0x1F801000,0x1000)
    start,size=struct.unpack_from('<II',exe,0x18)
    cpu.mem_write(start&0x1FFFFFFF,exe[0x800:0x800+size])
    if candidate:
        for address,payload in sections:cpu.mem_write(address,payload)
    def sync_globals():
        cpu.mem_write(0x956BC,struct.pack('<10I',state.pointer,*state.slots,state.base))
        for pointer,value in state.ports.items():cpu.mem_write(pointer,struct.pack('<I',value))
    sync_globals()
    cpu.mem_write(0x956B4,bytes([0xA5])*8)
    cpu.mem_write(0x956E4,bytes([0x5A])*8)
    for base,values in zip(BASES,MADR):
        for channel,value in enumerate(values):cpu.mem_write((base&0x1FFFFFFF)+channel*16,struct.pack('<4I',value,0x11223344,0x55667788,0xAABBCCDD))
    printf=int(symbols['printf'],16)
    for address in CALLBACKS+[printf]:cpu.mem_write(address&0x1FFFFFFF,bytes(8))
    pending=[]
    repair=[]
    def hook(cpu,address,size,user):
        if repair:
            for pointer in repair:cpu.mem_write(pointer,struct.pack('<I',state.ports[pointer]))
            repair.clear()
        if address-4 in CALLBACKS or address==printf+4:
            pending.append(address-4)
            cpu.emu_stop()
            return
        word=struct.unpack('<I',cpu.mem_read(address&0x1FFFFFFF,4))[0]
        opcode=word>>26
        if opcode not in (0x23,0x2B):return
        rs=(word>>21)&31;rt=(word>>16)&31
        offset=word&65535
        if offset&32768:offset-=65536
        pointer=(cpu.reg_read(UC_MIPS_REG_0+rs)+offset)&0x1FFFFFFF
        if pointer not in PORTS:return
        if opcode==0x23:
            cpu.mem_write(pointer,struct.pack('<I',state.ports[pointer]))
        else:
            state.write(pointer,cpu.reg_read(UC_MIPS_REG_0+rt))
            repair.append(pointer)
    cpu.hook_add(UC_HOOK_CODE,hook)
    cpu.reg_write(UC_MIPS_REG_SP,0x801F0000)
    cpu.reg_write(UC_MIPS_REG_RA,0x801E0000)
    for reg in range(16,24):cpu.reg_write(UC_MIPS_REG_0+reg,0x12340000+reg)
    pc=entries['trapIntrDMA'] if candidate else int(symbols['trapIntrDMA'],16)
    for _ in range(40):
        cpu.emu_start(pc,0x801E0000,count=10000)
        if not pending:break
        address=pending.pop()
        assert bytes(cpu.mem_read(0x956BC,40))==struct.pack('<10I',state.pointer,*state.slots,state.base)
        if address==printf:
            fmt=cpu.reg_read(UC_MIPS_REG_A0)
            assert fmt in FORMATS
            args=(fmt,cpu.reg_read(UC_MIPS_REG_A1))
            if fmt==FORMATS[1]:args+= (cpu.reg_read(UC_MIPS_REG_A2),)
            state.printing(args)
        else:state.callback(CALLBACKS.index(address))
        sync_globals()
        cpu.reg_write(UC_MIPS_REG_V0,0xFFFFFFFF)
        pc=cpu.reg_read(UC_MIPS_REG_RA)
    else:raise AssertionError('CPU did not finish')
    assert cpu.reg_read(UC_MIPS_REG_PC)==0x801E0000
    assert cpu.reg_read(UC_MIPS_REG_SP)==0x801F0000
    for reg in range(16,24):assert cpu.reg_read(UC_MIPS_REG_0+reg)==0x12340000+reg
    assert cpu.mem_read(0x956B4,8)==bytes([0xA5])*8
    assert cpu.mem_read(0x956E4,8)==bytes([0x5A])*8
    assert cpu.mem_read(0x956BC,40)==struct.pack('<10I',state.pointer,*state.slots,state.base)
    for base,values in zip(BASES,MADR):
        for channel,value in enumerate(values):
            assert cpu.mem_read((base&0x1FFFFFFF)+channel*16,16)==struct.pack('<4I',value,0x11223344,0x55667788,0xAABBCCDD)
    return state.events,state.snapshot()

count=0
for args in itertools.product(range(128),(0,127,0x55,0x2A),range(3),range(3)):
    expected=model(*args)
    assert run(*args,False)==expected,(args,'retail')
    assert run(*args,True)==expected,(args,'candidate')
    count+=1
print(f'PASS {count} cases: all DMA pending masks, W1C acknowledgements, callback mutations/new IRQs/status-pointer replacement, diagnostic base reloads, register/canary preservation; DMA transfers and interrupt timing not emulated')
