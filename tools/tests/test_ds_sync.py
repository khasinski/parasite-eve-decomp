"""DsSync reads the requested status word without changing caller state."""
from importlib.util import find_spec
from pathlib import Path
import struct
import unittest

ROOT = Path(__file__).resolve().parents[2]
ENTRY = 0x8007FBF0
TABLE = 0x8009B574


class DsSyncTests(unittest.TestCase):
    @unittest.skipUnless((ROOT / 'assets/USA/main.exe').is_file() and
                         (ROOT / 'build/USA/main.exe').is_file() and find_spec('unicorn'),
                         'images or unicorn unavailable')
    def test_exact_bytes_and_indexed_reads(self):
        from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN
        from unicorn import mips_const as R

        offset = ENTRY - 0x8000F800
        images = [(ROOT / path).read_bytes()[offset:offset + 24]
                  for path in ('assets/USA/main.exe', 'build/USA/main.exe')]
        self.assertEqual(images[0], images[1])
        for image in images:
            machine = Uc(UC_ARCH_MIPS, UC_MODE_MIPS32 | UC_MODE_LITTLE_ENDIAN)
            machine.mem_map(0, 0x200000)
            machine.mem_write(ENTRY & 0x1FFFFFFF, image)
            before = bytes((i * 43 + 17) & 255 for i in range(1040))
            machine.mem_write((TABLE - 4) & 0x1FFFFFFF, before)
            for mode in tuple(range(256)) + (0x40000000, 0xFFFFFFFF):
                index = (mode << 2) & 0xFFFFFFFF
                address = (TABLE + index) & 0xFFFFFFFF
                expected = struct.unpack('<I', machine.mem_read(address & 0x1FFFFFFF, 4))[0]
                machine.reg_write(R.UC_MIPS_REG_A0, mode)
                machine.reg_write(R.UC_MIPS_REG_RA, 0x80010000)
                machine.reg_write(R.UC_MIPS_REG_SP, 0x801F0000)
                machine.emu_start(ENTRY, 0x80010000, count=20)
                self.assertEqual(machine.reg_read(R.UC_MIPS_REG_V0), expected, mode)
                self.assertEqual(machine.reg_read(R.UC_MIPS_REG_PC), 0x80010000, mode)
                self.assertEqual(machine.reg_read(R.UC_MIPS_REG_SP), 0x801F0000, mode)
            self.assertEqual(bytes(machine.mem_read((TABLE - 4) & 0x1FFFFFFF, 1040)), before)


if __name__ == '__main__':
    unittest.main()
