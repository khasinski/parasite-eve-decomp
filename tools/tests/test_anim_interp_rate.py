"""Exhaustive signed-16 interpolation-rate semantics and plain-C matching."""
from pathlib import Path
import itertools
import random
import re
import unittest

ROOT = Path(__file__).resolve().parents[2]


class AnimInterpRateTests(unittest.TestCase):
    def test_plain_c(self):
        source = (ROOT / 'src/main/anim/Anim_SetInterpRate.c').read_text()
        source = re.sub(r'/\*.*?\*/|//[^\n]*', '', source, flags=re.S)
        self.assertIsNone(re.search(r'\b(?:asm|__asm__|INCLUDE_ASM|CC_POSTPASS|REGALLOC_BARRIER)\b', source))

    @unittest.skipUnless((ROOT / 'assets/USA/main.exe').is_file() and
                         (ROOT / 'build/USA/main.exe').is_file(), 'retail/build unavailable')
    def test_all_signed_16_rates(self):
        try:
            from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN
            from unicorn import mips_const as R
        except ImportError:
            self.skipTest('unicorn unavailable')
        base, size = 0x8003C5D8, 0x60
        offset = base - 0x8000F800
        bodies = [(ROOT / path).read_bytes()[offset:offset + size]
                  for path in ('assets/USA/main.exe', 'build/USA/main.exe')]
        self.assertEqual(len(bodies[0]), size)
        self.assertEqual(*bodies)
        obj, stop, stack = 0x80100020, 0x80010000, 0x801F0000
        saved = [getattr(R, f'UC_MIPS_REG_S{i}') for i in range(8)] + [R.UC_MIPS_REG_GP, R.UC_MIPS_REG_FP]
        initial = random.Random(base).randbytes(0xD4)
        values = list(range(65536)) + [hi | lo for hi, lo in itertools.product(
            (0x10000, 0x7FFF0000, 0x80000000, 0xFFFF0000),
            (0, 1, 2, 127, 128, 129, 255, 256, 32767, 32768, 65535))]
        for body in bodies:
            m = Uc(UC_ARCH_MIPS, UC_MODE_MIPS32 | UC_MODE_LITTLE_ENDIAN)
            m.mem_map(0, 0x200000)
            m.mem_write(base & 0x1FFFFFFF, body)
            for arg in values:
                signed = arg & 0xFFFF
                if signed >= 32768: signed -= 65536
                value = signed or 1
                divisor = (128 // abs(value)) * (-1 if value < 0 else 1)
                expected = bytearray(initial)
                expected[0x20 + 0x8D] = value & 255
                for field in (0x8E, 0x8F, 0x93): expected[0x20 + field] = divisor & 255
                m.mem_write((obj - 0x20) & 0x1FFFFFFF, initial)
                for name, number in (('A0', obj), ('A1', arg), ('SP', stack), ('RA', stop)):
                    m.reg_write(getattr(R, 'UC_MIPS_REG_' + name), number)
                for i, reg in enumerate(saved): m.reg_write(reg, 0xABCD0000 + i)
                m.emu_start(base, stop, count=100)
                self.assertEqual(bytes(m.mem_read((obj - 0x20) & 0x1FFFFFFF, len(expected))), expected, arg)
                self.assertEqual(m.reg_read(R.UC_MIPS_REG_PC), stop, arg)
                self.assertEqual(m.reg_read(R.UC_MIPS_REG_SP), stack, arg)
                for i, reg in enumerate(saved): self.assertEqual(m.reg_read(reg), 0xABCD0000 + i, arg)


if __name__ == '__main__': unittest.main()
