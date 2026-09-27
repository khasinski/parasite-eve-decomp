"""Ready-event dispatch: arguments, post-decoder state and callback mutation."""
from pathlib import Path
import itertools
import random
import struct
import unittest

ROOT = Path(__file__).resolve().parents[2]


class CDReadyDispatchTests(unittest.TestCase):
    def test_plain_source(self):
        source = (ROOT/'src/main/cdrom/CdRom_ReadyEventDispatch.c').read_text()
        self.assertNotRegex(source, r'\b(?:asm|__asm__)\b')
        self.assertIn('CdRom_ProcessEventByte(event_reg, data_reg);', source)

    @unittest.skipUnless((ROOT/'assets/USA/main.exe').is_file() and
                         (ROOT/'build/USA/main.exe').is_file(), 'images unavailable')
    def test_dispatch(self):
        try:
            from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN, UC_HOOK_CODE
            from unicorn import mips_const as R
        except ImportError:
            self.skipTest('unicorn unavailable')
        images = [(ROOT/p).read_bytes() for p in ('assets/USA/main.exe', 'build/USA/main.exe')]
        base, size = 0x80080778, 0x94
        offset = base-0x8000F800
        self.assertEqual(images[0][offset:offset+size], images[1][offset:offset+size])
        state, data, decoder, callback = 0x8009B534, 0x80100020, 0x8008080C, 0x80010040
        stop, stack = 0x80010000, 0x801F0000
        saved = [getattr(R, f'UC_MIPS_REG_S{i}') for i in range(8)]+[R.UC_MIPS_REG_GP, R.UC_MIPS_REG_FP]
        initial = random.Random(base).randbytes(0x90)
        events = list(range(256))+[0x105, 0x12345678, 0xFFFFFFFF]
        for image in images:
            m = Uc(UC_ARCH_MIPS, UC_MODE_MIPS32 | UC_MODE_LITTLE_ENDIAN)
            m.mem_map(0, 0x200000)
            def put(address, value): m.mem_write(address & 0x1FFFFFFF, bytes(value))
            def read(address, count): return bytes(m.mem_read(address & 0x1FFFFFFF, count))
            def word(address, value): put(address, struct.pack('<I', value))
            put(0x8000F800, image)
            for address in (decoder, callback):
                put(address, struct.pack('<III', 0, 0x03E00008, 0))
            calls = []
            expected = bytearray()
            def set_expected(address, value, width=4):
                expected[address-state:address-state+width] = value.to_bytes(width, 'little')
            def clobber():
                for name in ('V0', 'V1', 'A0', 'A1', 'A2', 'A3', 'T0', 'T1', 'T2', 'T3', 'T4', 'T5', 'T6', 'T7', 'T8', 'T9'):
                    m.reg_write(getattr(R, 'UC_MIPS_REG_'+name), 0xBADC0000)
            def hook(machine, pc, count, user):
                if pc not in (decoder+4, callback+4): return
                args = (machine.reg_read(R.UC_MIPS_REG_A0), machine.reg_read(R.UC_MIPS_REG_A1))
                self.assertEqual(args, (event & 255, data), case)
                calls.append(pc)
                if pc == decoder+4:
                    # The decoder can change both event status and enable state.
                    put(0x8009B56C, bytes([flag])); word(0x8009B554, enabled)
                    set_expected(0x8009B56C, flag, 1); set_expected(0x8009B554, enabled)
                    word(0x800A36A8, callback if present else 0)
                    if flag & 0x10:
                        set_expected(0x8009B574, 2); set_expected(0x8009B578, 12)
                else:
                    self.assertEqual(read(state, len(expected)), bytes(expected), case)
                    word(0x8009B574, 0xDEADBEEF); set_expected(0x8009B574, 0xDEADBEEF)
                    word(0x8009B554, 0); set_expected(0x8009B554, 0)
                    put(data, b'\x99')
                clobber()
            m.hook_add(UC_HOOK_CODE, hook)
            for event, flag, enabled, present in itertools.product(events, (0, 0x10, 0xFF), (0, 1, 0xFFFFFFFF), (False, True)):
                case = (event, flag, enabled, present)
                put(state, initial)
                put(data-0x20, b'\xA5'*0x50)
                word(0x800A36A8, 0 if present else callback)
                expected = bytearray(initial)
                calls.clear()
                for name, value in (('A0', event), ('A1', data), ('SP', stack), ('RA', stop)):
                    m.reg_write(getattr(R, 'UC_MIPS_REG_'+name), value)
                for i, reg in enumerate(saved): m.reg_write(reg, 0xABCD0000+i)
                m.emu_start(base, stop, count=200)
                fired = bool(enabled and present)
                self.assertEqual(calls, [decoder+4]+([callback+4] if fired else []), case)
                self.assertEqual(read(state, len(expected)), bytes(expected), case)
                buffer = bytearray(b'\xA5'*0x50)
                if fired: buffer[0x20] = 0x99
                self.assertEqual(read(data-0x20, 0x50), bytes(buffer), case)
                self.assertEqual(read(0x800A36A8, 4), struct.pack('<I', callback if present else 0), case)
                self.assertEqual(m.reg_read(R.UC_MIPS_REG_PC), stop, case)
                self.assertEqual(m.reg_read(R.UC_MIPS_REG_SP), stack, case)
                for i, reg in enumerate(saved): self.assertEqual(m.reg_read(reg), 0xABCD0000+i, case)


if __name__ == '__main__': unittest.main()
