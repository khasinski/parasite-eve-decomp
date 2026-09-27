"""Retail models for track volume read/slide; no audio hardware is invoked."""
from pathlib import Path
import itertools
import random
import re
import struct
import unittest

ROOT = Path(__file__).resolve().parents[2]


class AkaoTrackVolumeTests(unittest.TestCase):
    def test_translation_unit_is_plain_c(self):
        source = (ROOT/'src/main/akao/snd_track.c').read_text()
        source = re.sub(r'/\*.*?\*/|//[^\n]*', '', source, flags=re.S)
        self.assertNotRegex(source, r'\b(?:asm|__asm__|INCLUDE_ASM|CC_POSTPASS)\b')

    @unittest.skipUnless((ROOT/'assets/USA/main.exe').is_file() and
                         (ROOT/'build/USA/main.exe').is_file(), 'retail/build unavailable')
    def test_retail_and_built_models(self):
        try:
            from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN
            from unicorn import mips_const as R
        except ImportError:
            self.skipTest('unicorn unavailable')
        entry,stop,stack,track,stream = 0x8008F4E8,0x80010000,0x801F0000,0x80100000,0x80110000
        offset,size = entry-0x8000F800,0xB4
        bodies = [(ROOT/path).read_bytes()[offset:offset+size]
                  for path in ('assets/USA/main.exe','build/USA/main.exe')]
        self.assertEqual(len(bodies[0]),size)
        self.assertEqual(*bodies)
        initial = bytearray(random.Random(entry).randbytes(0x140))
        struct.pack_into('<I',initial,0,stream)
        flags = int.from_bytes(initial[0xF4:0xF8],'little')
        cases = [('read',byte,0,0) for byte in range(256)]
        cases += [('slide',duration,target,current)
                  for duration,target,current in itertools.product(range(256),(0,1,127,128,255),(0,0xFFFF,0x7FFF,0x80FF))]
        saved = [getattr(R,f'UC_MIPS_REG_S{i}') for i in range(8)]+[R.UC_MIPS_REG_GP,R.UC_MIPS_REG_FP]
        for body in bodies:
            m = Uc(UC_ARCH_MIPS,UC_MODE_MIPS32|UC_MODE_LITTLE_ENDIAN)
            m.mem_map(0,0x200000)
            m.mem_write(entry&0x1FFFFFFF,body)
            for kind,byte,target,current in cases:
                before = bytearray(initial)
                struct.pack_into('<H',before,0x6C,current)
                expected = bytearray(before)
                data = bytes([byte,target,0xA5,0x5A])
                if kind=='read':
                    struct.pack_into('<I',expected,0,stream+1)
                    struct.pack_into('<I',expected,0xF4,flags|3)
                    struct.pack_into('<H',expected,0x6C,byte<<8)
                else:
                    duration = byte or 256
                    level = current&0x7F00
                    difference = (target<<8)-level
                    delta = (abs(difference)//duration)*(-1 if difference<0 else 1)
                    struct.pack_into('<I',expected,0,stream+2)
                    struct.pack_into('<H',expected,0x6C,level)
                    struct.pack_into('<H',expected,0x6E,duration)
                    struct.pack_into('<H',expected,0xD4,delta&65535)
                m.mem_write(track&0x1FFFFFFF,bytes(before))
                m.mem_write(stream&0x1FFFFFFF,data)
                for name,value in (('A0',track),('SP',stack),('RA',stop)):
                    m.reg_write(getattr(R,'UC_MIPS_REG_'+name),value)
                for i,reg in enumerate(saved): m.reg_write(reg,0xABCD0000+i)
                m.emu_start(entry+(0x2C if kind=='slide' else 0),stop,count=100)
                self.assertEqual(bytes(m.mem_read(track&0x1FFFFFFF,0x140)),expected,(kind,byte,target,current))
                self.assertEqual(bytes(m.mem_read(stream&0x1FFFFFFF,4)),data)
                self.assertEqual(m.reg_read(R.UC_MIPS_REG_PC),stop)
                self.assertEqual(m.reg_read(R.UC_MIPS_REG_SP),stack)
                for i,reg in enumerate(saved): self.assertEqual(m.reg_read(reg),0xABCD0000+i)


if __name__ == '__main__':
    unittest.main()
