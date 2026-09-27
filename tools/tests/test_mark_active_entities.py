"""Battle target marking: exact linked bytes and independent actor/angle oracle.

Covers signed selection indices, unsigned sequential indices, s16 angle wrap,
target kinds, absent lists, and preserved memory/ABI. Inputs use disjoint,
bounded tables and acyclic actor lists with valid target descriptors.
"""
from pathlib import Path
import itertools
import struct
import unittest
from importlib.util import find_spec

ROOT = Path(__file__).resolve().parents[2]


class MarkActiveEntitiesTests(unittest.TestCase):
    @unittest.skipUnless((ROOT/'assets/USA/main.exe').is_file() and
                         (ROOT/'build/USA/main.exe').is_file() and
                         find_spec('unicorn'), 'images or unicorn unavailable')
    def test_behavior(self):
        from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN
        from unicorn import mips_const as R
        base = 0x800275CC
        start = base - 0x8000F800
        retail, candidate = [(ROOT/p).read_bytes()[start:start+0x43C]
                             for p in ('assets/USA/main.exe', 'build/USA/main.exe')]
        self.assertEqual(len(candidate), 0x43C)
        self.assertEqual(candidate, retail)
        syms = {name: int(name[2:], 16) for name in
                ('D_8009D278', 'D_8009D254', 'D_8009D20C', 'D_8009D2B0', 'D_8009CE6C')}
        actors,profiles,table,action=0x80104000,0x80108000,0x80101000,0x80109000
        saved=[getattr(R,f'UC_MIPS_REG_S{i}') for i in range(8)]+[R.UC_MIPS_REG_FP]
        centers=(-32768,-32257,-2048,-1537,-1536,-1535,0,1535,1536,1537,2047,32256,32767)
        cases=list(itertools.product(range(4),centers,(0,1,4,255),(False,True),(False,True),(3,129,255),(0x100,0x17F,0x180,0x1FF)))
        def narrow(v):return ((v+32768)&65535)-32768
        for body in (retail,candidate):
            m=Uc(UC_ARCH_MIPS,UC_MODE_MIPS32|UC_MODE_LITTLE_ENDIAN);m.mem_map(0,0x200000)
            def put(a,v):m.mem_write(a&0x1FFFFFFF,bytes(v))
            def read(a,n):return bytes(m.mem_read(a&0x1FFFFFFF,n))
            def word(a,v):put(a,struct.pack('<I',v))
            put(base,body)
            for mode,center,kind,list_present,enabled,count,argument in cases:
                case=(mode,center,kind,list_present,enabled,count,argument)
                selected_index=((argument+128)&255)-128
                put(actors,b'\xA5'*0x1800);put(profiles,b'\x5A'*0x100)
                for i in range(8):
                    address=actors+i*0x300
                    word(address,0 if i==5 else profiles+i*0x20)
                    put(profiles+i*0x20+5,bytes([kind if i<3 else (2 if i==6 else 4)]))
                    word(address+4,actors+(i+1)*0x300 if 4<=i<7 else 0)
                    word(address+0x18C,actors+3*0x300)
                    put(address+0x250,struct.pack('<H',0x8100+i))
                word(actors+0x68,action);word(action+0x10,mode<<6)
                word(syms['D_8009D278'],actors);word(syms['D_8009D254'],actors+4*0x300)
                word(syms['D_8009D20C'],actors+4*0x300 if list_present else 0)
                put(syms['D_8009D2B0'],bytes([int(enabled)]));put(syms['D_8009CE6C'],b'\xAA')
                table_data=bytearray(b'\x5A'*(384*12))
                angles=[]
                for i in range(-128,256):
                    angle=center if i==selected_index else (-2000,0,2000)[i%3]
                    if 0<=i<count:angles.append(angle)
                    offset=(i+128)*12
                    struct.pack_into('<I',table_data,offset,actors+(i%3)*0x300);struct.pack_into('<h',table_data,offset+8,angle)
                struct.pack_into('<I',table_data,(count+128)*12,0);put(table-128*12,table_data)
                expected=bytearray(read(actors,0x1800));before_profiles=read(profiles,0x100)
                marked=set()
                if enabled:
                    selected=[selected_index] if mode==0 else list(range(count))
                    if mode==2:
                        if center < -1536:low,high=narrow(center+512),narrow(center+3584)
                        elif center <1536:low,high=narrow(center-512),narrow(center+512)
                        else:low,high=narrow(center-3584),narrow(center-512)
                        selected=[i for i,a in enumerate(angles) if (low<=a<=high if -1536<=center<1536 else not (a<high and a>low))]
                    for i in selected:
                        if kind==1:marked.add(3)
                        elif kind==4:
                            if list_present:marked.add(7)
                        else:marked.add(i%3)
                    for i in marked:struct.pack_into('<H',expected,i*0x300+0x250,(0x8100+i)|0x20)
                for name,value in (('A0',table),('A1',argument),('SP',0x801F0000),('RA',0x80010000),('GP',0x8009CD70)):
                    m.reg_write(getattr(R,'UC_MIPS_REG_'+name),value)
                for i,r in enumerate(saved):m.reg_write(r,0xABCD0000+i)
                m.emu_start(base,0x80010000,count=60000)
                assert read(actors,0x1800)==expected,case
                assert read(table-128*12,len(table_data))==table_data and read(profiles,0x100)==before_profiles,case
                assert read(syms['D_8009CE6C'],1)==(b'\0' if enabled else b'\xAA'),case
                assert m.reg_read(R.UC_MIPS_REG_PC)==0x80010000 and m.reg_read(R.UC_MIPS_REG_SP)==0x801F0000,case
                assert m.reg_read(R.UC_MIPS_REG_GP)==0x8009CD70,case
                for i,r in enumerate(saved):assert m.reg_read(r)==0xABCD0000+i,case
