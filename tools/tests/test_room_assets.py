"""Process-slot initialization and CD retry protocol against an independent model."""
from pathlib import Path
from importlib.util import find_spec
import itertools
import struct
import unittest

ROOT=Path(__file__).resolve().parents[2]
BASE,SIZE=0x8006F39C,824
READ,POLL,ENTER,FLUSH,EXIT=0x8006E6A8,0x8006E7E8,0x80072714,0x800726C4,0x80072724
TEXTURES,SPECIAL=0x8006914C,0x800CE49C
INIT_A,INIT_B=0x80010100,0x80010200
TABLE_A,TABLE_B,DESC_A,DESC_B=0x80100100,0x80100500,0x80100900,0x80100910
BANK_A,BANK_B=0x80101000,0x80110000
TARGETS=(0x801F1BD8,0x801F1C58,0x801F1D00,0x801F1D8C,0x801F1E18,0x801F1EA4,0x801F1EF0)


class RoomAssetsTests(unittest.TestCase):
    @unittest.skipUnless((ROOT/'assets/USA/main.exe').is_file() and
                         (ROOT/'build/USA/main.exe').is_file() and
                         find_spec('unicorn'), 'images or unicorn unavailable')
    def test_model_and_exact_bytes(self):
        from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN, UC_HOOK_CODE
        from unicorn import mips_const as R
        start=BASE-0x8000F800
        retail,code=[(ROOT/p).read_bytes()[start:start+SIZE]
                     for p in ('assets/USA/main.exe','build/USA/main.exe')]
        self.assertEqual(code,retail);self.assertEqual(len(code),SIZE)
        commands=(0,69,70,84,85,108,114,115,191,192,0xffffffff)
        cases=list(itertools.product(commands,(0,5,10,11),(0,1,2),(0,1),(0,1),(0,1)))
        ranges=((0x80011610,16),(0x80093160,8),(0x800942D8,24),
                (0x800B0CD0,16),(0x800B0DD0,16),(0x800E1098,44),
                (TABLE_A-8,0x830),(BANK_A-8,0x7a18),(BANK_B-8,0x7a18))
        saved=[getattr(R,'UC_MIPS_REG_S'+str(i)) for i in range(8)]+[R.UC_MIPS_REG_FP]
        for label,body in (('retail',retail),('candidate',code)):
            m=Uc(UC_ARCH_MIPS,UC_MODE_MIPS32|UC_MODE_LITTLE_ENDIAN);m.mem_map(0,0x200000)
            def put(a,b):m.mem_write(a&0x1fffffff,bytes(b))
            def read(a,n):return bytes(m.mem_read(a&0x1fffffff,n))
            def ag(a,n=4):return int.from_bytes(read(a,n),'little')
            def aset(a,v,n=4):put(a,(v&((1<<(8*n))-1)).to_bytes(n,'little'))
            put(BASE,body)
            calls=(READ,POLL,ENTER,FLUSH,EXIT,TEXTURES,SPECIAL,INIT_A,INIT_B)
            for a in calls:put(a,struct.pack('<II',0x03e00008,0))
            actual_events=[];actual_context={}
            def environment(call,args,g,s,events,context):
                event=(call,args)
                if call in (SPECIAL,INIT_A,INIT_B):
                    event+=(tuple(g(args[0]+i,1) for i in range(12)),)
                events.append(event)
                if call==READ:
                    i=context.get('read',0);context['read']=i+1
                    result=-1 if retries and i==0 else 0
                    if mutation:
                        s(0x800B0DD8,g(0x800B0DD8)+1)
                        s(0x80093164,g(0x80093164,2)+1,2)
                    return result
                if call==POLL:
                    i=context.get('poll',0);context['poll']=i+1
                    return (2,-1,2,0)[min(i,3)] if retries else 0
                if mutation and call==TEXTURES:
                    s(0x800942E4,BANK_B);s(0x800942E8,BANK_B+0x6e84)
                if mutation and call==SPECIAL:s(0x800942E0,TABLE_B)
                if call in (INIT_A,INIT_B):s(args[0]+4,0x12345678)
                return 0x1234
            def hook(uc,a,size,data):
                if a not in calls:return
                n=3 if a==READ else 2 if a==SPECIAL else 1 if a in (TEXTURES,INIT_A,INIT_B) else 0
                args=tuple(uc.reg_read(getattr(R,'UC_MIPS_REG_A'+str(i))) for i in range(n))
                result=environment(a,args,ag,aset,actual_events,actual_context)
                for name in ('V0','V1','A0','A1','A2','A3','T0','T1','T2','T3','T4','T5','T6','T7','T8','T9'):
                    uc.reg_write(getattr(R,'UC_MIPS_REG_'+name),0xcccccccc)
                uc.reg_write(R.UC_MIPS_REG_V0,result&0xffffffff)
            m.hook_add(UC_HOOK_CODE,hook)
            for command,free,descriptor,loaded,retries,mutation in cases:
                regions=[(a,bytearray(b'\xa5'*n)) for a,n in ranges]
                def region(a,n):
                    for base,b in regions:
                        if base<=a and a+n<=base+len(b):return b,a-base
                    raise AssertionError(hex(a))
                def g(a,n=4):
                    b,i=region(a,n);return int.from_bytes(b[i:i+n],'little')
                def s(a,v,n=4):
                    b,i=region(a,n);b[i:i+n]=(v&((1<<(n*8))-1)).to_bytes(n,'little')
                s(0x80011618,0x80180000);s(0x80093162,8,2);s(0x80093164,12,2)
                s(0x800B0DD8,1200);s(0x800B0CD8,0x12340002|(loaded<<16))
                s(0x800942E0,TABLE_A);s(0x800942E4,BANK_A);s(0x800942E8,BANK_A+0x6e84)
                for table,desc,callback in ((TABLE_A,DESC_A,INIT_A),(TABLE_B,DESC_B,INIT_B)):
                    for i in range(86):s(table+i*4,desc if descriptor else 0)
                    s(desc+4,callback if descriptor==2 else 0)
                for bank in (BANK_A,BANK_B):
                    for base,stride in ((bank,0xa0c),(bank+0x6e84,0x10c)):
                        for i in range(11):s(base+i*stride,0 if i==free else 1,1)
                for a,b in regions:put(a,b)
                expected_events=[];context={};actual_events.clear();actual_context.clear()
                def invoke(call,*args):return environment(call,args,g,s,expected_events,context)
                owner=0x801abc00
                result=-7
                if command<192:
                    if 108<=command<115 and not(g(0x800B0CD8)&0x10000):
                        while True:
                            while invoke(READ,g(0x800B0DD8)+g(0x80093162,2),g(0x80011618),
                                         g(0x80093164,2)-g(0x80093162,2))==-1:pass
                            ready=invoke(POLL)
                            while ready not in (0,-1):ready=invoke(POLL)
                            if ready==0:break
                        for call in (ENTER,FLUSH,EXIT):invoke(call)
                        for i,target in enumerate(TARGETS):s(0x800E10A0+i*4,target)
                        s(0x800B0CD8,g(0x800B0CD8)|0x10000)
                    invoke(TEXTURES,0)
                    cmd=min(command,85);desc=g(g(0x800942E0)+cmd*4)
                    result=-8 if not desc else -1
                    if desc and g(desc+4):
                        secondary=70<=cmd<85
                        table=g(0x800942E8 if secondary else 0x800942E4)
                        stride=0x10c if secondary else 0xa0c
                        available=[i for i in range(11) if not g(table+i*stride,1)]
                        result=-3
                        if available:
                            i=available[0];entry=table+i*stride;result=i+(11 if secondary else 0)
                            for off,value in ((0,1),(1,command),(2,0),(3,0)):s(entry+off,value,1)
                            s(entry+4,0);s(entry+8,owner)
                            if cmd==85:invoke(SPECIAL,entry,command-85)
                            invoke(g(g(g(0x800942E0)+cmd*4)+4),entry)
                for name,value in (('A0',command),('A1',owner),('SP',0x801f0000),('GP',0x8009CD70),('RA',0x80010000)):
                    m.reg_write(getattr(R,'UC_MIPS_REG_'+name),value)
                for i,r in enumerate(saved):m.reg_write(r,0xabcd0000+i)
                m.emu_start(BASE,0x80010000,count=20000)
                case=(label,command,free,descriptor,loaded,retries,mutation)
                self.assertEqual(m.reg_read(R.UC_MIPS_REG_V0),result&0xffffffff,case)
                self.assertEqual(actual_events,expected_events,case)
                for a,b in regions:self.assertEqual(read(a,len(b)),b,case)
                self.assertEqual(m.reg_read(R.UC_MIPS_REG_PC),0x80010000,case)
                self.assertEqual(m.reg_read(R.UC_MIPS_REG_SP),0x801f0000,case)
                for i,r in enumerate(saved):self.assertEqual(m.reg_read(r),0xabcd0000+i,case)


if __name__=='__main__':unittest.main()
