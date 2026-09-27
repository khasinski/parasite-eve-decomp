"""Draw-state snapshot and signed TIM-table indices; RAM callback model only."""
from importlib.util import find_spec
from pathlib import Path
import itertools
import struct
import unittest

ROOT = Path(__file__).resolve().parents[2]
BASE, SIZE = 0x8006EC08, 228
STATUS, ENTRY, LOAD, IMAGE = BASE, 0x8006EC6C, 0x8006EC84, 0x800718D0
GLOBAL, TABLE, STOP, STACK = 0x800B0DBA, 0x80140010, 0x80010000, 0x801F0000


def check_model(images):
    from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN, UC_HOOK_CODE
    from unicorn import mips_const as R
    saved = [getattr(R, 'UC_MIPS_REG_S' + str(i)) for i in range(8)] + [R.UC_MIPS_REG_FP]
    states = list(itertools.product((-128, -1, 0, 1, 127), (-128, -1, 0, 1, 127),
                                   (0, 1, 32767, 32768, 65535)))
    indices = (-65537, -32769, -32768, -1, 0, 1, 32767, 32768, 65535, 65536,
               -2147483648, 2147483647, 0x12340001)
    offsets = (-2147483648, -1, 0, 1, 2147483647, 0x40000)
    counts = (-2147483648, -1, 0, 1, 2, 3, 32767, 32768, 32769, 65535, 65536, 65537)
    start = TABLE - 131072 - 8
    table_size = 262144 + 16
    verified = 0

    def narrow(index):
        low = index & 65535
        return low - 65536 if low & 32768 else low

    for label, code in images:
        machine = Uc(UC_ARCH_MIPS, UC_MODE_MIPS32 | UC_MODE_LITTLE_ENDIAN)
        machine.mem_map(0, 0x200000)
        def put(address, data):
            machine.mem_write(address & 0x1FFFFFFF, bytes(data))
        def read(address, size):
            return bytes(machine.mem_read(address & 0x1FFFFFFF, size))
        put(BASE, code)
        put(IMAGE, struct.pack('<II', 0x03E00008, 0))
        calls = []
        mutation = False
        expected = None

        def changed_offset(iteration):
            return (0x80000000 + iteration * 17) & 0xFFFFFFFF

        def hook(uc, address, size, data):
            if address != IMAGE:
                return
            iteration = len(calls)
            calls.append(uc.reg_read(R.UC_MIPS_REG_A0))
            if mutation:
                position = TABLE + narrow(iteration + 1) * 4
                value = changed_offset(iteration)
                put(position, struct.pack('<I', value))
            for name in ('AT', 'V0', 'V1', 'A0', 'A1', 'A2', 'A3',
                         'T0', 'T1', 'T2', 'T3', 'T4', 'T5', 'T6', 'T7', 'T8', 'T9'):
                uc.reg_write(getattr(R, 'UC_MIPS_REG_' + name), 0xCCCCCCCC)
        machine.hook_add(UC_HOOK_CODE, hook)

        def execute(entry, args, limit):
            calls.clear()
            for name, value in (('A0', args[0] if args else 0),
                                ('A1', args[1] if len(args) > 1 else 0),
                                ('SP', STACK), ('GP', 0x8009CD70), ('RA', STOP)):
                machine.reg_write(getattr(R, 'UC_MIPS_REG_' + name), value & 0xFFFFFFFF)
            for i, reg in enumerate(saved):
                machine.reg_write(reg, 0xABCD0000 + i)
            put(STACK - 80, b'\xC3' * 112)
            machine.emu_start(entry, STOP, count=limit)
            assert machine.reg_read(R.UC_MIPS_REG_SP) == STACK, label
            assert machine.reg_read(R.UC_MIPS_REG_GP) == 0x8009CD70, label
            assert machine.reg_read(R.UC_MIPS_REG_PC) == STOP, label
            assert read(STACK - 80, 40) == b'\xC3' * 40, label
            assert read(STACK, 32) == b'\xC3' * 32, label
            for i, reg in enumerate(saved):
                assert machine.reg_read(reg) == 0xABCD0000 + i, label

        for enabled, flag, elapsed in states:
            guard = b'\x5A' * 8 + struct.pack('<bbH', enabled, flag, elapsed) + b'\x5A' * 8
            put(GLOBAL - 8, guard)
            execute(STATUS, (), 1000)
            result = (2 if flag != 0 else 1) if enabled != 0 and 0 < elapsed < 32768 else 0
            assert machine.reg_read(R.UC_MIPS_REG_V0) == result, (label, enabled, flag, elapsed)
            assert read(GLOBAL - 8, len(guard)) == guard, label
            assert read(STACK - 80, 112) == b'\xC3' * 112, label
            assert calls == []
        for index, offset in itertools.product(indices, offsets):
            expected = bytearray(b'\xA5' * table_size)
            at = 8 + (narrow(index) + 32768) * 4
            struct.pack_into('<I', expected, at, offset & 0xFFFFFFFF)
            put(start, expected)
            execute(ENTRY, (TABLE, index), 1000)
            assert machine.reg_read(R.UC_MIPS_REG_V0) == (TABLE + offset) & 0xFFFFFFFF, (label, index, offset)
            assert read(start, len(expected)) == expected
            assert calls == []
        for count, mutation in itertools.product(counts, (False, True)):
            expected = bytearray(b'\xA5' * table_size)
            for index in range(-32768, 32768):
                struct.pack_into('<I', expected, 8 + (index + 32768) * 4,
                                 (index * 37) & 0xFFFFFFFF)
            put(start, expected)
            expected_calls = []
            for iteration in range(max(0, count)):
                at = 8 + (narrow(iteration) + 32768) * 4
                offset = struct.unpack_from('<I', expected, at)[0]
                expected_calls.append((TABLE + offset) & 0xFFFFFFFF)
                if mutation:
                    next_at = 8 + (narrow(iteration + 1) + 32768) * 4
                    struct.pack_into('<I', expected, next_at, changed_offset(iteration))
            execute(LOAD, (TABLE, count), max(1000, max(0, count) * 32 + 100))
            assert calls == expected_calls, (label, count, mutation)
            assert read(start, len(expected)) == expected, (label, count, mutation)
        verified = len(states) + len(indices) * len(offsets) + len(counts) * 2
    return verified


class GpuStatusTests(unittest.TestCase):
    @unittest.skipUnless((ROOT / 'assets/USA/main.exe').is_file() and
                         (ROOT / 'build/USA/main.exe').is_file() and find_spec('unicorn'),
                         'images or unicorn unavailable')
    def test_exact_bytes_and_model(self):
        images = [(name, (ROOT / name).read_bytes()[0x5F408:0x5F408 + SIZE])
                  for name in ('assets/USA/main.exe', 'build/USA/main.exe')]
        self.assertEqual(images[0][1], images[1][1])
        self.assertEqual(check_model(images), 227)


if __name__ == '__main__':
    unittest.main()
