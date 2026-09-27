"""RAM callback model of card acknowledgement and retail result mapping."""
from importlib.util import find_spec
from pathlib import Path
import itertools
import struct
import unittest

ROOT = Path(__file__).resolve().parents[2]
ENTRY, OFFSET, SIZE = 0x800840DC, 0x748DC, 140
CALLBACK, WRITE, SLOT = 0x80180000, 0x800832B4, 0x8009B72C
CARD, HEADER, STOP, STACK = 0x80181000, 0x80182000, 0x80010000, 0x801F0000


def check_model(images):
    from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN, UC_HOOK_CODE
    from unicorn import mips_const as R
    saved = [getattr(R, 'UC_MIPS_REG_S' + str(i)) for i in range(8)] + [R.UC_MIPS_REG_FP]
    results = (-2147483648, -90, -9, -1, 0, 1, 89, 90, 91, 2147483647)
    states = list(itertools.product(range(16), (0, 1, 255),
                                   (0, 0x12345678, 0x80000000, 0xFFFFFFFF), results))
    for label, code in images:
        machine = Uc(UC_ARCH_MIPS, UC_MODE_MIPS32 | UC_MODE_LITTLE_ENDIAN)
        machine.mem_map(0, 0x200000)
        def put(address, data):
            machine.mem_write(address & 0x1FFFFFFF, bytes(data))
        def read(address, size):
            return bytes(machine.mem_read(address & 0x1FFFFFFF, size))
        put(ENTRY, code)
        stub = struct.pack('<II', 0x03E00008, 0)
        put(CALLBACK, stub)
        put(WRITE, stub)
        put(SLOT, struct.pack('<I', CALLBACK))
        calls = []
        callback_value = result_value = 0
        def hook(uc, address, size, data):
            if address not in (CALLBACK, WRITE):
                return
            calls.append((address, uc.reg_read(R.UC_MIPS_REG_A0), uc.reg_read(R.UC_MIPS_REG_A1)))
            for name in ('AT', 'V0', 'V1', 'A0', 'A1', 'A2', 'A3',
                         'T0', 'T1', 'T2', 'T3', 'T4', 'T5', 'T6', 'T7', 'T8', 'T9'):
                uc.reg_write(getattr(R, 'UC_MIPS_REG_' + name), 0xCCCCCCCC)
            uc.reg_write(R.UC_MIPS_REG_V0,
                         callback_value if address == CALLBACK else result_value & 0xFFFFFFFF)
        machine.hook_add(UC_HOOK_CODE, hook)
        for kind, ack, callback_value, result_value in states:
            context = (label, kind, ack, callback_value, result_value)
            card = bytearray(b'\xA5' * 80)
            card[8 + 0x36] = ack
            struct.pack_into('<I', card, 8 + 0x3C, HEADER)
            header = b'\x5A' * 8 + bytes([(kind << 4) | 7]) + b'\x5A' * 8
            put(CARD - 8, card)
            put(HEADER - 8, header)
            put(STACK - 64, b'\xC3' * 96)
            calls.clear()
            for name, value in (('A0', CARD), ('SP', STACK), ('GP', 0x8009CD70), ('RA', STOP)):
                machine.reg_write(getattr(R, 'UC_MIPS_REG_' + name), value)
            for i, reg in enumerate(saved):
                machine.reg_write(reg, 0xABCD0000 + i)
            machine.emu_start(ENTRY, STOP, count=200)
            expected = result_value if result_value <= 0 or result_value == 90 else -9
            assert machine.reg_read(R.UC_MIPS_REG_V0) == expected & 0xFFFFFFFF, context
            assert calls == [(CALLBACK, CARD, int(kind == 8 and ack == 0)),
                             (WRITE, CARD, callback_value & 255)], context
            assert read(CARD - 8, len(card)) == card, context
            assert read(HEADER - 8, len(header)) == header, context
            assert read(STACK - 64, 40) == b'\xC3' * 40, context
            assert read(STACK, 32) == b'\xC3' * 32, context
            for name, value in (('PC', STOP), ('SP', STACK), ('GP', 0x8009CD70)):
                assert machine.reg_read(getattr(R, 'UC_MIPS_REG_' + name)) == value, context
            for i, reg in enumerate(saved):
                assert machine.reg_read(reg) == 0xABCD0000 + i, context
    return len(states)


class MemCardAckTests(unittest.TestCase):
    @unittest.skipUnless((ROOT / 'assets/USA/main.exe').is_file() and
                         (ROOT / 'build/USA/main.exe').is_file() and find_spec('unicorn'),
                         'images or unicorn unavailable')
    def test_exact_bytes_and_model(self):
        images = [(name, (ROOT / name).read_bytes()[OFFSET:OFFSET + SIZE])
                  for name in ('assets/USA/main.exe', 'build/USA/main.exe')]
        self.assertEqual(images[0][1], images[1][1])
        self.assertEqual(check_model(images), 1920)


if __name__ == '__main__':
    unittest.main()
