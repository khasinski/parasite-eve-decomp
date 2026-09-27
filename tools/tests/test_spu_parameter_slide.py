"""Retail model for masked parameter slides; no SPU hardware is invoked."""
from pathlib import Path
import itertools
import random
import re
import struct
import unittest

ROOT = Path(__file__).resolve().parents[2]


class SpuParameterSlideTests(unittest.TestCase):
    def test_translation_unit_is_plain_c(self):
        source = (ROOT/'src/main/akao/Spu_ParameterSlide.c').read_text()
        source = re.sub(r'/\*.*?\*/|//[^\n]*', '', source, flags=re.S)
        self.assertNotRegex(source, r'\b(?:asm|__asm__|INCLUDE_ASM|CC_POSTPASS)\b')

    @unittest.skipUnless((ROOT/'assets/USA/main.exe').is_file() and
                         (ROOT/'build/USA/main.exe').is_file(), 'retail/build unavailable')
    def test_retail_and_built_model(self):
        try:
            from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN
            from unicorn import mips_const as R
        except ImportError:
            self.skipTest('unicorn unavailable')
        entry,stop,stack,arg = 0x8008B780,0x80010000,0x801F0000,0x80110000
        tracks,active_address = 0x800BC000,0x800BCD50
        offset,size = entry-0x8000F800,0x180
        bodies = [(ROOT/path).read_bytes()[offset:offset+size]
                  for path in ('assets/USA/main.exe','build/USA/main.exe')]
        self.assertEqual(len(bodies[0]),size)
        self.assertEqual(*bodies)
        cases = list(itertools.product((0,0x1000,0x800000,0xFFF000,0x555000,0xAAA000,0xFFF,0xFFFFFFFF),
                                       (0,1,2,127,32767,32768,65535,0xFFFF0001),
                                       (0,1,127,128,255,0x7FFFFFFF,0x80000000,0xFFFFFFFF),
                                       ((0,0),(0,1),(0,0xFFFFFFFF),(1,0),(2,0),(0x80000000,0),(0xFFFFFFFF,0))))
        saved = [getattr(R,f'UC_MIPS_REG_S{i}') for i in range(8)]+[R.UC_MIPS_REG_FP,R.UC_MIPS_REG_GP]
        initial = bytearray(random.Random(entry).randbytes(12*0x11C))
        for i in range(12):
            struct.pack_into('<I',initial,i*0x11C+0x28,(0,1,0xFFFFFFFF,2)[i%4])
            struct.pack_into('<I',initial,i*0x11C+0x2C,(0,1,2,3,0x80000000,0xFFFFFFFF)[i%6])
            struct.pack_into('<H',initial,i*0x11C+0xD8,(0,1,0x7FFF,0x8000,0xFFFF,0xFF00)[i%6])
        def signed16(x): return (x&32767)-(x&32768)
        # Arguments and tracks are disjoint. Divide-by-zero durations (nonzero
        # full32 value with low16 zero) are outside this numerical model.
        for body in bodies:
            m = Uc(UC_ARCH_MIPS,UC_MODE_MIPS32|UC_MODE_LITTLE_ENDIAN)
            m.mem_map(0,0x200000)
            def put(address,data): m.mem_write(address&0x1FFFFFFF,bytes(data))
            put(entry,body)
            for active,step,target,(selection,identifier) in cases:
                expected = bytearray(initial)
                duration = step or 1
                for i in range(12):
                    flags = struct.unpack_from('<I',initial,i*0x11C+0x2C)[0]
                    track_id = struct.unpack_from('<I',initial,i*0x11C+0x28)[0]
                    selected = bool(flags&selection) if selection else track_id==identifier
                    if active&(0x1000<<i) and selected:
                        current = struct.unpack_from('<H',initial,i*0x11C+0xD8)[0]
                        delta,denom = signed16(((target&127)<<8)-current),signed16(duration)
                        quotient = (abs(delta)//abs(denom))*(-1 if (delta<0)!=(denom<0) else 1)
                        struct.pack_into('<H',expected,i*0x11C+0x74,duration&65535)
                        struct.pack_into('<H',expected,i*0x11C+0xDA,quotient&65535)
                put(tracks,initial)
                put(active_address,struct.pack('<I',active))
                arguments = struct.pack('<IIIII',0xAABBCCDD,identifier,selection,step,target)
                put(arg,arguments)
                for name,value in (('A0',arg),('SP',stack),('RA',stop)):
                    m.reg_write(getattr(R,'UC_MIPS_REG_'+name),value)
                for i,reg in enumerate(saved): m.reg_write(reg,0xABCD0000+i)
                m.emu_start(entry,stop,count=1500)
                self.assertEqual(bytes(m.mem_read(tracks&0x1FFFFFFF,len(expected))),expected,(active,step,target,selection,identifier))
                self.assertEqual(bytes(m.mem_read(arg&0x1FFFFFFF,20)),arguments)
                self.assertEqual(bytes(m.mem_read(active_address&0x1FFFFFFF,4)),struct.pack('<I',active))
                self.assertEqual(m.reg_read(R.UC_MIPS_REG_PC),stop)
                self.assertEqual(m.reg_read(R.UC_MIPS_REG_SP),stack)
                for i,reg in enumerate(saved): self.assertEqual(m.reg_read(reg),0xABCD0000+i)


if __name__ == '__main__':
    unittest.main()
