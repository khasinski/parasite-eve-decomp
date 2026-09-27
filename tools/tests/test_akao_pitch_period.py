"""Partial pitch-period pin removal and complete lookup arithmetic coverage."""
from pathlib import Path
import itertools
import random
import unittest

ROOT = Path(__file__).resolve().parents[2]


class PitchPeriodTests(unittest.TestCase):
    def test_offset_pin_is_removed(self):
        source = (ROOT/'src/main/akao/voice_pitch.c').read_text()
        self.assertLessEqual(source.count('asm("$'),1)
        self.assertNotRegex(source,r'offset\s+asm')

    @unittest.skipUnless((ROOT/'assets/USA/main.exe').is_file() and
                         (ROOT/'build/USA/main.exe').is_file(), 'retail/build unavailable')
    def test_period_lookup(self):
        try:
            from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN
            from unicorn import mips_const as R
        except ImportError:
            self.skipTest('unicorn unavailable')
        base = 0x8008E4E8
        offset,size = base-0x8000F800,0x3E8
        retail = (ROOT/'assets/USA/main.exe').read_bytes()[offset:offset+size]
        compiled = (ROOT/'build/USA/main.exe').read_bytes()[offset:offset+size]
        self.assertEqual(len(retail),size)
        self.assertEqual(retail,compiled)
        retail_syms = candidate_syms = {
            'Akao_LookupPitchPeriod':0x8008E840,
            'g_AkaoPitchPeriodTable':0x800B2910,
        }
        stop,stack = 0x80010000,0x801F0000
        saved = [getattr(R,f'UC_MIPS_REG_S{i}') for i in range(8)]+[R.UC_MIPS_REG_GP,R.UC_MIPS_REG_FP]
        rng = random.Random(base)
        table_addr = retail_syms['g_AkaoPitchPeriodTable']
        for case,(row,note,adjust,high) in enumerate(itertools.product((0,1,15),range(256),(0,1,127,255,0xFFFFFFFF),(0,0x12340000,0xFFFFFF00))):
            table = rng.randbytes(0x400)
            outputs = []
            for body,syms in ((retail,retail_syms),(compiled,candidate_syms)):
                m = Uc(UC_ARCH_MIPS,UC_MODE_MIPS32|UC_MODE_LITTLE_ENDIAN)
                m.mem_map(0,0x200000)
                m.mem_write(base&0x1FFFFFFF,body)
                m.mem_write(table_addr&0x1FFFFFFF,table)
                for reg,val in (('A0',row),('A1',note|high),('A2',adjust),('SP',stack),('RA',stop)):
                    m.reg_write(getattr(R,'UC_MIPS_REG_'+reg),val)
                for i,reg in enumerate(saved): m.reg_write(reg,0xABCD0000+i)
                m.emu_start(syms['Akao_LookupPitchPeriod'],stop,count=1000)
                assert m.reg_read(R.UC_MIPS_REG_PC)==stop and m.reg_read(R.UC_MIPS_REG_SP)==stack,case
                for i,reg in enumerate(saved): assert m.reg_read(reg)==0xABCD0000+i,case
                assert bytes(m.mem_read(table_addr&0x1FFFFFFF,len(table)))==table,case
                outputs.append(m.reg_read(R.UC_MIPS_REG_V0))
            assert outputs[0]==outputs[1],(case,row,note,adjust,outputs)
            expected = int.from_bytes(table[row*64+(note%12)*4:row*64+(note%12)*4+4],'little')
            if adjust: expected = (expected+((expected*adjust&0xFFFFFFFF)>>7))&0xFFFFFFFF
            octave = note//12
            if octave >= 7: expected = (expected << (octave-6))&0xFFFFFFFF
            elif octave < 6: expected >>= 6-octave
            assert outputs[0] == expected&0xFFFF,(case,outputs,expected)


if __name__ == '__main__': unittest.main()
