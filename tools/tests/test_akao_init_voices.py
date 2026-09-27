"""Independent24-track initialization model; remaining TU constraints untouched."""
from pathlib import Path
import itertools
import random
import re
import struct
import unittest

ROOT = Path(__file__).resolve().parents[2]


class AkaoInitVoicesTests(unittest.TestCase):
    def test_initializer_is_plain_typed_c(self):
        source = (ROOT/'src/main/akao/Akao_NestedTrack.c').read_text()
        body = source.split('void Akao_InitVoices(',1)[1].split('void Spu_ManageVoices(',1)[0]
        body = re.sub(r'/\*.*?\*/|//[^\n]*','',body,flags=re.S)
        self.assertNotRegex(body,r'\b(?:asm|__asm__|INCLUDE_ASM|CC_POSTPASS)\b')
        self.assertIn('voice->adsr_release_rate',body)
        self.assertIn('voice->update_flags',body)

    @unittest.skipUnless((ROOT/'assets/USA/main.exe').is_file() and
                         (ROOT/'build/USA/main.exe').is_file(), 'retail/build unavailable')
    def test_retail_and_built_initialization(self):
        try:
            from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN
            from unicorn import mips_const as R
        except ImportError:
            self.skipTest('unicorn unavailable')
        entry,stop,stack,state,voices = 0x8008A354,0x80010000,0x801F0000,0x80100000,0x80110010
        current,fallback = 0x8009D2C8,0x8009B8F4
        offset,size = entry-0x8000F800,0xAC
        bodies = [(ROOT/p).read_bytes()[offset:offset+size] for p in ('assets/USA/main.exe','build/USA/main.exe')]
        self.assertEqual(len(bodies[0]),size)
        self.assertEqual(*bodies)
        rng = random.Random(entry)
        saved = [getattr(R,f'UC_MIPS_REG_S{i}') for i in range(8)]+[R.UC_MIPS_REG_GP,R.UC_MIPS_REG_FP]
        cases = itertools.product((0,1,7,0xFFFF,0x10000,0xFFFFFFFF),(0,1,0x80000000),(0,1,7,0xFFFF),range(2))
        for mode,active,bank,repeat in cases:
            initial_state = bytearray(rng.randbytes(0x80))
            struct.pack_into('<I',initial_state,4,active)
            struct.pack_into('<H',initial_state,0x54,bank)
            initial_voices = bytearray(rng.randbytes(0x1AA0+0x20))
            expected_state,expected_voices = bytearray(initial_state),bytearray(initial_voices)
            if (mode==0 and active!=0) or (mode!=0 and mode==bank):
                struct.pack_into('<I',expected_state,0x18,0xFFFFFF)
                for i in range(24):
                    offset = 0x10+i*0x11C
                    struct.pack_into('<I',expected_voices,offset,fallback)
                    for field,value in ((0x56,3),(0x58,1),(0x116,5)):
                        struct.pack_into('<H',expected_voices,offset+field,value)
                    old = struct.unpack_from('<I',expected_voices,offset+0xF4)[0]
                    struct.pack_into('<I',expected_voices,offset+0xF4,old|0x4400)
            for body in bodies:
                m = Uc(UC_ARCH_MIPS,UC_MODE_MIPS32|UC_MODE_LITTLE_ENDIAN)
                m.mem_map(0,0x200000)
                def put(address,data): m.mem_write(address&0x1FFFFFFF,bytes(data))
                put(entry,body)
                put(state,initial_state)
                put(voices-0x10,initial_voices)
                put(current,struct.pack('<I',state))
                for name,value in (('A0',mode),('A1',voices),('SP',stack),('RA',stop)):
                    m.reg_write(getattr(R,'UC_MIPS_REG_'+name),value)
                for i,reg in enumerate(saved): m.reg_write(reg,0xABCD0000+i)
                m.emu_start(entry,stop,count=1000)
                self.assertEqual(m.reg_read(R.UC_MIPS_REG_PC),stop)
                self.assertEqual(m.reg_read(R.UC_MIPS_REG_SP),stack)
                for i,reg in enumerate(saved): self.assertEqual(m.reg_read(reg),0xABCD0000+i)
                self.assertEqual(bytes(m.mem_read(state&0x1FFFFFFF,len(expected_state))),expected_state)
                self.assertEqual(bytes(m.mem_read((voices-0x10)&0x1FFFFFFF,len(expected_voices))),expected_voices)
                self.assertEqual(bytes(m.mem_read(current&0x1FFFFFFF,4)),struct.pack('<I',state))


if __name__ == '__main__': unittest.main()
