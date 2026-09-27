"""Bouncing glow: retail/C comparison against a bounded independent model.

Valid disjoint state/palette storage. Vector padding is unspecified; only
defined components are compared. Renderer/CLUT call stubs are deterministic.
"""
from pathlib import Path
import itertools
import struct
import unittest
from importlib.util import find_spec

ROOT = Path(__file__).resolve().parents[2]


class BouncingGlowTests(unittest.TestCase):
    def test_plain_source(self):
        source = (ROOT/'src/main/engine/FieldEng_BouncingGlow.c').read_text()
        self.assertNotRegex(source, r'\b(?:asm|__asm__|INCLUDE_ASM|REGALLOC_BARRIER)\b')

    @unittest.skipUnless((ROOT/'assets/USA/main.exe').is_file() and
                         (ROOT/'build/USA/main.exe').is_file() and
                         find_spec('unicorn'), 'images or unicorn unavailable')
    def test_behavior(self):
        from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN, UC_HOOK_CODE
        from unicorn import mips_const as R
        BASE,SIZE=0x800D6C58,484
        STOP,SP,GP=0x80010000,0x801F0000,0x8009CD70
        syms={'GetClut':0x80077AA4,'func_800CEE20':0x800CEE20}
        retail,code=[(ROOT/p).read_bytes()[BASE-0x8000F800:BASE-0x8000F800+SIZE]
                     for p in ('assets/USA/main.exe','build/USA/main.exe')]
        self.assertEqual(len(code),SIZE)
        self.assertEqual(code,retail)
        STATE,TIME,SELECTOR,FLAG,PALETTE,COLOR=0x80100020,0x800E27EC,0x800F336C,0x800F3428,0x800E1204,0x800C22D0
        regions=((STATE-8,32),(TIME-8,20),(SELECTOR-8,20),(FLAG-8,20),(PALETTE-8,32),(COLOR-8,20))
        cases=list(itertools.product((-1,0,1,2,3),(-2147483648,-32768,-1,0,23,24,32767,2147483647),
            (-32768,-32767,-33,-1,0,1,32766,32767),(0,1,4,7),(0,1)))
        cases=[(*case,-case[2]-1) for case in cases]
        # Independent vertical speeds exercise both sides of the bounce test;
        # the base patterns deliberately land exactly at y == 0.
        cases += list(itertools.product((1,),(-2147483648,-32768,-1,0,23,24,32767,2147483647),
            (-32768,-32767,-33,-1,0,1,32766,32767),(0,),(0,),(-32768,-1,0,1,32767)))
        saved=[getattr(R,f'UC_MIPS_REG_S{i}') for i in range(8)]+[R.UC_MIPS_REG_FP]
        def sx(v,n):return ((v+(1<<(n-1)))&((1<<n)-1))-(1<<(n-1))
        def trunc(v,d):return (-1 if v<0 else 1)*(abs(v)//d)
        for label,body in (('retail',retail),('candidate',code)):
            m=Uc(UC_ARCH_MIPS,UC_MODE_MIPS32|UC_MODE_LITTLE_ENDIAN);m.mem_map(0,0x200000)
            def put(a,b):m.mem_write(a&0x1FFFFFFF,bytes(b))
            def read(a,n):return bytes(m.mem_read(a&0x1FFFFFFF,n))
            def get(a,n=4):return int.from_bytes(read(a,n),'little')
            def set_(a,v,n=4):put(a,(v&((1<<(8*n))-1)).to_bytes(n,'little'))
            put(BASE,body)
            for address in (syms['GetClut'],syms['func_800CEE20']):put(address,struct.pack('<II',0x03E00008,0))
            actual=[]
            def callback(kind,args,g,s,events):
                events.append((kind,args))
                if kind=='clut':
                    if mutation:
                        for i in range(8):s(STATE+2*i,0xABCD+i,2)
                        s(TIME,0xDEADBEEF);s(COLOR,0x12345678);s(SELECTOR,0,2);s(FLAG,0)
                    return 0x9876ABCD
                return 0
            def hook(uc,address,size,data):
                if address not in (syms['GetClut'],syms['func_800CEE20']):return
                a=[uc.reg_read(getattr(R,f'UC_MIPS_REG_A{i}')) for i in range(4)]
                if address==syms['GetClut']:kind='clut';args=tuple(a[:2])
                else:
                    kind='draw';sp=uc.reg_read(R.UC_MIPS_REG_SP)
                    args=(tuple(get(a[0]+i,2) for i in (0,2,4)),tuple(get(a[1]+i,2) for i in (0,2,4)),
                          a[2],a[3],*(get(sp+i) for i in (16,20,24,28)),read(get(sp+32),4))
                result=callback(kind,args,get,set_,actual)
                for name in ('V1','A0','A1','A2','A3','T0','T1','T2','T3','T4','T5','T6','T7','T8','T9'):
                    uc.reg_write(getattr(R,'UC_MIPS_REG_'+name),0xCCCCCCCC)
                uc.reg_write(R.UC_MIPS_REG_V0,result)
            m.hook_add(UC_HOOK_CODE,hook)
            for mode,time,value,selector,mutation,vertical in cases:
                case=(label,mode,time,value,selector,mutation,vertical)
                expected={a:0xA5 for start,size in regions for a in range(start,start+size)}
                def g(a,n=4):return int.from_bytes(bytes(expected[a+i] for i in range(n)),'little')
                def s(a,v,n=4):
                    for i,b in enumerate((v&((1<<(8*n))-1)).to_bytes(n,'little')):expected[a+i]=b
                for i,v in enumerate((value,value+1,-value,value,vertical,value+2,value,value^0xFFFF)):s(STATE+2*i,v,2)
                s(TIME,time);s(SELECTOR,selector,2);s(FLAG,mutation);s(COLOR,0x11C93F21)
                for i in range(8):s(PALETTE+2*i,i*9999,2)
                for start,size in regions:put(start,bytes(expected[a] for a in range(start,start+size)))
                events=[];result=0
                if mode==1:
                    for i in (0,2,4):s(STATE+i,g(STATE+i,2)+g(STATE+i+6,2),2)
                    for i in (6,10):s(STATE+i,trunc(sx(g(STATE+i,2),16)*31,32),2)
                    if sx(g(STATE+2,2),16)>0:s(STATE+8,-sx(g(STATE+8,2),16),2)
                    s(STATE+8,g(STATE+8,2)+3,2)
                    result=int(sx(g(TIME),32)>=sx(g(STATE+12,2),16))
                elif mode==2:
                    pos=tuple(g(STATE+i,2) for i in (0,2,4))
                    rot=(0,0,((g(STATE+14,2)<<8)+(g(TIME)<<7))&65535)
                    scale=(g(STATE+14,2)&2047)+1024
                    color=g(COLOR).to_bytes(4,'little')
                    y=g(PALETTE+selector*2,2)+(4 if selector==4 and g(FLAG) else 0)
                    clut=callback('clut',(128,y),g,s,events)&65535
                    callback('draw',(pos,rot,scale,scale,188,clut,255,128,color),g,s,events)
                actual.clear()
                for name,v in (('A0',mode),('A1',STATE),('SP',SP),('GP',GP),('RA',STOP)):
                    m.reg_write(getattr(R,'UC_MIPS_REG_'+name),v&0xFFFFFFFF)
                for i,reg in enumerate(saved):m.reg_write(reg,0xABCD0000+i)
                m.emu_start(BASE,STOP,count=2000)
                assert actual==events,(case,actual,events)
                for start,size in regions:assert read(start,size)==bytes(expected[a] for a in range(start,start+size)),(case,hex(start))
                assert m.reg_read(R.UC_MIPS_REG_V0)==result,case
                for name,v in (('PC',STOP),('SP',SP),('GP',GP)):assert m.reg_read(getattr(R,'UC_MIPS_REG_'+name))==v,case
                for i,reg in enumerate(saved):assert m.reg_read(reg)==0xABCD0000+i,case


if __name__ == '__main__':
    unittest.main()
