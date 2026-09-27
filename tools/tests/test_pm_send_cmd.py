"""Pm_SendCmd callback reload, six arguments, bounds and ABI model."""
from pathlib import Path
from importlib.util import find_spec
import itertools
import struct
import unittest

ROOT = Path(__file__).resolve().parents[2]
ENTRY, STOP, STACK = 0x8006F6D4, 0x80010000, 0x801F0000
TABLE_SLOT, PRIMARY_SLOT, SECONDARY_SLOT = 0x800942E0, 0x800942E4, 0x800942E8
TABLE, ALT, HANDLER, ALT_HANDLER = 0x80180000, 0x80180400, 0x80182000, 0x80182100
CALL, ALT_CALL, PRIMARY, SECONDARY, OUT = 0x80185000, 0x80185010, 0x80190000, 0x801A0000, 0x801B0000


def check_model(images):
    from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN, UC_HOOK_CODE
    from unicorn import mips_const as R
    cases = list(itertools.product((-2147483648, -1, 0, 10, 11, 21, 22),
                                  (0, 1, 84, 85, 191, 192, 255), range(3),
                                  ((0, 0), (1, 0), (1, 1), (2, 0)), (False, True)))
    for label, code in images:
        machine = Uc(UC_ARCH_MIPS, UC_MODE_MIPS32 | UC_MODE_LITTLE_ENDIAN)
        machine.mem_map(0, 0x200000)
        def put(address, data): machine.mem_write(address & 0x1FFFFFFF, bytes(data))
        def read(address, length): return bytes(machine.mem_read(address & 0x1FFFFFFF, length))
        def word(value): return struct.pack('<I', value & 0xFFFFFFFF)
        put(ENTRY, code)
        stub = struct.pack('<II', 0x03E00008, 0)
        put(CALL, stub); put(ALT_CALL, stub)
        events = []
        def hook(uc, address, size, data):
            if address not in (CALL, ALT_CALL): return
            args = tuple(uc.reg_read(getattr(R, 'UC_MIPS_REG_A' + str(i))) for i in range(4))
            args += struct.unpack('<II', read(uc.reg_read(R.UC_MIPS_REG_SP) + 16, 8))
            events.append((address, args, read(OUT - 8, 32), read(TABLE_SLOT, 4)))
            for name in ('AT', 'V0', 'V1', 'A0', 'A1', 'A2', 'A3', 'T0', 'T1', 'T2', 'T3', 'T4', 'T5', 'T6', 'T7', 'T8', 'T9'):
                uc.reg_write(getattr(R, 'UC_MIPS_REG_' + name), 0xCCCCCCCC)
            uc.reg_write(R.UC_MIPS_REG_V0, 0x13579BDF if address == CALL else 0xCAFEFFFF)
        machine.hook_add(UC_HOOK_CODE, hook)
        for slot, command, handler_kind, (mode, operation), alias in cases:
            context = (label, slot, command, handler_kind, mode, operation, alias)
            valid = 0 <= slot < 22
            entry = PRIMARY + slot * 2572 if slot < 11 else SECONDARY + (slot - 11) * 268
            slot_data = bytearray(b'\xA5' * 80)
            slot_data[9] = command
            slot_data[10] = 37; slot_data[11] = 99
            struct.pack_into('<I', slot_data, 12, ALT if alias else 0x12345678)
            if valid: put(entry - 8, slot_data)
            put(TABLE_SLOT, word(TABLE)); put(PRIMARY_SLOT, word(PRIMARY)); put(SECONDARY_SLOT, word(SECONDARY))
            put(TABLE, word(0 if handler_kind == 0 else HANDLER) * 86)
            put(ALT, word(ALT_HANDLER) * 86)
            put(HANDLER, word(0) * 2 + word(0 if handler_kind == 1 else CALL) + word(0))
            put(ALT_HANDLER, word(0) * 2 + word(ALT_CALL) + word(0))
            output = bytearray(b'\x5A' * 32)
            put(OUT - 8, output)
            third, fourth, fifth = OUT, OUT + 4, TABLE_SLOT if alias else OUT + 8
            expected_table = TABLE
            expected_events = []
            if not valid: result = -10
            elif command >= 192: result = -11
            elif handler_kind == 0: result = -12
            elif handler_kind == 1: result = -1
            else:
                if mode == 1 and operation == 0:
                    struct.pack_into('<I', output, 8, 37)
                    struct.pack_into('<I', output, 12, 99)
                    if alias: expected_table = ALT
                    else: struct.pack_into('<I', output, 16, 0x12345678)
                callback = ALT_CALL if expected_table == ALT else CALL
                result = 0xCAFEFFFF if callback == ALT_CALL else 0x13579BDF
                expected_events = [(callback, (entry, mode, operation, third, fourth, fifth), bytes(output), word(expected_table))]
            events.clear()
            put(STACK - 96, b'\xC3' * 128)
            put(STACK + 16, word(fourth) + word(fifth))
            for name, value in (('A0', slot), ('A1', mode), ('A2', operation), ('A3', third), ('SP', STACK), ('GP', 0x8009CD70), ('RA', STOP)):
                machine.reg_write(getattr(R, 'UC_MIPS_REG_' + name), value & 0xFFFFFFFF)
            saved = [getattr(R, 'UC_MIPS_REG_S' + str(i)) for i in range(8)] + [R.UC_MIPS_REG_FP]
            for i, reg in enumerate(saved): machine.reg_write(reg, 0xABCD0000 + i)
            machine.emu_start(ENTRY, STOP, count=1000)
            assert machine.reg_read(R.UC_MIPS_REG_V0) == result & 0xFFFFFFFF, context
            assert events == expected_events, (context, events, expected_events)
            assert read(OUT - 8, 32) == output, context
            assert read(TABLE_SLOT, 4) == word(expected_table), context
            assert read(PRIMARY_SLOT, 8) == word(PRIMARY) + word(SECONDARY), context
            if valid: assert read(entry - 8, 80) == slot_data, context
            assert read(STACK - 96, 64) == b'\xC3' * 64, context
            assert read(STACK, 16) == b'\xC3' * 16, context
            assert read(STACK + 16, 8) == word(fourth) + word(fifth), context
            for name, value in (('PC', STOP), ('SP', STACK), ('GP', 0x8009CD70)):
                assert machine.reg_read(getattr(R, 'UC_MIPS_REG_' + name)) == value, context
            for i, reg in enumerate(saved): assert machine.reg_read(reg) == 0xABCD0000 + i, context
    return len(cases)


class PmSendCmdTests(unittest.TestCase):
    @unittest.skipUnless((ROOT / 'assets/USA/main.exe').is_file() and
                         (ROOT / 'build/USA/main.exe').is_file() and find_spec('unicorn'),
                         'images or unicorn unavailable')
    def test_exact_bytes_and_model(self):
        images = [(name, (ROOT / name).read_bytes()[0x5FED4:0x5FED4 + 332])
                  for name in ('assets/USA/main.exe', 'build/USA/main.exe')]
        self.assertEqual(images[0][1], images[1][1])
        self.assertEqual(check_model(images), 1176)


if __name__ == '__main__':
    unittest.main()
