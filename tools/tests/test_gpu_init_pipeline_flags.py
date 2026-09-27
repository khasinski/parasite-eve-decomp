"""Verify the field-move-lock update in Gpu_InitPipeline."""
from importlib.util import find_spec
from pathlib import Path
import struct
import unittest

ROOT = Path(__file__).resolve().parents[2]
START = 0x8003F2C0
END = 0x8003F2D8
FIELD_MOVE_LOCK = 0x8009D2E8


class GpuInitPipelineFlagsTests(unittest.TestCase):
    @unittest.skipUnless((ROOT / 'assets/USA/main.exe').is_file() and
                         (ROOT / 'build/USA/main.exe').is_file() and find_spec('unicorn'),
                         'images or unicorn unavailable')
    def test_exact_window_and_bit_mask(self):
        from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN

        offset = START - 0x8000F800
        images = [(ROOT / path).read_bytes()[offset:offset + END - START]
                  for path in ('assets/USA/main.exe', 'build/USA/main.exe')]
        self.assertEqual(images[0], images[1])
        values = (0, 1, 4, 8, 12, 13, 0x100, 0xFFFFFFFF,
                  0x80000000, 0x7FFFFFFF, 0xA5A5A5A5)
        for image in images:
            machine = Uc(UC_ARCH_MIPS, UC_MODE_MIPS32 | UC_MODE_LITTLE_ENDIAN)
            machine.mem_map(0, 0x200000)
            machine.mem_write(START & 0x1FFFFFFF, image)
            for value in values:
                state = struct.pack('<III', 0xCAFEBABE, value, 0x12345678)
                machine.mem_write((FIELD_MOVE_LOCK - 4) & 0x1FFFFFFF, state)
                machine.emu_start(START, END, count=12)
                actual = bytes(machine.mem_read((FIELD_MOVE_LOCK - 4) & 0x1FFFFFFF, 12))
                expected = struct.pack('<III', 0xCAFEBABE, value & ~0xC, 0x12345678)
                self.assertEqual(actual, expected, value)


if __name__ == '__main__':
    unittest.main()
