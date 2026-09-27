"""Event dispatch and global argument storage without register constraints."""
from pathlib import Path
import itertools
import random
import re
import struct
import unittest

ROOT = Path(__file__).resolve().parents[2]


class EventDeliverTests(unittest.TestCase):
    def test_plain_c(self):
        source = (ROOT / 'src/main/event/Evt_Deliver.c').read_text()
        source = re.sub(r'/\*.*?\*/|//[^\n]*', '', source, flags=re.S)
        self.assertIsNone(re.search(r'\b(?:asm|__asm__|INCLUDE_ASM|CC_POSTPASS|REGALLOC_BARRIER)\b', source))

    @unittest.skipUnless((ROOT / 'assets/USA/main.exe').is_file() and
                         (ROOT / 'build/USA/main.exe').is_file(), 'retail/build unavailable')
    def test_event_arguments_and_callbacks(self):
        try:
            from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN, UC_HOOK_CODE
            from unicorn import mips_const as R
        except ImportError:
            self.skipTest('unicorn unavailable')
        base, size = 0x800739C4, 0x64
        offset = base - 0x8000F800
        bodies = [(ROOT / p).read_bytes()[offset:offset+size] for p in ('assets/USA/main.exe', 'build/USA/main.exe')]
        self.assertEqual(len(bodies[0]), size)
        self.assertEqual(*bodies)
        start, stop, stack, callback = 0x80094544, 0x80010000, 0x801F0000, 0x80073A34
        saved = [getattr(R, f'UC_MIPS_REG_S{i}') for i in range(8)] + [R.UC_MIPS_REG_GP, R.UC_MIPS_REG_FP]
        initial = random.Random(base).randbytes(0x48)
        selectors = list(range(256)) + [0x10021, 0x10022, 0x7FFFFFFF, 0x80000000, 0xFFFFFFFF]
        for body in bodies:
            m = Uc(UC_ARCH_MIPS, UC_MODE_MIPS32 | UC_MODE_LITTLE_ENDIAN)
            m.mem_map(0, 0x200000)
            def put(a, data): m.mem_write(a & 0x1FFFFFFF, bytes(data))
            def read(a, n): return bytes(m.mem_read(a & 0x1FFFFFFF, n))
            put(base, body)
            put(callback, struct.pack('<III', 0, 0x03E00008, 0))
            events = []
            expected = bytearray(initial)
            mutate = False
            def hook(machine, pc, size, user):
                if pc != callback+4: return
                self.assertEqual(read(start, len(expected)), expected)
                events.append((machine.reg_read(R.UC_MIPS_REG_A0), machine.reg_read(R.UC_MIPS_REG_A1)))
                if mutate:
                    struct.pack_into('<II', expected, 0x20, 0xABCDEF01, 0x81234567)
                    put(start, expected)
                for r in ('V0','V1','A0','A1','A2','A3','T0','T1','T2','T3','T4','T5','T6','T7','T8','T9'):
                    machine.reg_write(getattr(R,'UC_MIPS_REG_'+r),0xDEADCAFE)
            m.hook_add(UC_HOOK_CODE, hook)
            for selector, argument, mutate in itertools.product(selectors, (0, 1, 0x7FFFFFFF, 0x80000000, 0xFFFFFFFF), (False, True)):
                expected = bytearray(initial)
                struct.pack_into('<II', expected, 0x20, selector, argument)
                put(start, initial)
                events.clear()
                for name, value in (('A0', selector), ('A1', argument), ('SP', stack), ('RA', stop)):
                    m.reg_write(getattr(R, 'UC_MIPS_REG_'+name), value)
                for i, reg in enumerate(saved): m.reg_write(reg, 0xABCD0000+i)
                m.emu_start(base, stop, count=100)
                case = (selector, argument, mutate)
                wanted = [(0xF4000002, 0x301 if selector == 0x21 else 0x302)] if selector in (0x21,0x22) else []
                self.assertEqual(events, wanted, case)
                self.assertEqual(read(start, len(expected)), expected, case)
                self.assertEqual(m.reg_read(R.UC_MIPS_REG_V0), 0, case)
                self.assertEqual(m.reg_read(R.UC_MIPS_REG_PC), stop, case)
                self.assertEqual(m.reg_read(R.UC_MIPS_REG_SP), stack, case)
                for i, reg in enumerate(saved): self.assertEqual(m.reg_read(reg), 0xABCD0000+i, case)


if __name__ == '__main__': unittest.main()
