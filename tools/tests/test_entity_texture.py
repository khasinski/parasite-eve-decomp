"""Entity-bank loading and texture/record iteration against a separate model."""
from pathlib import Path
from importlib.util import find_spec
import itertools
import struct
import unittest

ROOT=Path(__file__).resolve().parents[2]
BASE,SIZE,TABLE=0x8006BECC,768,0x800113B0
STATE,SECTORS,READY=0x800B0CD8,0x800930D8,0x8009D25C
TEXTURES,BANK=0x80100000,0x80101000
READ,POLL,TIM=0x8006E6A8,0x8006E7E8,0x8006E1C0


class EntityTextureTests(unittest.TestCase):
    @unittest.skipUnless((ROOT/'assets/USA/main.exe').is_file() and
                         (ROOT/'build/USA/main.exe').is_file() and find_spec('unicorn'),
                         'images or unicorn unavailable')
    def test_model_and_exact_bytes(self):
        from unicorn import Uc,UC_ARCH_MIPS,UC_MODE_MIPS32,UC_MODE_LITTLE_ENDIAN,UC_HOOK_CODE
        from unicorn import mips_const as R
        images=[(ROOT/p).read_bytes() for p in ('assets/USA/main.exe','build/USA/main.exe')]
        def section(image,a,n): return image[a-0x8000F800:a-0x8000F800+n]
        self.assertEqual(section(images[0],BASE,SIZE),section(images[1],BASE,SIZE))
        self.assertEqual(section(images[0],TABLE,28),section(images[1],TABLE,28))
        cases=list(itertools.product(range(9),(0,0x200000,0x220000,0x2A0000),
                                     (0,5,255),(0,1,3),(-1,0,2),(0,2),(0,1)))
        ranges=((STATE-8,0x96C),(SECTORS-8,0x230),(READY-8,20),
                (TEXTURES,0x500),(BANK,0x500))
        saved=[getattr(R,'UC_MIPS_REG_S'+str(i)) for i in range(8)]+[R.UC_MIPS_REG_FP]
        for label,image in zip(('retail','candidate'),images):
            m=Uc(UC_ARCH_MIPS,UC_MODE_MIPS32|UC_MODE_LITTLE_ENDIAN);m.mem_map(0,0x200000)
            def put(a,b):m.mem_write(a&0x1fffffff,bytes(b))
            def read(a,n):return bytes(m.mem_read(a&0x1fffffff,n))
            def ag(a,n=4):return int.from_bytes(read(a,n),'little')
            def aset(a,v,n=4):put(a,(v&((1<<(n*8))-1)).to_bytes(n,'little'))
            put(BASE,section(image,BASE,SIZE));put(TABLE,section(image,TABLE,28))
            for a in (READ,POLL,TIM):put(a,struct.pack('<II',0x03e00008,0))
            actual_events=[]
            def environment(call,args,g,s,events):
                events.append((call,args))
                if mutation:
                    if call==TIM:
                        s(TEXTURES+0x68,(1<<22)|0x100)
                        s(STATE+0xA,7,1);s(STATE+0x100,9000)
                    if call==POLL:s(STATE,g(STATE)^0x80000)
                return status if call in (READ,POLL) else 0
            def hook(uc,a,size,data):
                if a not in (READ,POLL,TIM):return
                self.assertEqual(uc.reg_read(R.UC_MIPS_REG_SP),0x801effb0)
                n=3 if a==READ else 2 if a==TIM else 0
                args=tuple(uc.reg_read(getattr(R,'UC_MIPS_REG_A'+str(i))) for i in range(n))
                result=environment(a,args,ag,aset,actual_events)
                for name in ('V0','V1','A0','A1','A2','A3','T0','T1','T2','T3','T4','T5','T6','T7','T8','T9'):
                    uc.reg_write(getattr(R,'UC_MIPS_REG_'+name),0xcccccccc)
                uc.reg_write(R.UC_MIPS_REG_V0,result&0xffffffff)
            m.hook_add(UC_HOOK_CODE,hook)
            for phase,flags,bank,count,status,ready,mutation in cases:
                regions=[(a,bytearray(b'\xa5'*n)) for a,n in ranges]
                def region(a,n):
                    for base,b in regions:
                        if base<=a and a+n<=base+len(b):return b,a-base
                    raise AssertionError(hex(a))
                def g(a,n=4):
                    b,i=region(a,n);return int.from_bytes(b[i:i+n],'little')
                def s(a,v,n=4):
                    b,i=region(a,n);b[i:i+n]=(v&((1<<(8*n))-1)).to_bytes(n,'little')
                s(STATE,flags|0x123);s(STATE+0xA,bank,1);s(STATE+0xEC,phase,1)
                s(STATE+0x100,1200);s(STATE+0x194,TEXTURES);s(STATE+0x154,BANK)
                s(READY,ready,1)
                for i in range(270):s(SECTORS+2*i,100+3*i,2)
                for base in (TEXTURES,BANK):s(base+4,0x40)
                s(TEXTURES+0x68,(count<<22)|0x100)
                s(BANK+0x4C,0xEFC00180);s(BANK+0x50,(count<<22)|0x1A0)
                s(BANK+0x184,0xFA000380)
                for i,ident in enumerate((0,255,0)):
                    s(BANK+0x1A4+12*i,(ident<<24)|(0x300+0x20*i))
                for a,b in regions:put(a,b)
                expected_events=[];actual_events.clear()
                def invoke(call,*args):
                    return environment(call,tuple(v&0xffffffff for v in args),g,s,expected_events)
                result=0
                for step in range(10):
                    current=g(STATE+0xEC,1)
                    if current==0:
                        s(STATE+0xEC,1 if g(STATE)&0x200000 else 6,1)
                    elif current in (1,4):
                        index=bank+(3 if current==1 else 8)
                        first,last=g(SECTORS+index*2,2),g(SECTORS+(index+1)*2,2)
                        address=g(STATE+(0x194 if current==1 else 0x154))
                        if invoke(READ,1200+first,address,last-first)!=-1:
                            s(STATE+0xEC,current+1,1)
                        result=1;break
                    elif current in (2,5):
                        response=invoke(POLL)
                        if response==-1:
                            s(STATE+0xEC,current-1,1);result=1;break
                        if response:
                            result=1;break
                        if current==2 and g(STATE)&0x20000:
                            if not(g(STATE)&0x80000) and g(READY,1)<2:
                                result=1;break
                            s(STATE,g(STATE)|0x40000)
                        s(STATE+0xEC,current+1,1)
                    elif current==3:
                        blob=g(STATE+0x194);directory=blob+g(blob+4)
                        entry=blob+(g(directory+0x28)&0x3fffff)
                        i=0
                        while i<(g(directory+0x28)>>22):
                            invoke(TIM,entry+20*i,blob);i+=1
                        s(STATE+0xEC,4,1)
                    elif current==6:
                        blob=g(STATE+0x154);directory=blob+g(blob+4)
                        root=blob+(g(directory+0xC)&0x3fffff)
                        s(STATE+0x198,blob+(g(root+4)&0xffffff))
                        entry=blob+(g(directory+0x10)&0x3fffff)
                        for i in range(g(directory+0x10)>>22):
                            packed=g(entry+12*i+4)
                            s(STATE+0x1C0+4*(packed>>24),blob+(packed&0xffffff))
                        s(STATE+0xEC,0,1);s(STATE+0xB,g(STATE+0xA,1),1)
                        s(STATE,g(STATE)&~0x200000);break
                    else:break
                else:self.fail('model did not terminate')
                for name,value in (('SP',0x801f0000),('GP',0x8009CD70),('RA',0x80010000)):
                    m.reg_write(getattr(R,'UC_MIPS_REG_'+name),value)
                for i,reg in enumerate(saved):m.reg_write(reg,0xabcd0000+i)
                m.emu_start(BASE,0x80010000,count=20000)
                case=(label,phase,flags,bank,count,status,ready,mutation)
                self.assertEqual(m.reg_read(R.UC_MIPS_REG_V0),result,case)
                self.assertEqual(actual_events,expected_events,case)
                for a,b in regions:self.assertEqual(read(a,len(b)),b,case)
                self.assertEqual(m.reg_read(R.UC_MIPS_REG_PC),0x80010000,case)
                self.assertEqual(m.reg_read(R.UC_MIPS_REG_SP),0x801f0000,case)
                for i,reg in enumerate(saved):self.assertEqual(m.reg_read(reg),0xabcd0000+i,case)


if __name__=='__main__':unittest.main()
