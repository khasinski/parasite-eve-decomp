"""Retail code and two-sprite field-copy model, including source/dest overlap."""
from pathlib import Path
import random
import re
import shutil
import struct
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[2]
SOURCE = ROOT/'src/main/gpu/Gpu_SetupSprites.c'


class GpuSetupSpritesTests(unittest.TestCase):
    def test_setup_function_has_no_assembly(self):
        source = SOURCE.read_text().split('void Gpu_QueuePrimitive')[0]
        source = re.sub(r'/\*.*?\*/|//[^\n]*','',source,flags=re.S)
        self.assertNotRegex(source,r'\b(?:asm|__asm__|INCLUDE_ASM|CC_POSTPASS)\b')

    @unittest.skipUnless(shutil.which('mipsel-none-elf-ld') and
                         (ROOT/'assets/USA/main.exe').is_file() and
                         (ROOT/'build/USA/main.elf').is_file(), 'retail/toolchain unavailable')
    def test_retail_bytes_and_sprite_model(self):
        try:
            from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN
            from unicorn import mips_const as R
        except ImportError:
            self.skipTest('unicorn unavailable')
        symbols = {}
        for line in subprocess.check_output(['mipsel-none-elf-nm',str(ROOT/'build/USA/main.elf')],text=True).splitlines():
            p = line.split()
            if len(p)==3: symbols[p[2]] = int(p[0],16)
        entry, stop, stack, obj = 0x8003335C,0x80010000,0x801F0000,0x80130000
        offset = entry-0x8000F800
        retail = (ROOT/'assets/USA/main.exe').read_bytes()[offset:offset+0x644]
        with tempfile.TemporaryDirectory() as directory:
            work = Path(directory)
            subprocess.run([str(ROOT/'tools/scripts/cc.sh'),str(SOURCE),str(work/'test.o')],check=True,capture_output=True)
            needed = {line.split()[-1] for line in subprocess.check_output(['mipsel-none-elf-nm','-u',str(work/'test.o')],text=True).splitlines()}
            needed.add('_gp')
            script = f'SECTIONS {{ .text 0x{entry:X} : SUBALIGN(4) {{ *(.text) }} /DISCARD/ : {{ *(.reginfo) *(.mdebug) *(.pdr) *(.MIPS.abiflags) }} }}\n'
            (work/'test.ld').write_text(script+'\n'.join(f'{name} = 0x{symbols[name]:X};' for name in sorted(needed)))
            subprocess.run(['mipsel-none-elf-ld','-EL','-T',str(work/'test.ld'),str(work/'test.o'),'-o',str(work/'test.elf')],check=True,capture_output=True)
            subprocess.run(['mipsel-none-elf-objcopy','-O','binary','-j','.text',str(work/'test.elf'),str(work/'test.bin')],check=True,capture_output=True)
            compiled = (work/'test.bin').read_bytes()
        self.assertEqual(compiled,retail)
        rng = random.Random(entry)
        cases = list(range(256))+[-2147483648,-1,256,257,65535,2147483647]
        for body in (retail,compiled):
            m = Uc(UC_ARCH_MIPS,UC_MODE_MIPS32|UC_MODE_LITTLE_ENDIAN)
            m.mem_map(0,0x200000)
            m.mem_write(entry&0x1FFFFFFF,body)
            for code in cases:
                region = rng.randbytes(0x2000)
                expected = bytearray(region)
                object_bytes = rng.randbytes(0x220)
                x,y = struct.unpack_from('<HH',object_bytes,0x210)
                for i in range(2):
                    src = (code&255)*28+i*364
                    dst = 0x8009EC40-0x8009E974+i*28
                    expected[dst+12] = expected[src]
                    expected[dst+13] = expected[src+1]
                    struct.pack_into('<HH',expected,dst+8,(x-8)&65535,(y-16)&65535)
                    expected[dst+14:dst+16] = expected[src+2:src+4]
                m.mem_write(0x9E974,region)
                m.mem_write(obj&0x1FFFFFFF,object_bytes)
                m.mem_write(symbols['g_BattleSpritePrimCountdown']&0x1FFFFFFF,b'\xA5')
                for name,value in (('A0',obj),('A1',rng.getrandbits(32)),('A2',code&0xFFFFFFFF),('GP',symbols['_gp']),('SP',stack),('RA',stop)):
                    m.reg_write(getattr(R,'UC_MIPS_REG_'+name),value)
                for i in range(8): m.reg_write(getattr(R,f'UC_MIPS_REG_S{i}'),0xABCD0000+i)
                m.emu_start(entry,stop,count=300)
                self.assertEqual(bytes(m.mem_read(0x9E974,len(region))),bytes(expected),code)
                self.assertEqual(bytes(m.mem_read(obj&0x1FFFFFFF,len(object_bytes))),object_bytes)
                self.assertEqual(bytes(m.mem_read(symbols['g_BattleSpritePrimCountdown']&0x1FFFFFFF,1)),b'\x1E')
                self.assertEqual(m.reg_read(R.UC_MIPS_REG_PC),stop)
                self.assertEqual(m.reg_read(R.UC_MIPS_REG_SP),stack)
                self.assertEqual(m.reg_read(R.UC_MIPS_REG_GP),symbols['_gp'])
                for i in range(8): self.assertEqual(m.reg_read(getattr(R,f'UC_MIPS_REG_S{i}')),0xABCD0000+i)


if __name__=='__main__':
    unittest.main()
