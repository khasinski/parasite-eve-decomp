"""Animation event transitions, callback reloads, exact retail bytes and ABI."""
from pathlib import Path
from importlib.util import find_spec
import itertools
import struct
import unittest

ROOT = Path(__file__).resolve().parents[2]


class TriggerAnimEventTests(unittest.TestCase):
    @unittest.skipUnless((ROOT/'assets/USA/main.exe').is_file() and
                         (ROOT/'build/USA/main.exe').is_file() and find_spec('unicorn'),
                         'images or unicorn unavailable')
    def test_model_and_exact_bytes(self):
        from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN, UC_HOOK_CODE
        from unicorn import mips_const as R
        base=0x8002FAF8
        images=[(ROOT/p).read_bytes() for p in ('assets/USA/main.exe','build/USA/main.exe')]
        def section(image,address,size):
            offset=address-0x8000F800
            return image[offset:offset+size]
        self.assertEqual(section(images[0],base,896),section(images[1],base,896))
        self.assertEqual(section(images[0],0x80010A88,64),section(images[1],0x80010A88,64))
        E,P,C,G=0x80100000,0x80100400,0x80101000,0x8009D1A0
        SET,ASSET=0x8001A680,0x8006DCE4
        scenarios=[
            (0x10000,0,3,0,0,0,5),
            (0x30000,0x20000,2,2,1,0,0x80),
            (0x10000,0x30000,3,2,1,4,0xff),
            (0xfffe0000,0x7fff0000,255,254,3,1,0x7f),
            (0x50000,0x50000,5,5,4,4,0),
        ]
        cases=list(itertools.product(range(18),(0,1,2,3,4,5,6,255),range(5),(0,1),range(5),
                                     (0,0x6000,0x40000000,0xc0e06000),(False,True)))
        # Each kind is independently exercised with and without a parent.
        kinds=(-128,0,1,2,4)
        cases=[case+(4,) for case in cases]
        cases += [case for case in itertools.product((7,9,15),range(6),(1,3),(0,1),range(5),
                                                    (0,0x6000),(False,True),(0,2,5))]
        saved=[getattr(R,f'UC_MIPS_REG_S{i}') for i in range(8)]+[R.UC_MIPS_REG_FP]
        def signed(v,bits):return v-(1<<bits) if v&(1<<(bits-1)) else v

        for candidate, image in enumerate(images):
            m=Uc(UC_ARCH_MIPS,UC_MODE_MIPS32|UC_MODE_LITTLE_ENDIAN);m.mem_map(0,0x200000)
            def put(a,v):m.mem_write(a&0x1fffffff,bytes(v))
            def read(a,n):return bytes(m.mem_read(a&0x1fffffff,n))
            def ag(a,n=4):return int.from_bytes(read(a,n),'little')
            def aset(a,v,n=4):put(a,(v&((1<<(8*n))-1)).to_bytes(n,'little'))
            put(base,section(image,base,896))
            put(0x80010A88,section(image,0x80010A88,64))
            for a in (SET,ASSET):put(a,struct.pack('<II',0x03e00008,0))
            actual=[]
            def environment(call,args,g,s,events):
                args=tuple(v&0xffffffff for v in args)
                events.append((call,args))
                if call==SET:
                    s(args[0]+0xe,args[1],1)
                    if mutation:
                        s(C+0x18,C+0x2c)
                        s(C,g(C)^0x6000)
                        s(C+5,0,1)
                        s(args[0]+0x268,-31,2);s(args[0]+0x26a,47,2);s(args[0]+0x26c,-63,2)
                elif mutation:
                    s(C+0x18,C+0x3c)
                return 0
            def hook(uc,address,size,data):
                if address not in (SET,ASSET):return
                count=2 if address==SET else 5
                args=tuple(uc.reg_read(getattr(R,f'UC_MIPS_REG_A{i}')) if i<4
                           else ag(uc.reg_read(R.UC_MIPS_REG_SP)+0x10) for i in range(count))
                environment(address,args,ag,aset,actual)
                for name in ('V0','V1','A0','A1','A2','A3','T0','T1','T2','T3','T4','T5','T6','T7','T8','T9'):
                    uc.reg_write(getattr(R,'UC_MIPS_REG_'+name),0xcccccccc)
            m.hook_add(UC_HOOK_CODE,hook)
            verified=0
            for mode,slot,kind_index,parent,scenario,flags,mutation,active_index in cases:
                kind=kinds[kind_index]
                regions=[(E,bytearray(b'\xa5'*0x280)),(P,bytearray(b'\xa5'*0x280)),
                         (C,bytearray(b'\x00'*0xc0)),(G,bytearray(4))]
                def region(a,n):
                    for start,data in regions:
                        if start<=a and a+n<=start+len(data):return data,a-start
                    raise AssertionError(hex(a))
                def g(a,n=4):
                    data,i=region(a,n);return int.from_bytes(data[i:i+n],'little')
                def s(a,v,n=4):
                    data,i=region(a,n);data[i:i+n]=(v&((1<<(8*n))-1)).to_bytes(n,'little')
                frame,previous,last,event_frame,active_state,requested_state,base_mode=scenarios[scenario]
                s(E,C);s(E+0x18c,P if parent else 0);s(G,2)
                s(C,flags);s(C+5,kind,1);s(C+6,base_mode,1);s(C+0xb2,0x8123,2)
                s(C+0x18,C+0x1c+16*active_index)
                for i in range(6):
                    rec=C+0x1c+16*i
                    s(rec,requested_state,1);s(rec+2,6+i,1);s(rec+3,9+i,1)
                    s(rec+4,0x22000+i);s(rec+8,0xffff1234+i)
                    s(rec+0xe,(1,2,3)[i%3],1);s(rec+0xf,event_frame,1)
                s(C+0x1c+16*active_index,active_state,1)
                for actor in (E,P):
                    s(actor+0xe,mode,1);s(actor+0xf,last,1)
                    s(actor+0x14,frame);s(actor+0x18,previous);s(actor+0x1c,0x23456)
                    s(actor+0x268,-123,2);s(actor+0x26a,321,2);s(actor+0x26c,-456,2)
                # Include early exits without multiplying the already broad matrix.
                if mode==17 and slot in (6,255):
                    if slot==6:s(E,0)
                    else:s(G,0)
                before=[(a,bytes(data)) for a,data in regions]
                expected=[]
                def call(address,*args):return environment(address,args,g,s,expected)
                def model():
                    core=g(E)
                    result=0
                    if not core or not(g(G)&2):return 1
                    actor=E
                    if signed(g(core+5,1),8) in (2,4) and g(E+0x18c):actor=g(E+0x18c)
                    if slot>=6:return 0
                    current=g(actor+0x14);prev=g(actor+0x18)
                    end=(current>>16)+(g(actor+0xf,1)+1 if current<prev else 0)
                    action=g(actor+0xe,1)
                    if action in (6,8,10,12,14):
                        if g(actor+0xf,1)>=g(actor+0x1a,2) and g(actor+0xf,1)<end:
                            call(SET,actor,g(g(core+0x18)+3,1))
                            call(ASSET,g(core+0xb2,2),0,signed(g(actor+0x268,2),16),
                                 signed(g(actor+0x26a,2),16),signed(g(actor+0x26c,2),16))
                            s(actor+0x1c,g(g(core+0x18)+8));s(g(core+0x18),1,1)
                    elif action in (7,9,11,13,15):
                        if g(actor+0xf,1)>=g(actor+0x1a,2) and g(actor+0xf,1)<end and not(g(core)&0x40000000):
                            call(SET,actor,signed(g(core+6,1),8)&0xffff)
                            s(actor+0x1c,0x10000)
                            if slot<3:result=1;s(g(core+0x18),4,1)
                        if signed(g(core+5,1),8)==0 and g(core)&0x6000:
                            if g(g(core+0x18),1)<2:s(g(core+0x18),4,1)
                            result=2
                        if slot<3 and g(g(core+0x18)+0xe,1) in (1,3):
                            f=g(g(core+0x18)+0xf,1)
                            if f<g(actor+0x16,2) and f>=g(actor+0x1a,2):s(g(core+0x18),3,1)
                    elif action not in (0,1):
                        rec=core+0x1c+slot*16
                        if g(rec,1)==0:
                            s(core+0x18,rec)
                            s(core,((g(core)&0xff1fffff)|((slot&7)<<21))&0x3fffffff)
                            call(SET,actor,g(g(core+0x18)+2,1))
                            s(actor+0x1c,g(g(core+0x18)+4))
                    if g(g(core+0x18),1)==4 or g(core+0x1c+slot*16,1)==4:
                        call(SET,actor,signed(g(core+6,1),8)&0xffff)
                        s(actor+0x1c,0x10000);result=1
                        s(g(core+0x18),0,1);s(core+0x18,0)
                    return result
                result=model()
                for a,data in before:put(a,data)
                actual.clear()
                for name,value in (('A0',E),('A1',slot),('SP',0x801f0000),('RA',0x80010000),('GP',0x8009cd70)):
                    m.reg_write(getattr(R,'UC_MIPS_REG_'+name),value)
                for i,r in enumerate(saved):m.reg_write(r,0xabcd0000+i)
                m.emu_start(base,0x80010000,count=5000)
                case=(candidate,mode,slot,kind,parent,scenario,flags,mutation,active_index)
                assert actual==expected,(case,actual,expected)
                for a,data in regions:assert read(a,len(data))==data,(case,hex(a))
                assert m.reg_read(R.UC_MIPS_REG_V0)==result,case
                assert m.reg_read(R.UC_MIPS_REG_PC)==0x80010000 and m.reg_read(R.UC_MIPS_REG_SP)==0x801f0000,case
                assert m.reg_read(R.UC_MIPS_REG_GP)==0x8009cd70,case
                for i,r in enumerate(saved):assert m.reg_read(r)==0xabcd0000+i,case
                verified+=1
            self.assertEqual(verified,61920)


if __name__ == "__main__":
    unittest.main()
