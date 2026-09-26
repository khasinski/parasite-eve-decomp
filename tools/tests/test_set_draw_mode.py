"""Retail matching and packet-write boundaries for SetDrawMode."""
from pathlib import Path
import random
import re
import shutil
import struct
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[2]
SOURCE = ROOT/'src/main/gpu/SetPolyF3.c'


class SetDrawModeTests(unittest.TestCase):
    def test_source_has_no_assembly(self):
        source = re.sub(r'/\*.*?\*/|//[^\n]*', '', SOURCE.read_text(), flags=re.S)
        self.assertNotRegex(source, r'\b(?:asm|__asm__|INCLUDE_ASM|CC_POSTPASS)\b')

    @unittest.skipUnless(shutil.which('mipsel-none-elf-ld') and
                         (ROOT/'assets/USA/main.exe').is_file(), 'retail/toolchain unavailable')
    def test_retail_bytes_and_packet_model(self):
        try:
            from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN
            from unicorn import mips_const as R
        except ImportError:
            self.skipTest('unicorn unavailable')
        base, entry, stop = 0x80077B64, 0x80077C84, 0x80010000
        packet, stack = 0x80120010, 0x801F0000
        offset = base-0x8000F800
        retail = (ROOT/'assets/USA/main.exe').read_bytes()[offset:offset+0x150]
        with tempfile.TemporaryDirectory() as directory:
            work = Path(directory)
            subprocess.run([str(ROOT/'tools/scripts/cc.sh'),str(SOURCE),str(work/'test.o')],check=True,capture_output=True)
            (work/'test.ld').write_text(f'SECTIONS {{ .text 0x{base:X} : SUBALIGN(4) {{ *(.text) }} /DISCARD/ : {{ *(.reginfo) *(.mdebug) *(.pdr) *(.MIPS.abiflags) }} }}')
            subprocess.run(['mipsel-none-elf-ld','-EL','-T',str(work/'test.ld'),str(work/'test.o'),'-o',str(work/'test.elf')],check=True,capture_output=True)
            subprocess.run(['mipsel-none-elf-objcopy','-O','binary','-j','.text',str(work/'test.elf'),str(work/'test.bin')],check=True,capture_output=True)
            compiled = (work/'test.bin').read_bytes()
        self.assertEqual(compiled,retail)
        cases = [(a,b,bits) for bits in range(4096) for a,b in ((0,0),(0,1),(1,0),(1,1))]
        rng = random.Random(entry)
        cases += [(rng.getrandbits(32),rng.getrandbits(32),rng.getrandbits(32)) for _ in range(1024)]
        initial = bytes(range(32))
        for body in (retail,compiled):
            m = Uc(UC_ARCH_MIPS,UC_MODE_MIPS32|UC_MODE_LITTLE_ENDIAN)
            m.mem_map(0,0x200000)
            m.mem_write(base&0x1FFFFFFF,body)
            for draw_texture,dither,tpage in cases:
                m.mem_write((packet-8)&0x1FFFFFFF,initial)
                for name,value in (('A0',packet),('A1',draw_texture),('A2',dither),('A3',tpage),('SP',stack),('GP',0x8009CD70),('RA',stop)):
                    m.reg_write(getattr(R,'UC_MIPS_REG_'+name),value)
                for i in range(8): m.reg_write(getattr(R,f'UC_MIPS_REG_S{i}'),0xABCD0000+i)
                m.emu_start(entry,stop,count=100)
                expected = bytearray(initial)
                expected[11] = 1
                command = 0xE1000000 | (0x200 if dither else 0) | (tpage&0x9FF) | (0x400 if draw_texture else 0)
                expected[12:16] = struct.pack('<I',command)
                self.assertEqual(bytes(m.mem_read((packet-8)&0x1FFFFFFF,32)),bytes(expected),(draw_texture,dither,tpage))
                self.assertEqual(m.reg_read(R.UC_MIPS_REG_PC),stop)
                self.assertEqual(m.reg_read(R.UC_MIPS_REG_SP),stack)
                self.assertEqual(m.reg_read(R.UC_MIPS_REG_GP),0x8009CD70)
                for i in range(8): self.assertEqual(m.reg_read(getattr(R,f'UC_MIPS_REG_S{i}')),0xABCD0000+i)


if __name__ == '__main__':
    unittest.main()
