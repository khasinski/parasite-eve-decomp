"""Retail model for the sequence volume-slide command, without audio hardware."""
from pathlib import Path
import random
import re
import struct
import unittest

ROOT = Path(__file__).resolve().parents[2]


class AkaoVolumeSlideTargetTests(unittest.TestCase):
    def test_translation_unit_is_plain_c(self):
        source = (ROOT/'src/main/akao/SeqOp_SetVolumeSlideTarget.c').read_text()
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
        entry,stop,stack = 0x8008F37C,0x80010000,0x801F0000
        track,cursor,stream,global_track = 0x80100000,0x80110000,0x80120000,0x8009D2C8
        offset,size = entry-0x8000F800,0xB4
        bodies = [(ROOT/path).read_bytes()[offset:offset+size]
                  for path in ('assets/USA/main.exe','build/USA/main.exe')]
        self.assertEqual(len(bodies[0]),size)
        self.assertEqual(*bodies)
        rng = random.Random(entry)
        cases = [(duration,target,current) for duration in range(256)
                 for target in (0,1,32767,32768,65535)
                 for current in (0,0xFFFF,0x7FFFFFFF,0x8000ABCD,0xFFFFFFFF)]
        cases += [(rng.randrange(256),rng.randrange(65536),rng.getrandbits(32)) for _ in range(512)]
        initial = rng.randbytes(0x100)
        saved = [getattr(R,f'UC_MIPS_REG_S{i}') for i in range(8)]+[R.UC_MIPS_REG_FP,R.UC_MIPS_REG_GP]
        # Ordinary disjoint objects: no track/stream aliasing is assumed here.
        for body in bodies:
            m = Uc(UC_ARCH_MIPS,UC_MODE_MIPS32|UC_MODE_LITTLE_ENDIAN)
            m.mem_map(0,0x200000)
            def put(address,data): m.mem_write(address&0x1FFFFFFF,bytes(data))
            put(entry,body)
            put(global_track,struct.pack('<I',track))
            for duration,target,current in cases:
                before = bytearray(initial)
                struct.pack_into('<I',before,0x40,current)
                put(track,before)
                cursor_before = bytes(range(8))+struct.pack('<I',stream)+bytes(range(12,20))
                put(cursor-8,cursor_before)
                data = bytes([duration,target&255,target>>8,0xA5])
                put(stream,data)
                for name,value in (('A0',cursor),('SP',stack),('RA',stop)):
                    m.reg_write(getattr(R,'UC_MIPS_REG_'+name),value)
                for i,reg in enumerate(saved): m.reg_write(reg,0xABCD0000+i)
                m.emu_start(entry,stop,count=150)
                normalized = current&0xFFFF0000
                difference = ((target<<16)-normalized)&0xFFFFFFFF
                if difference>=0x80000000: difference -= 0x100000000
                divisor = duration or 256
                quotient = (abs(difference)//divisor)*(-1 if difference<0 else 1)
                expected = bytearray(before)
                struct.pack_into('<I',expected,0x40,normalized)
                struct.pack_into('<I',expected,0x44,quotient&0xFFFFFFFF)
                struct.pack_into('<H',expected,0x58,divisor)
                self.assertEqual(bytes(m.mem_read(track&0x1FFFFFFF,0x100)),expected,(duration,target,current))
                self.assertEqual(bytes(m.mem_read((cursor-8)&0x1FFFFFFF,20)),cursor_before[:8]+struct.pack('<I',stream+3)+cursor_before[12:])
                self.assertEqual(bytes(m.mem_read(stream&0x1FFFFFFF,4)),data)
                self.assertEqual(bytes(m.mem_read(global_track&0x1FFFFFFF,4)),struct.pack('<I',track))
                self.assertEqual(m.reg_read(R.UC_MIPS_REG_PC),stop)
                self.assertEqual(m.reg_read(R.UC_MIPS_REG_SP),stack)
                for i,reg in enumerate(saved): self.assertEqual(m.reg_read(reg),0xABCD0000+i)


if __name__ == '__main__':
    unittest.main()
