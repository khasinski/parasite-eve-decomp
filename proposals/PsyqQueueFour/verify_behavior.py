#!/usr/bin/env python3
"""Execute retail and rebuilt Psy-Q code against independent state models.

Requires the local retail EXE, Unicorn, pyelftools and native PSX toolchain.
Run from the repository root: .venv/bin/python proposals/PsyqQueueFour/verify_behavior.py
"""
import random
import struct
import subprocess
import tempfile
from pathlib import Path
from elftools.elf.elffile import ELFFile
from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN, UC_HOOK_CODE
from unicorn.mips_const import *
import sys
sys.path.insert(0, str(Path(__file__).resolve().parents[2]))
from tools.tests.test_psyq_queue_four import CASES

ENTRY = [0x8007EE84, 0x8007F0C8, 0x8007F418, 0x8007E4E0]
Q, PENDING, SERIAL, HISTORY, CURSOR, PUBLISHED = 0x800A3540, 0x800A3608, 0x8009B53C, 0x800A3610, 0x800A3690, 0x800A3500
PARAM, OUT, STACK, RETURN = 0x80180020, 0x80180120, 0x801FF000, 0x8000F000
ALLOC, COPY4, COPY8, QUEUE_STATE, DISPATCH, POLL, POS = 0x8007E6B0, 0x80080950, 0x80080998, 0x8007FBF0, 0x8007E8F4, 0x8007FC64, 0x80080C48

def pack(x): return struct.pack('<I', x & 0xFFFFFFFF)
def get(b, at=0): return struct.unpack_from('<I', b, at)[0]
def signed(x): return x if x < 0x80000000 else x - 0x100000000

def run(exe, which, case, replacement):
    cpu = Uc(UC_ARCH_MIPS, UC_MODE_MIPS32 | UC_MODE_LITTLE_ENDIAN)
    cpu.mem_map(0, 0x200000)
    def write(a,b): cpu.mem_write(a & 0x1FFFFFFF, bytes(b))
    def read(a,n): return bytes(cpu.mem_read(a & 0x1FFFFFFF,n))
    def word(a,v): write(a,pack(v))
    start,size = struct.unpack_from('<II',exe,0x18)
    write(start,exe[0x800:0x800+size])
    if replacement:
        for addr,data in replacement: write(addr,data)
    write(STACK-256,bytes([case['stack_fill']])*288)
    cpu.reg_write(UC_MIPS_REG_SP,STACK)
    cpu.reg_write(UC_MIPS_REG_RA,RETURN)
    cpu.reg_write(UC_MIPS_REG_GP,0x8009CD70)
    saved = [0xCAFE0010+i for i in range(8)]
    for i,v in enumerate(saved):cpu.reg_write(UC_MIPS_REG_S0+i,v)
    trace=[]; allocations=0
    write(PARAM,case['payload']);write(OUT-8,b'\x6D'*32)
    if which < 2:
        write(Q,b'\xA5'*192);word(PENDING,case['pending']);word(SERIAL,case['serial']);word(0x800A3604,0)
        write(0x8009B4BC,bytes(1024));word(0x8009B4BC+4*(case['command']&255),case['prefix']);word(SERIAL,case['serial'])
        args=([case['command'],PARAM if case['parameter'] else 0,case['callback'],case['count']] if which==0 else [case['mode'],PARAM,case['command'],case['callback']])
        word(STACK+16,case['count'])
    elif which==2:
        write(HISTORY,case['history']);word(CURSOR,case['cursor']);write(PUBLISHED,b'\xA5'*16)
        args=[case['id'],OUT]
    else:
        write(0xDF70,b'\x6D'*144)
        # Pattern the source image to check every copied word, independently of code content.
        write(0x8007E344,case['image']);args=[]
    for i,v in enumerate(args):cpu.reg_write(UC_MIPS_REG_A0+i,v&0xFFFFFFFF)
    def helper(cpu,address,size,_):
        nonlocal allocations
        a0=cpu.reg_read(UC_MIPS_REG_A0);a1=cpu.reg_read(UC_MIPS_REG_A1)
        value=0
        if address==ALLOC:
            value=0 if which==1 and allocations==case['fail_at'] else Q+allocations*24
            allocations+=1;trace.append(('alloc',value))
        elif address in (COPY4,COPY8):
            n=4 if address==COPY4 else 8
            if a0:write(a0,read(a1,n))
        elif address==QUEUE_STATE:value=case['state'];trace.append(('state',value))
        elif address==DISPATCH:trace.append(('dispatch',))
        elif address==POLL:trace.append(('poll',))
        elif address==POS:value=case['position_result'];trace.append(('position',))
        cpu.reg_write(UC_MIPS_REG_V0,value&0xFFFFFFFF)
        cpu.reg_write(UC_MIPS_REG_PC,cpu.reg_read(UC_MIPS_REG_RA))
    for addr in [ALLOC,COPY4,COPY8,QUEUE_STATE,DISPATCH,POLL,POS]:cpu.hook_add(UC_HOOK_CODE,helper,begin=addr,end=addr)
    cpu.emu_start(ENTRY[which],RETURN,count=20000)
    assert cpu.reg_read(UC_MIPS_REG_PC)==RETURN,'did not return'
    assert cpu.reg_read(UC_MIPS_REG_SP)==STACK
    assert [cpu.reg_read(UC_MIPS_REG_S0+i) for i in range(8)]==saved
    result=cpu.reg_read(UC_MIPS_REG_V0)
    if which<2:return result,read(Q,192),get(read(PENDING,4)),get(read(SERIAL,4)),trace
    if which==2:return result,read(PUBLISHED,16),read(OUT-8,32),read(HISTORY,128),trace
    return read(0xDF70,144)

