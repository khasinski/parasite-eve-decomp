"""Shared current-track declaration and transpose command regression."""
from pathlib import Path
import itertools
import random
import struct
import unittest

ROOT = Path(__file__).resolve().parents[2]


class Seq4TrackPointerTests(unittest.TestCase):
    def test_duplicate_assembly_alias_is_gone(self):
        source=(ROOT/'src/main/akao/seq4.c').read_text()
        self.assertNotIn('g_AkaoCurTrack_1',source)
        self.assertNotIn('__asm__',source)
        self.assertEqual(source.count('extern char *g_AkaoCurTrack;'),1)

    @unittest.skipUnless((ROOT/'assets/USA/main.exe').is_file() and
                         (ROOT/'build/USA/main.exe').is_file(), 'retail/build unavailable')
    def test_transpose_command(self):
        try:
            from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN
            from unicorn import mips_const as R
        except ImportError:
            self.skipTest('unicorn unavailable')
        base,size=0x800903A0,0x620
        offset=base-0x8000F800
        bodies=[(ROOT/path).read_bytes()[offset:offset+size]
                for path in ('assets/USA/main.exe','build/USA/main.exe')]
        self.assertEqual(len(bodies[0]),size)
        self.assertEqual(*bodies)
        track,bank,script,stop,stack=0x80100020,0x80110020,0x80120020,0x80010000,0x801F0000
        saved=[getattr(R,f'UC_MIPS_REG_S{i}') for i in range(8)]+[R.UC_MIPS_REG_GP,R.UC_MIPS_REG_FP]
        rng=random.Random(base)
        track_initial=rng.randbytes(0x160);bank_initial=rng.randbytes(0xC0)
        globals_initial=rng.randbytes(0x20)
        for body in bodies:
            m=Uc(UC_ARCH_MIPS,UC_MODE_MIPS32|UC_MODE_LITTLE_ENDIAN);m.mem_map(0,0x200000)
            def put(a,data): m.mem_write(a&0x1FFFFFFF,bytes(data))
            def read(a,n): return bytes(m.mem_read(a&0x1FFFFFFF,n))
            put(base,body)
            for value,parent,initial in itertools.product(range(256),(0,1),(0,63,65535)):
                before_track=bytearray(track_initial);before_bank=bytearray(bank_initial)
                before_global=bytearray(globals_initial)
                struct.pack_into('<I',before_track,0x20,script)
                struct.pack_into('<H',before_track,0x20+0x54,parent)
                struct.pack_into('<H',before_bank,0x20+0x5A,initial)
                struct.pack_into('<H',before_global,8,initial)
                expected_track=bytearray(before_track);expected_bank=bytearray(before_bank)
                expected_global=bytearray(before_global)
                struct.pack_into('<I',expected_track,0x20,script+1)
                updated=((initial+(value&63))&63) if value&0xC0 else value
                if parent: struct.pack_into('<H',expected_global,8,updated)
                else: struct.pack_into('<H',expected_bank,0x20+0x5A,updated)
                flags=(value*0x10001)^0xA55A00E0
                put(track-0x20,before_track);put(bank-0x20,before_bank)
                put(0x800BCD70,before_global)
                put(0x8009D2C4,struct.pack('<II',flags,bank))
                program=bytes((value,0xAA,0x55,0xFF));put(script,program)
                for name,v in (('A0',track),('SP',stack),('RA',stop)): m.reg_write(getattr(R,'UC_MIPS_REG_'+name),v)
                for i,reg in enumerate(saved): m.reg_write(reg,0xABCD0000+i)
                m.emu_start(0x800904C4,stop,count=200)
                case=(value,parent,initial)
                self.assertEqual(read(track-0x20,len(expected_track)),expected_track,case)
                self.assertEqual(read(bank-0x20,len(expected_bank)),expected_bank,case)
                self.assertEqual(read(0x800BCD70,len(expected_global)),expected_global,case)
                self.assertEqual(read(0x8009D2C4,8),struct.pack('<II',flags|0x10,bank),case)
                self.assertEqual(read(script,4),program,case)
                self.assertEqual(m.reg_read(R.UC_MIPS_REG_PC),stop,case)
                self.assertEqual(m.reg_read(R.UC_MIPS_REG_SP),stack,case)
                for i,reg in enumerate(saved): self.assertEqual(m.reg_read(reg),0xABCD0000+i,case)


if __name__ == '__main__': unittest.main()
