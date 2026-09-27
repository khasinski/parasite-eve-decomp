"""Plain-C expression slide: whole-TU byte gate and independent RAM oracle."""
from pathlib import Path
import itertools
import random
import re
import struct
import unittest

ROOT = Path(__file__).resolve().parents[2]


class ExpressionSlideTests(unittest.TestCase):
    def test_translation_unit_is_plain_c(self):
        source = (ROOT / 'src/main/akao/akao5.c').read_text()
        source = re.sub(r'/\*.*?\*/|//[^\n]*', '', source, flags=re.S)
        self.assertIsNone(re.search(r'\b(?:asm|__asm__|INCLUDE_ASM|CC_POSTPASS|REGALLOC_BARRIER)\b', source))

    @unittest.skipUnless((ROOT / 'assets/USA/main.exe').is_file() and
                         (ROOT / 'build/USA/main.exe').is_file(), 'retail/build unavailable')
    def test_expression_slide(self):
        try:
            from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN
            from unicorn import mips_const as R
        except ImportError:
            self.skipTest('unicorn unavailable')
        base, size = 0x8008F608, 0x4F8
        offset = base - 0x8000F800
        bodies = [(ROOT / path).read_bytes()[offset:offset + size]
                  for path in ('assets/USA/main.exe', 'build/USA/main.exe')]
        self.assertEqual(len(bodies[0]), size)
        self.assertEqual(*bodies)
        track, script, stop, stack = 0x80100020, 0x80120020, 0x80010000, 0x801F0000
        saved = [getattr(R, f'UC_MIPS_REG_S{i}') for i in range(8)] + [R.UC_MIPS_REG_GP, R.UC_MIPS_REG_FP]
        initial = random.Random(base).randbytes(0x160)
        for body in bodies:
            m = Uc(UC_ARCH_MIPS, UC_MODE_MIPS32 | UC_MODE_LITTLE_ENDIAN)
            m.mem_map(0, 0x200000)
            def put(a, data): m.mem_write(a & 0x1FFFFFFF, bytes(data))
            def read(a, n): return bytes(m.mem_read(a & 0x1FFFFFFF, n))
            put(base, body)
            for alias in (False, True):
                durations = (0, 1, 2, 7, 63, 127, 128, 255) if alias else range(256)
                for duration, target, current in itertools.product(durations, (0, 127, 128, 255),
                                                                  (0, 0x1FFFF, 0x1000FFFF, 0xF000FFFF)):
                    pc = track + 0x72 if alias else script
                    expected = bytearray(initial)
                    struct.pack_into('<I', expected, 0x20, pc)
                    struct.pack_into('<I', expected, 0x20 + 0x44, current)
                    program = bytes((duration, target, 0x55, 0xAA))
                    if alias: expected[0x92:0x96] = program
                    put(track - 0x20, expected)
                    put(script, program)
                    # Sequential stores matter when the stream overlaps duration.
                    struct.pack_into('<I', expected, 0x20, pc + 1)
                    ticks = duration or 256
                    struct.pack_into('<H', expected, 0x92, ticks)
                    next_byte = expected[0x93] if alias else target
                    struct.pack_into('<I', expected, 0x20, pc + 2)
                    signed_target = next_byte if next_byte < 128 else next_byte - 256
                    masked = current & 0xFFFF0000
                    signed_current = masked if masked < 0x80000000 else masked - 0x100000000
                    difference = signed_target * (1 << 23) - signed_current
                    delta = (abs(difference) // ticks) * (-1 if difference < 0 else 1)
                    struct.pack_into('<Ii', expected, 0x64, masked, delta)
                    for name, value in (('A0', track), ('SP', stack), ('RA', stop)):
                        m.reg_write(getattr(R, 'UC_MIPS_REG_' + name), value)
                    for i, reg in enumerate(saved): m.reg_write(reg, 0xABCD0000 + i)
                    m.emu_start(base, stop, count=200)
                    case = (alias, duration, target, current)
                    self.assertEqual(read(track - 0x20, len(expected)), expected, case)
                    self.assertEqual(read(script, 4), program, case)
                    self.assertEqual(m.reg_read(R.UC_MIPS_REG_PC), stop, case)
                    self.assertEqual(m.reg_read(R.UC_MIPS_REG_SP), stack, case)
                    for i, reg in enumerate(saved):
                        self.assertEqual(m.reg_read(reg), 0xABCD0000 + i, case)


if __name__ == '__main__':
    unittest.main()