def queue_model(which,c):
    queue=bytearray(b'\xA5'*192);pending=c['pending'];serial=c['serial'];trace=[];allocated=0
    def ticket():
        nonlocal serial
        serial=(serial+1)&0xFFFFFFFF
        if not serial:serial=1
        return serial
    def dispatch(t):
        trace.append(('state',c['state']))
        if c['state']==1 and get(queue)==t:trace.append(('dispatch',))
    def enqueue(t,command,payload,parameter,callback,count):
        nonlocal allocated,pending
        if which==1 and allocated==c['fail_at']:
            trace.append(('alloc',0));allocated+=1;return False
        at=allocated*24;trace.append(('alloc',Q+at));allocated+=1
        queue[at:at+4]=pack(t);queue[at+4]=command&255
        if parameter:queue[at+5:at+9]=payload
        queue[at+12:at+16]=pack(Q+at+5 if which==1 and parameter else parameter)
        queue[at+16:at+20]=pack(callback);queue[at+20:at+24]=pack(count);pending+=1
        return True
    result=0
    if which==0:
        commands=([2] if c['prefix'] and c['parameter'] else [])+[c['command']]
        for i,command in enumerate(commands):
            if pending>=8:result=0;break
            result=ticket();last=i==len(commands)-1
            enqueue(result,command,c['payload'],PARAM if c['parameter'] else 0,c['callback'] if last else 0,c['count'] if last else 0)
            dispatch(result)
    else:
        trace.append(('position',))
        if c['position_result']>=0 and (c['command']&255) in [3,6,21,22,27] and pending+4<9:
            result=ticket()
            # Only the mode byte is initialized in this stack packet; the other
            # three bytes retain the patterned stack contents, copied by retail too.
            for command,payload,param,callback in [(9,bytes(4),0,0),(14,bytes([c['mode']&255])+bytes([c['stack_fill']])*3,1,0),(2,c['payload'],1,0),(c['command'],bytes(4),0,c['callback'])]:
                if not enqueue(result,command,payload,param,callback,c['count']):result=0;break
            if result:dispatch(result)
    return result,bytes(queue),pending,serial,trace

def sync_model(c):
    records=[c['history'][i:i+16] for i in range(0,128,16)];ids=[signed(get(r)) for r in records];id=signed(c['id']);cursor=c['cursor'];selected=None;trace=[]
    if id:
        trace=[('poll',)]
        if id not in ids and id>=ids[cursor]:return 0,b'\xA5'*16,b'\x6D'*32,c['history'],trace
        for step in range(8):
            index=(cursor-1-step)%8
            if ids[index]==id:selected=records[index];break
    elif ids[(cursor-1)%8]:selected=records[(cursor-1)%8]
    out=bytearray(b'\x6D'*32)
    if selected:out[8:16]=selected[5:13]
    return selected[4] if selected else 6,selected or b'\xA5'*16,bytes(out),c['history'],trace

def main():
    exe=Path('assets/USA/main.exe').read_bytes();rng=random.Random(430);replacements=[]
    with tempfile.TemporaryDirectory() as d:
        d=Path(d)
        for name,size,digest,script in CASES:
            obj,elf,ld=d/'code.o',d/'code.elf',d/'code.ld';ld.write_text(script)
            subprocess.run(['tools/scripts/cc.sh',f'src/main/psyq/{name}.c',str(obj)],check=True,capture_output=True)
            subprocess.run(['mipsel-none-elf-ld','-T',str(ld),str(obj),'-o',str(elf)],check=True)
            with elf.open('rb') as f:
                replacements.append([(s['sh_addr'],s.data()) for s in ELFFile(f).iter_sections() if s.name in ['.text','.rodata']])
    counts=[0]*4
    for which in range(4):
        for n in range(512):
            c=dict(stack_fill=rng.randrange(256),payload=rng.randbytes(4),pending=n%9,serial=[0,1,0xFFFFFFFE,0xFFFFFFFF][n%4],command=[3,6,21,22,27,0,2,14,255,0x103,-1][n%11],prefix=(n//4)%2,parameter=(n//8)%2,callback=rng.getrandbits(32),count=rng.getrandbits(32),mode=rng.getrandbits(32),state=(n//16)%3,position_result=-1 if n%13==0 else 0,fail_at=[-1,0,1,2,3][n%5])
            ids=[rng.choice([0,1,2,10,11,99,0x7FFFFFFF,0x80000000]) for _ in range(8)]
            c.update(history=b''.join(pack(x)+rng.randbytes(12) for x in ids),cursor=n%8,id=rng.choice([0,1,3,10,12,100,0x7FFFFFFF,0x80000000]),image=rng.randbytes(112))
            expected=queue_model(which,c) if which<2 else sync_model(c) if which==2 else b'\x6D'*16+c['image']+b'\x6D'*16
            retail=run(exe,which,c,None);rebuilt=run(exe,which,c,replacements[which])
            assert retail==expected,(which,n,'model',retail,expected,c)
            assert rebuilt==expected,(which,n,'rebuilt',rebuilt,expected,c)
            counts[which]+=1
    print(f'OK: {sum(counts)} cases x retail/rebuilt, independent models; per function: {counts}')
if __name__=='__main__':main()
