"""Primitive setup against retail and a bounded packet-mutation model.

Checks valid disjoint object/header/buffer layouts, coordinate narrowing,
UV and palette wrapping, mixed double-buffered packet types and early return.
No physical GPU timing or arbitrary aliasing claim.
"""
from pathlib import Path
import itertools
import random
import struct
import unittest

ROOT = Path(__file__).resolve().parents[2]


class RenderInitPrimBlockTests(unittest.TestCase):
    def test_plain_source(self):
        source = (ROOT/'src/main/main/Render_InitPrimBlock.c').read_text()
        self.assertNotRegex(source, r'\b(?:asm|__asm__|INCLUDE_ASM|REGALLOC_BARRIER)\b')

    @unittest.skipUnless((ROOT/'assets/USA/main.exe').is_file() and
                         (ROOT/'build/USA/main.exe').is_file(), 'images unavailable')
    def test_packets(self):
        try:
            from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN
            from unicorn import mips_const as R
        except ImportError:
            self.skipTest('unicorn unavailable')
        base = 0x8003D94C
        offset = base - 0x8000F800
        retail, candidate = [(ROOT/p).read_bytes()[offset:offset+0x298]
                             for p in ('assets/USA/main.exe', 'build/USA/main.exe')]
        self.assertEqual(len(candidate), 0x298)
        self.assertEqual(candidate, retail)
        saved=[getattr(R,f'UC_MIPS_REG_S{i}') for i in range(8)]+[R.UC_MIPS_REG_GP,R.UC_MIPS_REG_FP]
        obj,header,packets,stack,stop=0x80100020,0x80101020,0x80102020,0x801F0000,0x80010000
        initial=random.Random(base).randbytes(0x300)
        cases=list(itertools.product((0,512,960,1023,-64,-32768,32767,0x103C0),(0,128,256,-128,-32768,32767,0x10080),(0,1,511,0xFFFFFFFF),((0,0),(1,0),(0,1),(2,2)),(0,31,0xFFFF)))
        def s16(v):return ((v+32768)&65535)-32768
        for body in (retail,candidate):
            m=Uc(UC_ARCH_MIPS,UC_MODE_MIPS32|UC_MODE_LITTLE_ENDIAN);m.mem_map(0,0x200000)
            def put(a,v):m.mem_write(a&0x1FFFFFFF,bytes(v))
            def read(a,n):return bytes(m.mem_read(a&0x1FFFFFFF,n))
            put(base,body)
            for x,y,row,(quads,tris),first_page in cases:
                case=(x,y,row,quads,tris,first_page)
                x_value,y_value=s16(x),s16(y)
                object_data=bytearray(b'\xA5'*0xA0);struct.pack_into('<I',object_data,0x20,header);struct.pack_into('<I',object_data,0x74,packets)
                header_data=bytearray(b'\x5A'*0x60);struct.pack_into('<HH',header_data,0x28,quads,tris)
                packet_data=bytearray(initial);struct.pack_into('<H',packet_data,0x20+0x1A,first_page)
                expected=bytearray(packet_data)
                if x_value!=960 or first_page!=31:
                    page=(((x_value>>6)-8)*2+(y_value>>7)+(y_value>>8)*30)&0xFFFFFFFF
                    page_offset=s16(page)>>1 if x_value==960 else page>>1
                    clut_offset=((row<<6)-0x7080)&0xFFFFFFFF
                    cursor=0x20
                    for count,size,uv in ((quads,0x34,(13,25,37,49)),(tris,0x28,(13,25,37))):
                        for _ in range(count*2):
                            carry=0
                            if y_value==128:
                                carry=int(expected[cursor+13]>=128)
                                for v in uv:expected[cursor+v]=(expected[cursor+v]+128)&255
                            old=struct.unpack_from('<H',expected,cursor+26)[0]
                            struct.pack_into('<H',expected,cursor+26,(old+page_offset+carry)&65535)
                            old=struct.unpack_from('<H',expected,cursor+14)[0]
                            struct.pack_into('<H',expected,cursor+14,(old+clut_offset)&65535)
                            cursor+=size
                put(obj-0x20,object_data);put(header-0x20,header_data);put(packets-0x20,packet_data);put(stack+16,struct.pack('<I',row))
                for name,value in (('A0',obj),('A1',x&0xFFFFFFFF),('A2',y&0xFFFFFFFF),('A3',123),('SP',stack),('RA',stop)):
                    m.reg_write(getattr(R,'UC_MIPS_REG_'+name),value)
                for i,r in enumerate(saved):m.reg_write(r,0xABCD0000+i)
                m.emu_start(base,stop,count=2000)
                assert read(packets-0x20,len(expected))==expected,case
                assert read(obj-0x20,len(object_data))==object_data and read(header-0x20,len(header_data))==header_data,case
                assert m.reg_read(R.UC_MIPS_REG_PC)==stop and m.reg_read(R.UC_MIPS_REG_SP)==stack,case
                for i,r in enumerate(saved):assert m.reg_read(r)==0xABCD0000+i,case
        print('verified',len(cases),'cases per image')


if __name__ == '__main__':
    unittest.main()
