"""Independent ramp model; finite nondegenerate inputs, no division traps."""
from pathlib import Path
import itertools, struct, unittest
from importlib.util import find_spec
ROOT = Path(__file__).resolve().parents[2]
BASE, MUL = 0x8001C7DC, 0x8003708C
ENTITY, EDGES, STOP = 0x80100010, 0x80101010, 0x80010000
def signed(n):
    n &= 0xffffffff
    return n if n < 0x80000000 else n - 0x100000000
def fixed(a,b): return signed((signed(a)*signed(b)) >> 16)
def div(a,b):
    assert b and not (a == -0x80000000 and b == -1)
    q=abs(a)//abs(b)
    return -q if (a<0) != (b<0) else q
def model(old, pos, vector, edge, length, threshold):
    calls=[]
    def mul(a,b):
        calls.append((signed(a),signed(b)))
        return fixed(a,b)
    projection = mul(vector[0],signed(pos[0]-old[0]))
    projection = signed(projection+mul(vector[1],signed(pos[1]-old[1])))
    x=mul(vector[0],projection); z=mul(vector[1],projection)
    x=signed(x+old[0]); z=signed(z+old[1])
    x0,z0,x1,z1=edge
    def outside(x,z):
        cross=signed(((z>>16)-z0)*(x1-x0)-((x>>16)-x0)*(z1-z0))
        distance=div(cross,length)
        if distance<0: distance=signed(-distance)
        return distance>threshold
    if outside(x,z): return (x,z),calls,'direct'
    for radius in range(1024):
        for direction,(dx,dz) in enumerate(((radius,0),(-radius,0),(0,radius),(0,-radius))):
            trial=(signed(x+(dx<<16)),signed(z+(dz<<16)))
            if outside(*trial): return trial,calls,direction
    raise AssertionError('test case did not terminate')


class SlideOnRampTests(unittest.TestCase):
    @unittest.skipUnless((ROOT/'assets/USA/main.exe').is_file() and
                         (ROOT/'build/USA/main.exe').is_file() and
                         find_spec('unicorn'), 'images or unicorn unavailable')
    def test_behavior_and_exact_bytes(self):
        from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN, UC_HOOK_CODE
        from unicorn import mips_const as R
        image=(ROOT/'assets/USA/main.exe').read_bytes()
        def retail(a,n):return image[a-0x8000F800:a-0x8000F800+n]
        code=(ROOT/'build/USA/main.exe').read_bytes()[BASE-0x8000F800:BASE-0x8000F800+724]
        self.assertEqual(code,retail(BASE,724))
        vectors=((65536,0),(0,65536),(46341,46341),(-46341,46341),(0,0))
        edges=((-8,-8,8,-8),(-8,-8,-8,8),(-8,-8,8,8),(8,-8,-8,8),(-32768,32767,32767,-32768),(0,0,16,0))
        positions=((0,0),(3<<16,-2<<16),(-7<<16,9<<16),(0x7fffffff,-0x80000000))
        cases=list(itertools.product(vectors,edges,positions,(1,16,-16),(0,2,9),(0,2)))
        branches=set()
        saved=[getattr(R,'UC_MIPS_REG_S'+str(i)) for i in range(8)]+[R.UC_MIPS_REG_FP]
        for label,body in (('retail',retail(BASE,724)),('candidate',code)):
            m=Uc(UC_ARCH_MIPS,UC_MODE_MIPS32|UC_MODE_LITTLE_ENDIAN);m.mem_map(0,0x200000)
            def put(a,b):m.mem_write(a&0x1fffffff,bytes(b))
            def read(a,n):return bytes(m.mem_read(a&0x1fffffff,n))
            put(BASE,body);put(MUL,retail(MUL,28))
            calls=[]
            def hook(m,a,size,data):
                calls.append((signed(m.reg_read(R.UC_MIPS_REG_A0)),signed(m.reg_read(R.UC_MIPS_REG_A1))))
            m.hook_add(UC_HOOK_CODE,hook,begin=MUL,end=MUL)
            for vector,edge,pos,length,threshold,index in cases:
                old=(-65536,131072)
                result,expected_calls,branch=model(old,pos,vector,edge,length,threshold)
                branches.add(branch)
                entity=bytearray(b'\xa5'*0x28C)
                struct.pack_into('<i',entity,8+0x28,pos[0]);struct.pack_into('<i',entity,8+0x30,pos[1])
                struct.pack_into('<i',entity,8+0x40,old[0]);struct.pack_into('<i',entity,8+0x48,old[1])
                put(ENTITY-8,entity)
                table=bytearray(b'\x5a'*52)
                struct.pack_into('<hhii',table,8+12*index,0,length,*vector);put(EDGES-8,table)
                globals=bytearray(b'\x6c'*48)
                struct.pack_into('<hhhhIH',globals,4,*edge,EDGES,index)
                struct.pack_into('<H',globals,36,threshold);put(0x8009CE08,globals)
                flags=0xa5020040;put(0x8009D2E8,struct.pack('<I',flags))
                calls.clear()
                for name,value in (('A0',ENTITY),('SP',0x801f0000),('GP',0x8009CD70),('RA',STOP)):
                    m.reg_write(getattr(R,'UC_MIPS_REG_'+name),value)
                for i,r in enumerate(saved):m.reg_write(r,0xabcd0000+i)
                m.emu_start(BASE,STOP,count=200000)
                struct.pack_into('<i',entity,8+0x28,result[0]);struct.pack_into('<i',entity,8+0x30,result[1])
                case=(vector,edge,pos,length,threshold,index)
                assert read(ENTITY-8,len(entity))==entity,(label,case,result)
                assert read(EDGES-8,len(table))==table and read(0x8009CE08,48)==globals,(label,case)
                assert read(0x8009D2E8,4)==struct.pack('<I',flags|8)
                assert calls==expected_calls,(label,case,calls,expected_calls)
                assert m.reg_read(R.UC_MIPS_REG_PC)==STOP and m.reg_read(R.UC_MIPS_REG_SP)==0x801f0000
                for i,r in enumerate(saved):assert m.reg_read(r)==0xabcd0000+i
            self.assertEqual(branches, {'direct',0,1,2,3})

if __name__ == '__main__':
    unittest.main()
