"""Timer counters and callback reloads in RAM; no physical memory-card emulation."""
from importlib.util import find_spec
from pathlib import Path
import itertools
import struct
import unittest

ROOT = Path(__file__).resolve().parents[2]
BASE, SIZE = 0x80082B08, 488
READY, TIMER, TAKE = BASE, 0x80082B70, 0x80082CDC
PAD, STEP = 0x80082E00, 0x80083014
ERROR, CALLBACK = 0x801E0100, 0x801E0200
G, T, IRQ, SIO0, SIO1 = 0x8009B720, 0x800A5AC0, 0x80103010, 0x80105010, 0x80105050
OBJECT0, OBJECT1 = 0x80104000, 0x80106000


def check_model(images):
    from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN, UC_HOOK_CODE
    from unicorn import mips_const as R

    saved = [getattr(R, 'UC_MIPS_REG_S' + str(i)) for i in range(8)] + [R.UC_MIPS_REG_FP]
    counters = (-2147483648, -1, 0, 149, 150, 151, 2147483647)
    ready_cases = list(itertools.product((0, 1, 2, 3, 0x80000000, 0xFFFFFFFF), repeat=2))
    timer_cases = list(itertools.product((-1, 0, 1, 2), (-1, 0, 1, 2), (0, 1),
                                        counters, counters, (0, 1), range(3)))
    take_cases = (0, 1, 2, -1, -2147483648, 2147483647, 0xABCD1234, 0x12340001)
    stop, stack = 0x80010000, 0x801F0000

    def signed(value):
        return value - 0x100000000 if value & 0x80000000 else value

    for label, code in images:
        machine = Uc(UC_ARCH_MIPS, UC_MODE_MIPS32 | UC_MODE_LITTLE_ENDIAN)
        machine.mem_map(0, 0x200000)
        def put(address, data):
            machine.mem_write(address & 0x1FFFFFFF, bytes(data))
        def read(address, size):
            return bytes(machine.mem_read(address & 0x1FFFFFFF, size))
        def actual_get(address, size=4):
            return int.from_bytes(read(address, size), 'little')
        def actual_set(address, value, size=4):
            put(address, (value & ((1 << (size * 8)) - 1)).to_bytes(size, 'little'))
        put(BASE, code)
        for address in (PAD, STEP, ERROR, CALLBACK):
            put(address, struct.pack('<II', 0x03E00008, 0))
        actual_calls = []
        coverage = set()
        mode, pad_result = 0, 1

        def environment(address, args, get, set_value, calls):
            calls.append((address, args))
            if address == PAD:
                if mode == 1:
                    set_value(G + 0x58, get(G + 0x44) - 1)
                return pad_result
            if address == ERROR:
                if mode == 1:
                    set_value(G + 0x58, get(G + 0x44))
                return 0
            if address == STEP:
                index = signed(get(G + 0x44))
                if mode == 2 and sum(a == STEP for a, _ in calls) == 1:
                    set_value(G + 0x58, index + 1)
                    set_value(G + 0x38, OBJECT1)
                    set_value(G + 0x68, SIO1)
                set_value(G + 0x44, index + 1)
                return 0
            set_value(G + 0x6C, 0xDEADBEEF)
            return 0

        def hook(uc, address, size, data):
            if address not in (PAD, STEP, ERROR, CALLBACK):
                return
            args = () if address == CALLBACK else (uc.reg_read(R.UC_MIPS_REG_A0),)
            result = environment(address, args, actual_get, actual_set, actual_calls)
            for name in ('AT', 'V0', 'V1', 'A0', 'A1', 'A2', 'A3',
                         'T0', 'T1', 'T2', 'T3', 'T4', 'T5', 'T6', 'T7', 'T8', 'T9'):
                uc.reg_write(getattr(R, 'UC_MIPS_REG_' + name), 0xCCCCCCCC)
            uc.reg_write(R.UC_MIPS_REG_V0, result & 0xFFFFFFFF)
        machine.hook_add(UC_HOOK_CODE, hook)

        def fixture():
            regions = [(G - 8, bytearray(b'\xA5' * 144)),
                       (T - 8, bytearray(b'\x5A' * 32)),
                       (IRQ - 8, bytearray(b'\x3C' * 32)),
                       (SIO0 - 8, bytearray(b'\x69' * 32)),
                       (SIO1 - 8, bytearray(b'\x96' * 32)),
                       (OBJECT0 - 256, bytearray(b'\x57' * 1024)),
                       (OBJECT1 - 256, bytearray(b'\x75' * 1024))]
            def get(address, size=4):
                for base, buffer in regions:
                    if base <= address and address + size <= base + len(buffer):
                        return int.from_bytes(buffer[address-base:address-base+size], 'little')
                raise AssertionError(hex(address))
            def set_value(address, value, size=4):
                for base, buffer in regions:
                    if base <= address and address + size <= base + len(buffer):
                        buffer[address-base:address-base+size] = (value & ((1 << (8*size))-1)).to_bytes(size, 'little')
                        return
                raise AssertionError(hex(address))
            for address, value in ((G + 4, ERROR), (G + 0x2C, CALLBACK),
                                   (G + 0x38, OBJECT0), (G + 0x64, IRQ), (G + 0x68, SIO0)):
                set_value(address, value)
            return regions, get, set_value

        def execute(entry, regions, expected_calls, result, context):
            actual_calls.clear()
            for name, value in (('SP', stack), ('GP', 0x8009CD70), ('RA', stop)):
                machine.reg_write(getattr(R, 'UC_MIPS_REG_' + name), value)
            for i, reg in enumerate(saved):
                machine.reg_write(reg, 0xABCD0000 + i)
            put(stack - 64, b'\xC3' * 96)
            machine.emu_start(entry, stop, count=10000)
            assert actual_calls == expected_calls, (label, context, actual_calls, expected_calls)
            for base, expected in regions:
                assert read(base, len(expected)) == expected, (label, context, hex(base))
            assert machine.reg_read(R.UC_MIPS_REG_V0) == result & 0xFFFFFFFF, (label, context)
            assert machine.reg_read(R.UC_MIPS_REG_SP) == stack
            assert machine.reg_read(R.UC_MIPS_REG_GP) == 0x8009CD70
            assert machine.reg_read(R.UC_MIPS_REG_PC) == stop
            assert read(stack - 64, 40) == b'\xC3' * 40
            assert read(stack, 32) == b'\xC3' * 32
            for i, reg in enumerate(saved):
                assert machine.reg_read(reg) == 0xABCD0000 + i

        for status, mask in ready_cases:
            for callback_present in (False, True):
                regions, get, set_value = fixture()
                set_value(IRQ, status); set_value(IRQ + 4, mask)
                set_value(G + 0x2C, CALLBACK if callback_present else 0)
                for base, buffer in regions: put(base, buffer)
                calls = []
                ready = bool(status & mask & 1)
                if ready and callback_present:
                    environment(CALLBACK, (), get, set_value, calls)
                execute(READY, regions, calls, int(ready), ('ready', status, mask, callback_present))

        for first, last, service, first_timer, second_timer, pad_result, mode in timer_cases:
            regions, get, set_value = fixture()
            for address, value in ((G + 0x54, first), (G + 0x58, last),
                                   (G + 0x3C, service), (T, first_timer), (T + 4, second_timer)):
                set_value(address, value)
            for base, buffer in regions: put(base, buffer)
            calls = []
            set_value(G + 0x6C, 1)
            if first != 0 and first_timer < 150:
                set_value(T, first_timer + 1); coverage.add('first timer')
            if last == 0 and second_timer < 150:
                set_value(T + 4, second_timer + 1); coverage.add('second timer')
            if service and last >= first:
                coverage.add('service')
                set_value(G + 0x48, 0); set_value(G + 0x44, first)
                result = environment(PAD, ((OBJECT0 + first * 240) & 0xFFFFFFFF,), get, set_value, calls)
                if result == 0:
                    coverage.add('error')
                    environment(ERROR, (65535,), get, set_value, calls)
                set_value(G + 0x4C, 0)
                while signed(get(G + 0x58)) >= signed(get(G + 0x44)):
                    index = signed(get(G + 0x44))
                    address = (get(G + 0x38) + index * 240) & 0xFFFFFFFF
                    environment(STEP, (address,), get, set_value, calls)
                    coverage.add('step')
                    assert len(calls) < 10
                set_value(get(G + 0x68) + 14, 0x88, 2)
            execute(TIMER, regions, calls, 0, ('timer', first, last, service, first_timer, second_timer, pad_result, mode))
        for pending in take_cases:
            regions, get, set_value = fixture()
            set_value(G + 0x6C, pending)
            for base, buffer in regions: put(base, buffer)
            set_value(G + 0x6C, 0)
            execute(TAKE, regions, [], pending, ('take', pending))
        assert coverage == {'first timer', 'second timer', 'service', 'error', 'step'}
    return len(ready_cases) * 2 + len(timer_cases) + len(take_cases)


class MemCardTimerTests(unittest.TestCase):
    @unittest.skipUnless((ROOT / 'assets/USA/main.exe').is_file() and
                         (ROOT / 'build/USA/main.exe').is_file() and find_spec('unicorn'),
                         'images or unicorn unavailable')
    def test_exact_bytes_and_model(self):
        images = [(name, (ROOT / name).read_bytes()[0x73308:0x73308 + SIZE])
                  for name in ('assets/USA/main.exe', 'build/USA/main.exe')]
        self.assertEqual(images[0][1], images[1][1])
        self.assertEqual(check_model(images), 9488)


if __name__ == '__main__':
    unittest.main()
