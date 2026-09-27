"""Signed low-halfword clipping, guarded state writes, retail bytes and ABI."""
from importlib.util import find_spec
from pathlib import Path
import itertools
import struct
import unittest

ROOT = Path(__file__).resolve().parents[2]
BASE, SIZE, GLOBAL = 0x800679C4, 180, 0x800B1624


def check_model(images):
    from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN
    from unicorn import mips_const as R

    state, stop, stack = 0x80100010, 0x80010000, 0x801F0000
    points = (-32769, -32768, -1, 0, 1, 32767, 32768,
              0x7FFFFFFF, 0x80000000, 0xFFFFFFFF, 0x12340001)
    offsets = ((0, 0, 0), (1, 65535, 32768), (65535, 1, 65535),
               (32768, 32767, 1), (32767, 32768, 0))
    bounds = ((-100, 100, -200, 200), (-32768, 32767, -32768, 32767),
              (0, 0, -1, -1), (100, -100, 200, -200),
              (-32768, -32768, 32767, 32767), (32767, 32767, -32768, -32768))
    cases = list(itertools.product(points, points, (0, -1, 0x7FFFFFFF), offsets, bounds))
    preserved = [getattr(R, 'UC_MIPS_REG_S' + str(i)) for i in range(8)] + [R.UC_MIPS_REG_FP]

    def clipped(value, low, high):
        value &= 65535
        signed = value - 65536 if value & 32768 else value
        if signed < low:
            return low & 65535, 'minimum'
        if signed > high:
            return high & 65535, 'maximum'
        return value, 'inside'

    for label, code in images:
        machine = Uc(UC_ARCH_MIPS, UC_MODE_MIPS32 | UC_MODE_LITTLE_ENDIAN)
        machine.mem_map(0, 0x200000)
        def put(address, data):
            machine.mem_write(address & 0x1FFFFFFF, bytes(data))
        def read(address, size):
            return bytes(machine.mem_read(address & 0x1FFFFFFF, size))
        put(BASE, code)
        global_guard = b'\x5A' * 8 + struct.pack('<I', state) + b'\x5A' * 8
        put(GLOBAL - 8, global_guard)
        stack_guard = b'\xC3' * 96
        coverage = set()
        for x, y, z, (ox, oy, oz), (xmin, xmax, ymin, ymax) in cases:
            initial = bytearray(b'\xA5' * 80)
            struct.pack_into('<H', initial, 8 + 0x24, oz)
            struct.pack_into('<HH', initial, 8 + 0x28, ox, oy)
            struct.pack_into('<hhhh', initial, 8 + 0x30, xmin, xmax, ymin, ymax)
            expected = bytearray(initial)
            cx, bx = clipped(ox + x, xmin, xmax)
            cy, by = clipped(oy + y, ymin, ymax)
            coverage.update((('x', bx), ('y', by)))
            struct.pack_into('<H', expected, 8 + 0x26, (oz + z) & 65535)
            struct.pack_into('<HH', expected, 8 + 0x2C, cx, cy)
            put(state - 8, initial)
            put(stack - 64, stack_guard)
            for name, value in (('A0', x), ('A1', y), ('A2', z),
                                ('SP', stack), ('GP', 0x8009CD70), ('RA', stop)):
                machine.reg_write(getattr(R, 'UC_MIPS_REG_' + name), value & 0xFFFFFFFF)
            for i, reg in enumerate(preserved):
                machine.reg_write(reg, 0xABCD0000 + i)
            machine.emu_start(BASE, stop, count=1000)
            context = (label, x, y, z, ox, oy, oz, xmin, xmax, ymin, ymax)
            assert read(state - 8, len(expected)) == expected, context
            assert read(GLOBAL - 8, len(global_guard)) == global_guard, context
            assert read(stack - 64, len(stack_guard)) == stack_guard, context
            assert machine.reg_read(R.UC_MIPS_REG_V0) == 0, context
            assert machine.reg_read(R.UC_MIPS_REG_SP) == stack, context
            assert machine.reg_read(R.UC_MIPS_REG_GP) == 0x8009CD70, context
            assert machine.reg_read(R.UC_MIPS_REG_PC) == stop, context
            for i, reg in enumerate(preserved):
                assert machine.reg_read(reg) == 0xABCD0000 + i, context
        assert coverage == set(itertools.product(('x', 'y'), ('minimum', 'maximum', 'inside')))
    return len(cases)


class GeoClipPointTests(unittest.TestCase):
    @unittest.skipUnless((ROOT / 'assets/USA/main.exe').is_file() and
                         (ROOT / 'build/USA/main.exe').is_file() and find_spec('unicorn'),
                         'images or unicorn unavailable')
    def test_model_and_exact_bytes(self):
        images = []
        for name in ('assets/USA/main.exe', 'build/USA/main.exe'):
            image = (ROOT / name).read_bytes()
            offset = BASE - 0x8000F800
            images.append((name, image[offset:offset + SIZE]))
        self.assertEqual(images[0][1], images[1][1])
        self.assertEqual(check_model(images), 10890)


if __name__ == '__main__':
    unittest.main()
