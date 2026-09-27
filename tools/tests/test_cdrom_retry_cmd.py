"""The retry callback keeps its command lookup and timeout semantics."""
from importlib.util import find_spec
from pathlib import Path
import struct
import unittest

ROOT = Path(__file__).resolve().parents[2]
ENTRY = 0x800800F4
IMAGE_BASE = 0x8000F800
TABLE = 0x8009B5A4
STATE = 0x8009B534
FLUSH = 0x8007B9EC
SEND = 0x8007B558


class CdRomRetryCmdTests(unittest.TestCase):
    @unittest.skipUnless((ROOT / 'assets/USA/main.exe').is_file() and
                         (ROOT / 'build/USA/main.exe').is_file() and find_spec('unicorn'),
                         'images or unicorn unavailable')
    def test_exact_bytes_and_callback_state(self):
        from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN, UC_HOOK_CODE
        from unicorn import mips_const as R

        images = [(ROOT / path).read_bytes()[ENTRY - IMAGE_BASE:ENTRY - IMAGE_BASE + 112]
                  for path in ('assets/USA/main.exe', 'build/USA/main.exe')]
        self.assertEqual(images[0], images[1])
        symbols = (ROOT / 'build/USA/main.map').read_text()
        self.assertIn('g_CdRomCmdLongTimeoutTable', symbols)

        for image in images:
            machine = Uc(UC_ARCH_MIPS, UC_MODE_MIPS32 | UC_MODE_LITTLE_ENDIAN)
            machine.mem_map(0, 0x200000)
            def put(address, data):
                machine.mem_write(address & 0x1FFFFFFF, bytes(data))
            def get(address, size):
                return bytes(machine.mem_read(address & 0x1FFFFFFF, size))
            def word(address, value):
                put(address, struct.pack('<I', value & 0xFFFFFFFF))

            put(ENTRY, image)
            for address in (FLUSH, SEND):
                put(address, struct.pack('<III', 0, 0x03E00008, 0))
            calls = []
            def callback(uc, address, size, user_data):
                if address == FLUSH:
                    calls.append('flush')
                elif address == SEND:
                    calls.append('send')
                    self.assertEqual(tuple(uc.reg_read(getattr(R, 'UC_MIPS_REG_A' + str(i)))
                                           for i in range(4)), (command, 0x80101000, 0, 1))
                else:
                    return
                for name in ('V0', 'V1', 'A0', 'A1', 'A2', 'A3', 'T0', 'T1'):
                    uc.reg_write(getattr(R, 'UC_MIPS_REG_' + name), 0xBADC0000)
            machine.hook_add(UC_HOOK_CODE, callback)

            for command in (0, 1, 127, 255):
                for timeout in (0, 1, 0xFFFFFFFF):
                    for count in (0, 0x7FFFFFFF, 0xFFFFFFFF):
                        calls.clear()
                        put(STATE, b'\xA5' * 0x70)
                        put(TABLE, b'\x5A' * 1024)
                        word(TABLE + command * 4, timeout)
                        put(0x8009B558, bytes([command]))
                        word(0x8009B59C, count)
                        word(0x8009B560, 0x80101000)
                        machine.reg_write(R.UC_MIPS_REG_SP, 0x801F0000)
                        machine.reg_write(R.UC_MIPS_REG_RA, 0x80010000)
                        machine.emu_start(ENTRY, 0x80010000, count=150)
                        self.assertEqual(calls, ['flush', 'send'])
                        self.assertEqual(struct.unpack('<I', get(0x8009B59C, 4))[0],
                                         (count + 1) & 0xFFFFFFFF)
                        self.assertEqual(struct.unpack('<I', get(0x8009B598, 4))[0],
                                         960 if timeout else 30)
                        self.assertEqual(machine.reg_read(R.UC_MIPS_REG_V0), 0)


if __name__ == '__main__':
    unittest.main()
