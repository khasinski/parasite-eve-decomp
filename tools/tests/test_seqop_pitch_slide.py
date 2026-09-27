"""Plain-C sequence pitch slide with sequential-memory reference semantics."""
from pathlib import Path
import itertools
import random
import re
import struct
import unittest

ROOT = Path(__file__).resolve().parents[2]


class SeqOpPitchSlideTests(unittest.TestCase):
    def test_translation_unit_is_plain_c(self):
        source = (ROOT / 'src/main/akao/seq_op.c').read_text()
        source = re.sub(r'/\*.*?\*/|//[^\n]*', '', source, flags=re.S)
        self.assertIsNone(re.search(r'\b(?:asm|__asm__|INCLUDE_ASM|CC_POSTPASS|REGALLOC_BARRIER)\b', source))

    @unittest.skipUnless((ROOT / 'assets/USA/main.exe').is_file() and
                         (ROOT / 'build/USA/main.exe').is_file(), 'retail/build unavailable')
    def test_pitch_slide(self):
        try:
            from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN
            from unicorn import mips_const as R
        except ImportError:
            self.skipTest('unicorn unavailable')
        base, size = 0x8008F0D0, 0x258
        offset = base - 0x8000F800
        bodies = [(ROOT / path).read_bytes()[offset:offset + size]
                  for path in ('assets/USA/main.exe', 'build/USA/main.exe')]
        self.assertEqual(len(bodies[0]), size)
        self.assertEqual(*bodies)
        bank, track, script = 0x80100020, 0x80110020, 0x80120020
        stop, stack = 0x80010000, 0x801F0000
        saved = [getattr(R, f'UC_MIPS_REG_S{i}') for i in range(8)] + [R.UC_MIPS_REG_GP, R.UC_MIPS_REG_FP]
        rng = random.Random(base)
        bank_initial, track_initial, globals_initial = (rng.randbytes(n) for n in (0xC0, 0x160, 0x20))
        for body in bodies:
            m = Uc(UC_ARCH_MIPS, UC_MODE_MIPS32 | UC_MODE_LITTLE_ENDIAN)
            m.mem_map(0, 0x200000)
            def put(a, data): m.mem_write(a & 0x1FFFFFFF, bytes(data))
            def read(a, n): return bytes(m.mem_read(a & 0x1FFFFFFF, n))
            put(base, body)
            for alias in (False, True):
                durations = (0, 1, 2, 7, 63, 127, 128, 255) if alias else range(256)
                for duration, target, current in itertools.product(durations,
                        (-16384, -1, 0, 1, 255, 16383), (0, 0x1FFFF, 0x1000FFFF, 0xF000FFFF)):
                    pc = bank + 0x52 if alias else script
                    expected_bank, expected_track = bytearray(bank_initial), bytearray(track_initial)
                    expected_globals = bytearray(globals_initial)
                    struct.pack_into('<I', expected_bank, 0x40, current)
                    struct.pack_into('<I', expected_track, 0x20, pc)
                    struct.pack_into('<I', expected_globals, 8, bank)
                    program = bytes((duration, target & 255, (target >> 8) & 255, 0xAA))
                    if alias: expected_bank[0x72:0x76] = program
                    put(bank - 0x20, expected_bank)
                    put(track - 0x20, expected_track)
                    put(0x8009D2C0, expected_globals)
                    put(script, program)
                    ticks = duration or 256
                    struct.pack_into('<H', expected_bank, 0x72, ticks)
                    low, high = expected_bank[0x73:0x75] if alias else program[1:3]
                    target_word = (low << 16) | (high << 24)
                    signed_target = target_word if target_word < 0x80000000 else target_word - 0x100000000
                    masked = current & 0xFFFF0000
                    signed_current = masked if masked < 0x80000000 else masked - 0x100000000
                    difference = signed_target - signed_current
                    delta = (abs(difference) // ticks) * (-1 if difference < 0 else 1)
                    struct.pack_into('<Ii', expected_bank, 0x40, masked, delta)
                    struct.pack_into('<I', expected_track, 0x20, pc + 3)
                    for name, value in (('A0', track), ('SP', stack), ('RA', stop)):
                        m.reg_write(getattr(R, 'UC_MIPS_REG_' + name), value)
                    for i, reg in enumerate(saved): m.reg_write(reg, 0xABCD0000 + i)
                    m.emu_start(base + 0x1A4, stop, count=200)
                    case = (alias, duration, target, current)
                    self.assertEqual(read(bank - 0x20, len(expected_bank)), expected_bank, case)
                    self.assertEqual(read(track - 0x20, len(expected_track)), expected_track, case)
                    self.assertEqual(read(0x8009D2C0, len(expected_globals)), expected_globals, case)
                    self.assertEqual(read(script, 4), program, case)
                    self.assertEqual(m.reg_read(R.UC_MIPS_REG_PC), stop, case)
                    self.assertEqual(m.reg_read(R.UC_MIPS_REG_SP), stack, case)
                    for i, reg in enumerate(saved):
                        self.assertEqual(m.reg_read(reg), 0xABCD0000 + i, case)


if __name__ == '__main__':
    unittest.main()
