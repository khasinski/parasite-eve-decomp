"""Fading emitter: retail/C and independent state-transition model.

Valid disjoint storage and palette indices. Table prefix covers count -1;
no claim about arbitrary negative table indices or physical rendering.
"""
from pathlib import Path
import itertools
import struct
import unittest
from importlib.util import find_spec

ROOT = Path(__file__).resolve().parents[2]


class FadingEmitterTests(unittest.TestCase):
    def test_plain_source(self):
        source = (ROOT/'src/main/engine/FieldEng_FadingEmitter.c').read_text()
        self.assertNotRegex(source, r'\b(?:asm|__asm__|INCLUDE_ASM|REGALLOC_BARRIER)\b')

    @unittest.skipUnless((ROOT/'assets/USA/main.exe').is_file() and
                         (ROOT/'build/USA/main.exe').is_file() and
                         find_spec('unicorn'), 'images or unicorn unavailable')
    def test_behavior(self):
        from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN, UC_HOOK_CODE
        from unicorn import mips_const as R
        BASE,SIZE=0x800DF6AC,464
        STOP,SP,GP=0x80010000,0x801F0000,0x8009CD70
        syms={'func_800CE560':0x800CE560,'func_800CE610':0x800CE610,
              'func_800CEDA8':0x800CEDA8,'func_800DEFFC':0x800DEFFC}
        retail,code=[(ROOT/p).read_bytes()[BASE-0x8000F800:BASE-0x8000F800+SIZE]
                     for p in ('assets/USA/main.exe','build/USA/main.exe')]
        self.assertEqual(len(code),SIZE)
        self.assertEqual(code,retail)
        STATE,SLOT,PARTICLE=0x80100020,0x80100120,0x80100220
        TIME,CHANNEL,TABLE,PARAMS,PAGEINDEX,PAGES,INTENSITY=0x800E27EC,0x800F33E0,0x800E2164,0x800F3368,0x800E11E6,0x800E2850,0x800E2244
        regions=((STATE-8,20),(SLOT-8,28),(PARTICLE-8,36),(TIME-8,20),(CHANNEL-8,20),
                 (TABLE-8,32),(PARAMS-8,36),(PAGEINDEX-8,20),(PAGES-8,32),(INTENSITY-8,20))
        cases=list(itertools.product((-1,0,1,2,3),(-1,0,7,8,32767),(-32768,-1,0,1,2,3,160,32767),
                                    (-2,0,3,4,31,32,33,2147483647),(0,1,2)))
        saved=[getattr(R,f'UC_MIPS_REG_S{i}') for i in range(8)]+[R.UC_MIPS_REG_FP]
        def sx(v,n):return ((v+(1<<(n-1)))&((1<<n)-1))-(1<<(n-1))
        hooks={syms['func_800CE560']:'init',syms['func_800CE610']:'alloc',syms['func_800CEDA8']:'display'}
        for label,body in (('retail',retail),('candidate',code)):
            m=Uc(UC_ARCH_MIPS,UC_MODE_MIPS32|UC_MODE_LITTLE_ENDIAN);m.mem_map(0,0x200000)
            def put(a,b):m.mem_write(a&0x1FFFFFFF,bytes(b))
            def read(a,n):return bytes(m.mem_read(a&0x1FFFFFFF,n))
            def get(a,n=4):return int.from_bytes(read(a,n),'little')
            def set_(a,v,n=4):put(a,(v&((1<<(8*n))-1)).to_bytes(n,'little'))
            put(BASE,body)
            for address in hooks:put(address,struct.pack('<II',0x03E00008,0))
            actual=[]
            def callback(kind,args,g,s,events):
                events.append((kind,args,g(STATE,2),g(STATE+2,2),g(TIME),tuple(g(PARAMS+i,2) for i in range(0,18,2)),g(INTENSITY,2)))
                if kind=='init':
                    if mutation:s(STATE,6,2);s(STATE+2,-3,2);s(TIME,99)
                    return (0,0x81234567,33)[mutation]
                if kind=='alloc':
                    if mutation==1:s(STATE,5,2);s(STATE+2,-32768,2);s(TIME,32)
                    return 0 if mutation==2 else PARTICLE
                if mutation:
                    for i in range(0,18,2):s(PARAMS+i,0xD000+i,2)
                    s(STATE+2,123,2);s(INTENSITY,456,2)
                return 0
            def hook(uc,address,size,data):
                if address not in hooks:return
                kind=hooks[address];argc=4 if kind=='init' else 1
                args=tuple(uc.reg_read(getattr(R,f'UC_MIPS_REG_A{i}')) for i in range(argc))
                result=callback(kind,args,get,set_,actual)
                for name in ('V1','A0','A1','A2','A3','T0','T1','T2','T3','T4','T5','T6','T7','T8','T9'):
                    uc.reg_write(getattr(R,'UC_MIPS_REG_'+name),0xCCCCCCCC)
                uc.reg_write(R.UC_MIPS_REG_V0,result)
            m.hook_add(UC_HOOK_CODE,hook)
            for mode,count,intensity,time,mutation in cases:
                case=(label,mode,count,intensity,time,mutation)
                expected={a:0xA5 for start,size in regions for a in range(start,start+size)}
                def g(a,n=4):return int.from_bytes(bytes(expected[a+i] for i in range(n)),'little')
                def s(a,v,n=4):
                    for i,b in enumerate((v&((1<<(8*n))-1)).to_bytes(n,'little')):expected[a+i]=b
                s(STATE,count,2);s(STATE+2,intensity,2);s(TIME,time);s(CHANNEL,SLOT);s(SLOT+8,0x80102000)
                s(PAGEINDEX,mutation*3,2)
                for i in range(-1,8):s(TABLE+i,(i*37+13)&255,1)
                for i in range(8):s(PAGES+i*2,i*9999,2)
                for start,size in regions:put(start,bytes(expected[a] for a in range(start,start+size)))
                events=[];result=0
                def call(kind,*args):return callback(kind,tuple(args),g,s,events)
                if mode==0:
                    s(STATE,0,2);s(STATE+2,160,2)
                    result=call('init',g(g(CHANNEL)+8),20,32,syms['func_800DEFFC'])
                elif mode==1:
                    if g(TIME)&3==0 and sx(g(STATE,2),16)<8:
                        p=call('alloc',g(g(CHANNEL)+8))
                        if p:
                            kind=g(TABLE+sx(g(STATE,2),16),1)
                            s(p+16,0,2);s(p+18,0,2);s(p+6,kind,2)
                        s(STATE,g(STATE,2)+1,2)
                    if sx(g(TIME),32)>=32:s(STATE+2,g(STATE+2,2)-2,2)
                    if sx(g(STATE+2,2),16)<=0:s(STATE+2,0,2);result=1
                elif mode==2:
                    old=g(STATE+2,2)
                    for off,v in ((0,32),(2,2),(14,32),(16,32)):s(PARAMS+off,v,2)
                    page=g(PAGES+2*g(PAGEINDEX,2),2)
                    s(INTENSITY,old,2);s(PARAMS+4,1,2);s(PARAMS+8,page,2)
                    call('display',1)
                    for off,v in ((6,0),(10,0),(12,24)):s(PARAMS+off,v,2)
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
