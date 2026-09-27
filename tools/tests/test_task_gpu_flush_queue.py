"""Primitive queue arithmetic, wrap behavior, aliasing and ABI preservation."""
from importlib.util import find_spec
from pathlib import Path
import itertools
import struct
import unittest

ROOT = Path(__file__).resolve().parents[2]
ENTRY, OFFSET, SIZE = 0x80070D6C, 0x6156C, 100
HEAD, TAIL, BASE = 0x80070E04, 0x80070E08, 0x80070E0C
STOP, STACK = 0x80010000, 0x801F0000


def check_model(images):
    from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN
    from unicorn import mips_const as R
    saved = [getattr(R, 'UC_MIPS_REG_S' + str(i)) for i in range(8)] + [R.UC_MIPS_REG_FP]
    cases = list(itertools.product((-8, -4, 0, 4, 8, 60, 64), repeat=2))
    values = (0, 1, 0x7FFFFFFF, 0x80000000, 0xFFFFFFFF)
    for label, code in images:
        machine = Uc(UC_ARCH_MIPS, UC_MODE_MIPS32 | UC_MODE_LITTLE_ENDIAN)
        machine.mem_map(0, 0x200000)
        def put(address, data):
            machine.mem_write(address & 0x1FFFFFFF, bytes(data))
        def read(address, size):
            return bytes(machine.mem_read(address & 0x1FFFFFFF, size))
        put(ENTRY, code)
        for (head, tail), first, second in itertools.product(cases, values, values):
            start = HEAD - 16
            expected = bytearray(b'\xA5' * 112)
            def set_word(address, value):
                struct.pack_into('<I', expected, address - start, value & 0xFFFFFFFF)
            def word(address):
                return struct.unpack_from('<I', expected, address - start)[0]
            set_word(BASE + head, first)
            set_word(BASE + tail, second)
            set_word(HEAD, head)
            set_word(TAIL, tail)
            put(start, expected)
            result = (word(BASE + head) + word(BASE + tail)) & 0xFFFFFFFF
            set_word(BASE + head, result)
            set_word(HEAD, 64 if head - 4 < 0 else head - 4)
            set_word(TAIL, (tail - 4) | 64 if tail - 4 < 0 else tail - 4)
            for name, value in (('SP', STACK), ('GP', 0x8009CD70), ('RA', STOP)):
                machine.reg_write(getattr(R, 'UC_MIPS_REG_' + name), value)
            for i, reg in enumerate(saved):
                machine.reg_write(reg, 0xABCD0000 + i)
            put(STACK - 32, b'\xC3' * 64)
            machine.emu_start(ENTRY, STOP, count=100)
            context = (label, head, tail, first, second)
            assert machine.reg_read(R.UC_MIPS_REG_PC) == STOP, context
            assert machine.reg_read(R.UC_MIPS_REG_V0) == result, context
            assert read(start, len(expected)) == expected, context
            assert read(STACK - 32, 64) == b'\xC3' * 64, context
            assert machine.reg_read(R.UC_MIPS_REG_SP) == STACK, context
            assert machine.reg_read(R.UC_MIPS_REG_GP) == 0x8009CD70, context
            for i, reg in enumerate(saved):
                assert machine.reg_read(reg) == 0xABCD0000 + i, context
    return len(cases) * len(values) ** 2


class GpuQueueTests(unittest.TestCase):
    @unittest.skipUnless((ROOT / 'assets/USA/main.exe').is_file() and
                         (ROOT / 'build/USA/main.exe').is_file() and find_spec('unicorn'),
                         'images or unicorn unavailable')
    def test_exact_bytes_and_model(self):
        images = [(name, (ROOT / name).read_bytes()[OFFSET:OFFSET + SIZE])
                  for name in ('assets/USA/main.exe', 'build/USA/main.exe')]
        self.assertEqual(images[0][1], images[1][1])
        self.assertEqual(check_model(images), 1225)


if __name__ == '__main__':
    unittest.main()
