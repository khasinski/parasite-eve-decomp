"""Bounded wall response model and retail bytes; normalization uses a controlled callback."""
from pathlib import Path
from importlib.util import find_spec
import itertools
import math
import struct
import unittest

ROOT=Path(__file__).resolve().parents[2]


class SlideOnWallTests(unittest.TestCase):
    @unittest.skipUnless((ROOT/'assets/USA/main.exe').is_file() and
                         (ROOT/'build/USA/main.exe').is_file() and find_spec('unicorn'),
                         'images or unicorn unavailable')
    def test_model_and_exact_bytes(self):
        from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN, UC_HOOK_CODE
        from unicorn import mips_const as R
        BASE,NORM,SQRT,MUL=0x8001CBA0,0x80078134,0x80078004,0x8003708C
        ENTITY,VERTICES,STOP,LIMIT=0x80100010,0x80101010,0x80010000,0x8009CE2C
        def signed(n):
            n &= 0xffffffff
            return n if n < 0x80000000 else n-0x100000000
        def fixed(a,b):return signed((signed(a)*signed(b))>>16)
        def div(a,b):
            assert b and not(a == -0x80000000 and b == -1)
            q=abs(a)//abs(b)
            return -q if (a<0)!=(b<0) else q
        images=[(ROOT/p).read_bytes() for p in ('assets/USA/main.exe','build/USA/main.exe')]
        retail,candidate=[image[BASE-0x8000f800:BASE-0x8000f800+744] for image in images]
        self.assertEqual(retail,candidate)
        polygons=(((0,0),(8,0),(8,8)),((-8,-8),(8,8),(-8,8)),((5,-7),(-5,7),(12,10)))
        units=((4096,0),(0,4096),(2896,2896),(-4096,0),(-2896,-2896))
        positions=((0,0),(3<<16,-2<<16),(-7<<16,9<<16),(0x30001,-0x20001))
        cases=list(itertools.product(polygons,range(3),units,positions,(0,2,5),(False,True)))
        cases += list(itertools.product(polygons[:1],range(3),units[:2],positions[:1],
                                       (255,256,257,511),(False,)))
        saved=[getattr(R,f'UC_MIPS_REG_S{i}') for i in range(8)]+[R.UC_MIPS_REG_FP]
        for label,code in (('retail',retail),('candidate',candidate)):
            m=Uc(UC_ARCH_MIPS,UC_MODE_MIPS32|UC_MODE_LITTLE_ENDIAN);m.mem_map(0,0x200000)
            def put(a,b):m.mem_write(a&0x1fffffff,bytes(b))
            def read(a,n):return bytes(m.mem_read(a&0x1fffffff,n))
            def word(a,v):put(a,struct.pack('<I',v&0xffffffff))
            put(BASE,code)
            for a in (NORM,SQRT,MUL):put(a,struct.pack('<II',0x03e00008,0))
            actual=[]
            def hook(uc,address,size,data):
                if address not in (NORM,SQRT,MUL):return
                a=uc.reg_read(R.UC_MIPS_REG_A0);b=uc.reg_read(R.UC_MIPS_REG_A1)
                if address==NORM:
                    actual.append(('norm',struct.unpack('<iii',read(a,12))))
                    put(b,struct.pack('<iii',unit[0],0,unit[1]))
                    if mutate:
                        word(ENTITY+0x40,old[0]);word(ENTITY+0x48,old[1])
                    value=0
                elif address==MUL:
                    actual.append(('mul',signed(a),signed(b)))
                    value=fixed(a,b)
                else:
                    actual.append(('sqrt',a))
                    value=math.isqrt(a)
                for name in ('V1','A0','A1','A2','A3','T0','T1','T2','T3','T4','T5','T6','T7','T8','T9'):
                    uc.reg_write(getattr(R,'UC_MIPS_REG_'+name),0xcccccccc)
                uc.reg_write(R.UC_MIPS_REG_V0,value&0xffffffff)
            m.hook_add(UC_HOOK_CODE,hook)
            branches=set()
            for polygon,edge,unit,pos,limit,mutate in cases:
                old=(-2<<16,3<<16) if mutate else (-1<<16,2<<16)
                end=polygon[edge];start=polygon[edge-1]
                delta=(end[0]-start[0],end[1]-start[1])
                expected=[('norm',(delta[0],0,delta[1]))]
                def mul(a,b):
                    expected.append(('mul',signed(a),signed(b)))
                    return fixed(a,b)
                direction=(signed(unit[0]<<4),signed(unit[1]<<4))
                projection=signed(mul(direction[0],pos[0]-old[0])+mul(direction[1],pos[1]-old[1]))
                x=signed(mul(direction[0],projection)+old[0])
                z=signed(mul(direction[1],projection)+old[1])
                square=delta[0]*delta[0]+delta[1]*delta[1]
                expected.append(('sqrt',square));length=math.isqrt(square)
                def outside(x,z):
                    cross=signed(((z>>16)-end[1])*(-delta[0])-((x>>16)-end[0])*(-delta[1]))
                    d=div(cross,length)
                    if d<0:d=signed(-d)
                    return d>limit
                result=(x,z)
                if outside(x,z):branches.add('direct')
                else:
                    for radius in range(1024):
                        found=False
                        for side,(dx,dz) in enumerate(((radius,0),(-radius,0),(0,radius),(0,-radius))):
                            trial=(signed(x+(dx<<16)),signed(z+(dz<<16)))
                            if outside(*trial):
                                result=trial;branches.add(side);found=True;break
                        if found:break
                    else:raise AssertionError('model search bound exceeded')
                entity=bytearray(b'\xa5'*0x290)
                struct.pack_into('<i',entity,0x10+0x28,pos[0]);struct.pack_into('<i',entity,0x10+0x30,pos[1])
                struct.pack_into('<i',entity,0x10+0x40,-1<<16);struct.pack_into('<i',entity,0x10+0x48,2<<16)
                put(ENTITY-0x10,entity)
                vertices=b''.join(struct.pack('<HhHh',0x1234,vx,0x5678,vz) for vx,vz in polygon)
                put(VERTICES,vertices);put(LIMIT,struct.pack('<H',limit))
                actual.clear()
                for name,value in (('A0',ENTITY),('A1',VERTICES),('A2',3),('A3',edge),('SP',0x801f0000),('RA',STOP),('GP',0x8009cd70)):
                    m.reg_write(getattr(R,'UC_MIPS_REG_'+name),value)
                for i,r in enumerate(saved):m.reg_write(r,0xabcd0000+i)
                m.emu_start(BASE,STOP,count=500000)
                case=(label,polygon,edge,unit,pos,limit,mutate)
                struct.pack_into('<i',entity,0x10+0x28,result[0]);struct.pack_into('<i',entity,0x10+0x30,result[1])
                struct.pack_into('<i',entity,0x10+0x40,old[0]);struct.pack_into('<i',entity,0x10+0x48,old[1])
                assert read(ENTITY-0x10,len(entity))==entity,case
                assert read(VERTICES,len(vertices))==vertices,case
                assert read(LIMIT,2)==struct.pack('<H',limit),case
                assert actual==expected,(case,actual,expected)
                assert m.reg_read(R.UC_MIPS_REG_PC)==STOP and m.reg_read(R.UC_MIPS_REG_SP)==0x801f0000,case
                assert m.reg_read(R.UC_MIPS_REG_GP)==0x8009cd70,case
                for i,r in enumerate(saved):assert m.reg_read(r)==0xabcd0000+i,case
            self.assertEqual(len(cases),1104)
            self.assertEqual(branches,{'direct',0,1,2,3})


if __name__ == "__main__":
    unittest.main()
