"""Draw-area allocation: ancestry, rectangle encoding, DMA linkage and ABI.

Valid non-null acyclic nodes and aligned arenas with at least 16 bytes free.
The exhausted-arena path asserts and dereferences null in retail and is not a
portable-C contract. SetDrawArea is modeled at its call boundary, not as GPU
execution. Unicorn does not prove physical hardware load-delay behavior.
"""
from pathlib import Path
import itertools
import struct
import unittest

ROOT = Path(__file__).resolve().parents[2]


class DrawAllocPrimWithMaskTests(unittest.TestCase):
    def test_plain_source(self):
        source = (ROOT/'src/main/menu/Draw_AllocPrimWithMask.c').read_text()
        self.assertNotRegex(source, r'\b(?:asm|__asm__|INCLUDE_ASM|REGALLOC_BARRIER)\b')

    @unittest.skipUnless((ROOT/'assets/USA/main.exe').is_file() and
                         (ROOT/'build/USA/main.exe').is_file(), 'images unavailable')
    def test_packets(self):
        try:
            from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN, UC_HOOK_CODE
            from unicorn import mips_const as R
        except ImportError:
            self.skipTest('unicorn unavailable')
        images = [(ROOT/p).read_bytes() for p in ('assets/USA/main.exe', 'build/USA/main.exe')]
        base = 0x8006374C
        offset = base - 0x8000F800
        self.assertEqual(images[0][offset:offset+0x18C], images[1][offset:offset+0x18C])
        syms = {'SetDrawArea': 0x80075B84}
        NODE=0x80100000;ARENA=0x80110000;OT=0x80120000
        cases=list(itertools.product((0,1,3),range(4),((0,0),(12,-20),(-32768,32767),(0x7FFFFFFF,-1)),
            ((1,1,8,12),(3,4,17,9),(0,0,0,0),(-1,2,32767,-32768)),(0,1,2),(16,20,0x4000),(0,1)))
        saved=[getattr(R,f'UC_MIPS_REG_S{i}') for i in range(8)]+[R.UC_MIPS_REG_FP]
        for candidate, image in enumerate(images):
            m=Uc(UC_ARCH_MIPS,UC_MODE_MIPS32|UC_MODE_LITTLE_ENDIAN);m.mem_map(0,0x200000)
            def put(a,v):m.mem_write(a&0x1FFFFFFF,bytes(v))
            def read(a,n):return bytes(m.mem_read(a&0x1FFFFFFF,n))
            def get(a):return struct.unpack('<I',read(a,4))[0]
            def word(a,v):put(a,struct.pack('<I',v&0xFFFFFFFF))
            put(0x8000F800,image)
            hookaddr=syms['SetDrawArea'];put(hookaddr,struct.pack('<II',0x03E00008,0))
            events=[]
            def hook(uc,address,size,data):
                if address!=hookaddr:return
                packet=uc.reg_read(R.UC_MIPS_REG_A0);rect=uc.reg_read(R.UC_MIPS_REG_A1)
                events.append((packet,read(rect,8),get(0x8009D100)))
                put(packet,struct.pack('<III',0x02ABCDEF,0xE3000123,0xE4000456))
                if mutation:word(0x8009D11C,OT+4)
                for name in ('V0','V1','A0','A1','A2','A3','T0','T1','T2','T3','T4','T5','T6','T7','T8','T9'):
                    uc.reg_write(getattr(R,'UC_MIPS_REG_'+name),0xCCCCCCCC)
            m.hook_add(UC_HOOK_CODE,hook)
            for depth,child,coords,dims,offset,room,mutation in cases:
                case=(candidate,depth,child,coords,dims,offset,room,mutation)
                put(NODE-16,b'\xA5'*0x620)
                # One unrelated list entry precedes the ancestors. Parent fields are
                # deliberately wrong: retail finds ancestry by searching child links.
                for i in range(5):
                    n=NODE+i*0x100
                    word(n,0);word(n+4,0xDEADBEEF)
                    for c in range(4):word(n+8+c*4,0)
                chain=[NODE+0x400]+[NODE+i*0x100 for i in range(1,depth+1)]
                for i,n in enumerate(chain):word(n,chain[i+1] if i+1<len(chain) else 0)
                word(0x8009D154,chain[0]);sx=sy=0
                for i in range(depth+1):
                    n=NODE+i*0x100;x=coords[0]+i;y=coords[1]-i
                    word(n+0x18,x);word(n+0x1C,y);sx+=x;sy+=y
                    if i:word(n+8+child*4,n-0x100)
                columns,rows,w,h=dims
                for off,v in ((0x34,columns),(0x38,rows),(0x3C,w),(0x40,h)):word(NODE+off,v)
                node_before=read(NODE-16,0x620)
                packet=ARENA+0x4000-room
                put(packet-8,b'\xA5'*32);put(OT,struct.pack('<II',0xAA654321,0xBB123456))
                word(0x8009D100,packet);word(0x8009D104,ARENA);word(0x8009D108,offset);word(0x8009D11C,OT)
                rect=struct.pack('<4H',sx&65535,(sy+(224 if offset else 0))&65535,(columns*w)&65535,(rows*h)&65535)
                expected_packet=struct.pack('<III',0x02000000|(0x123456 if mutation else 0x654321),0xE3000123,0xE4000456)
                expected_ot=struct.pack('<II',0xAA654321 if mutation else 0xAA000000|(packet&0xFFFFFF),0xBB000000|(packet&0xFFFFFF) if mutation else 0xBB123456)
                events.clear()
                for name,value in (('A0',NODE),('SP',0x801F0000),('GP',0x8009CD70),('RA',0x80010000)):
                    m.reg_write(getattr(R,'UC_MIPS_REG_'+name),value)
                for i,r in enumerate(saved):m.reg_write(r,0xABCD0000+i)
                m.emu_start(base,0x80010000,count=20000)
                assert events==[(packet,rect,packet+12)],case
                assert read(packet-8,32)==b'\xA5'*8+expected_packet+b'\xA5'*12,case
                assert read(OT,8)==expected_ot,case
                assert read(NODE-16,0x620)==node_before,case
                assert (get(0x8009D100),get(0x8009D104),get(0x8009D108),get(0x8009D11C))==(packet+12,ARENA,offset,OT+4*mutation),case
                assert all(m.reg_read(r)==0xABCD0000+i for i,r in enumerate(saved)),case
                assert (m.reg_read(R.UC_MIPS_REG_SP),m.reg_read(R.UC_MIPS_REG_GP),m.reg_read(R.UC_MIPS_REG_PC))==(0x801F0000,0x8009CD70,0x80010000),case
            print('candidate' if candidate else 'retail','verified',len(cases))


if __name__ == '__main__':
    unittest.main()
