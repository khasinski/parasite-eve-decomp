"""Abort command state transitions and reloads after CD_flush."""
from pathlib import Path
import itertools
import random
import struct
import unittest

ROOT = Path(__file__).resolve().parents[2]


class CDAbortCmdTests(unittest.TestCase):
    def test_plain_source(self):
        source = (ROOT/'src/main/cdrom/CdRom_AbortCmd.c').read_text()
        self.assertNotRegex(source, r'\b(?:asm|__asm__)\b')

    @unittest.skipUnless((ROOT/'assets/USA/main.exe').is_file() and
                         (ROOT/'build/USA/main.exe').is_file(), 'images unavailable')
    def test_abort(self):
        try:
            from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN, UC_HOOK_CODE
            from unicorn import mips_const as R
        except ImportError:
            self.skipTest('unicorn unavailable')
        images = [(ROOT/p).read_bytes() for p in ('assets/USA/main.exe', 'build/USA/main.exe')]
        base, size = 0x800808BC, 0x74
        offset = base-0x8000F800
        self.assertEqual(images[0][offset:offset+size], images[1][offset:offset+size])
        start, flush, stop, stack = 0x8009B534, 0x8007B9EC, 0x80010000, 0x801F0000
        initial = random.Random(base).randbytes(0x90)
        saved = [getattr(R, f'UC_MIPS_REG_S{i}') for i in range(8)]+[R.UC_MIPS_REG_GP, R.UC_MIPS_REG_FP]
        commands = list(range(256))+[0x1000B, 0x10010, 0x10011, 0xFFFFFFFF]
        for image in images:
            m = Uc(UC_ARCH_MIPS, UC_MODE_MIPS32 | UC_MODE_LITTLE_ENDIAN)
            m.mem_map(0, 0x200000)
            def put(address, value): m.mem_write(address & 0x1FFFFFFF, bytes(value))
            def read(address, count): return bytes(m.mem_read(address & 0x1FFFFFFF, count))
            def word(address, value): put(address, struct.pack('<I', value))
            def set_expected(address, value):
                expected[address-start:address-start+4] = struct.pack('<I', value)
            put(0x8000F800, image)
            put(flush, struct.pack('<III', 0, 0x03E00008, 0))
            calls = []
            def hook(machine, pc, count, user):
                if pc != flush: return
                calls.append(pc)
                self.assertEqual(read(start, 0x90), bytes(expected), case)
                if mutate:
                    for address, value in ((0x8009B554, 0x12345678), (0x8009B574, status), (0x8009B578, command)):
                        word(address, value); set_expected(address, value)
                for name in ('V0', 'V1', 'A0', 'A1', 'A2', 'A3', 'T0', 'T1', 'T2', 'T3', 'T4', 'T5', 'T6', 'T7', 'T8', 'T9'):
                    machine.reg_write(getattr(R, 'UC_MIPS_REG_'+name), 0xBADC0000)
            m.hook_add(UC_HOOK_CODE, hook)
            for status, command, mutate in itertools.product((0, 1, 2, 0x10002, 0xFFFFFFFF), commands, (False, True)):
                case = (status, command, mutate)
                put(start, initial)
                word(0x8009B574, status ^ 3 if mutate else status)
                word(0x8009B578, command ^ 0xFF if mutate else command)
                expected = bytearray(read(start, 0x90))
                set_expected(0x8009B554, 0)
                calls.clear()
                for name, value in (('SP', stack), ('RA', stop)):
                    m.reg_write(getattr(R, 'UC_MIPS_REG_'+name), value)
                for i, reg in enumerate(saved): m.reg_write(reg, 0xABCD0000+i)
                m.emu_start(base, stop, count=150)
                if status == 2 and command in (11, 16, 17):
                    set_expected(0x8009B574, 1); set_expected(0x8009B578, 11)
                self.assertEqual(calls, [flush], case)
                self.assertEqual(read(start, 0x90), bytes(expected), case)
                self.assertEqual(m.reg_read(R.UC_MIPS_REG_PC), stop, case)
                self.assertEqual(m.reg_read(R.UC_MIPS_REG_SP), stack, case)
                for i, reg in enumerate(saved): self.assertEqual(m.reg_read(reg), 0xABCD0000+i, case)


if __name__ == '__main__': unittest.main()
