"""Turn rewind: exact linked bytes and independent slot/state model.

Valid initial indices 1..45, disjoint actor/action storage; includes mutation
by UndoPending, unsigned index wrap and signed generation cleanup.
"""
from pathlib import Path
import itertools
import struct
import unittest
from importlib.util import find_spec

ROOT = Path(__file__).resolve().parents[2]


class AdvanceTurnSlotTests(unittest.TestCase):
    @unittest.skipUnless((ROOT/'assets/USA/main.exe').is_file() and
                         (ROOT/'build/USA/main.exe').is_file() and
                         find_spec('unicorn'), 'images or unicorn unavailable')
    def test_behavior(self):
        from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN, UC_HOOK_CODE
        from unicorn import mips_const as R
        BASE, SIZE = 0x80026CF0, 0x2E0
        STOP, SP, GP = 0x80010000, 0x801F0000, 0x8009CD70
        start = BASE - 0x8000F800
        retail, code = [(ROOT/p).read_bytes()[start:start+SIZE]
                        for p in ('assets/USA/main.exe', 'build/USA/main.exe')]
        self.assertEqual(len(code), SIZE)
        self.assertEqual(code, retail)
        syms = {'BattleCmd_UndoPending': 0x8005112C}
        INDEX, CLEAR, PENDING = 0x8009CE3C,0x8009CE44,0x8009CE60
        COUNT, SAVED, SUB, ACTOR = 0x8009D2D8,0x8009D1DC,0x8009D2B0,0x8009D278
        SLOTS, CORE, ACTION = 0x800BE830,0x80100000,0x80101000
        regions=((0x8009CE30,64),(0x8009D1D0,16),(0x8009D270,128),
                 (SLOTS-8,45*8+16),(CORE,0x180),(ACTION,0x80))
        cases=list(itertools.product((1,2,23,45),(-32768,0,1,2,3,406,407,32767),
            (0,0x41,0x4F,0xC0,0xCF,0x80),(0,127,128,255),(0,1),(0,1,2,3)))
        cases += list(itertools.product(range(1,46),(1,2,406),(0x40,0x4F,0xCF),(255,),(0,1),(0,1,2,3)))
        cases += list(itertools.product((23,),(1,2,407),(0x4F,),range(256),(0,),(0,1)))
        cases += list(itertools.product((23,),(1,2),range(256),(127,),(0,),(1,)))
        saved=[getattr(R,f'UC_MIPS_REG_S{i}') for i in range(8)]+[R.UC_MIPS_REG_FP]
        def sx(x,n): return x-(1<<n) if x&(1<<(n-1)) else x
        for label,body in (('retail',retail),('candidate',code)):
            m=Uc(UC_ARCH_MIPS,UC_MODE_MIPS32|UC_MODE_LITTLE_ENDIAN);m.mem_map(0,0x200000)
            def put(a,b): m.mem_write(a&0x1FFFFFFF,bytes(b))
            def read(a,n): return bytes(m.mem_read(a&0x1FFFFFFF,n))
            def get(a,n=1): return int.from_bytes(read(a,n),'little')
            def set_(a,v,n=1): put(a,(v&((1<<(8*n))-1)).to_bytes(n,'little'))
            put(BASE,body); calladdr=syms['BattleCmd_UndoPending']
            put(calladdr,struct.pack('<II',0x03E00008,0))
            actual=[]
            def callback(g,s,events):
                events.append((g(INDEX),g(COUNT),g(PENDING),g(CLEAR)))
                if mutation&1:
                    s(INDEX,23);s(COUNT,128);s(PENDING,1)
                if mutation&2:
                    s(ACTOR,CORE+0x100,4);s(ACTION+0x50,0x4B,4)
                    s(SLOTS+22*8+6,319,2)
            def hook(uc,address,size,data):
                if address!=calladdr:return
                callback(get,set_,actual)
                for name in ('V0','V1','A0','A1','A2','A3','T0','T1','T2','T3','T4','T5','T6','T7','T8','T9'):
                    uc.reg_write(getattr(R,'UC_MIPS_REG_'+name),0xCCCCCCCC)
            m.hook_add(UC_HOOK_CODE,hook)
            for index,kind,word,generation,pending,mutation in cases:
                case=(label,index,kind,word,generation,pending,mutation)
                expected={a:0xA5 for start,size in regions for a in range(start,start+size)}
                def g(a,n=1):return int.from_bytes(bytes(expected[a+i] for i in range(n)),'little')
                def s(a,v,n=1):
                    for i,b in enumerate((v&((1<<(8*n))-1)).to_bytes(n,'little')):expected[a+i]=b
                for a,v in ((INDEX,index),(CLEAR,233),(PENDING,pending),(COUNT,generation),
                            (SAVED,(word&15) if mutation&1 else 255),(SUB,255 if mutation&2 else 3)):s(a,v)
                s(ACTOR,CORE,4);s(CORE+0x68,ACTION,4);s(CORE+0x168,ACTION+0x40,4)
                s(ACTION+0x10,word,4)
                for i in range(45):
                    s(SLOTS+i*8,0x80001000+i*16,4);s(SLOTS+i*8+4,i+9,2)
                    # Equal runs plus signed cleanup matches and mismatches.
                    turn=(i//5 if mutation&1 else sx(generation,8)) if i%3 else sx((generation+1)&255,8)
                    s(SLOTS+i*8+6,turn,2)
                s(SLOTS+(index-1)*8+4,kind,2)
                for start,size in regions:put(start,bytes(expected[a] for a in range(start,start+size)))
                events=[]
                def rewind():
                    s(INDEX,g(INDEX)-1)
                    while g(INDEX) and g(SLOTS+g(INDEX)*8+6,2)==g(SLOTS+(g(INDEX)-1)*8+6,2):
                        s(INDEX,g(INDEX)-1)
                def restore(old):
                    if g(PENDING):s(PENDING,0);s(INDEX,old)
                def actionword():return g(g(g(ACTOR,4)+0x68,4)+0x10,4)
                s(CLEAR,0)
                if kind==1:
                    equal=g(SAVED)==(actionword()&15)
                    if equal:s(COUNT,g(COUNT)+1)
                    rewind()
                    if equal:restore(index)
                    s(SAVED,actionword()&15)
                elif kind==2:
                    mode=actionword()&0xC0
                    if mode in (0x40,0xC0):
                        amount=g(SUB) if mode==0xC0 else ((actionword()&15)*3)//2
                        s(COUNT,g(COUNT)+1);s(INDEX,index-amount)
                    s(SAVED,actionword()&15)
                elif kind<407:
                    s(COUNT,g(COUNT)+1)
                    if not g(PENDING):callback(g,s,events)
                    old=g(INDEX);rewind();restore(old);s(SAVED,actionword()&15)
                target=sx(g(COUNT),8)
                for i in range(45):
                    p=SLOTS+i*8
                    if sx(g(p+6,2),16)==target:s(p,0,4);s(p+6,0,2);s(p+4,0,2)
                actual.clear()
                for name,value in (('SP',SP),('GP',GP),('RA',STOP)):
                    m.reg_write(getattr(R,'UC_MIPS_REG_'+name),value)
                for i,r in enumerate(saved):m.reg_write(r,0xABCD0000+i)
                m.emu_start(BASE,STOP,count=10000)
                assert actual==events,(case,actual,events)
                for start,size in regions:
                    want=bytes(expected[a] for a in range(start,start+size));got=read(start,size)
                    assert got==want,(case,hex(start),[(i,got[i],want[i]) for i in range(size) if got[i]!=want[i]][:12])
                assert m.reg_read(R.UC_MIPS_REG_PC)==STOP and m.reg_read(R.UC_MIPS_REG_SP)==SP and m.reg_read(R.UC_MIPS_REG_GP)==GP,case
                for i,r in enumerate(saved):assert m.reg_read(r)==0xABCD0000+i,case
