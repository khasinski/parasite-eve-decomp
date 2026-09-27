"""Read-done callback control flow and callback-driven state changes."""
from pathlib import Path
import itertools
import struct
import unittest

ROOT = Path(__file__).resolve().parents[2]


class CDReadDoneTests(unittest.TestCase):
    def test_plain_source(self):
        self.assertNotRegex((ROOT/'src/main/cdrom/CdRom_ReadDoneCallback.c').read_text(), r'\b(?:asm|__asm__)\b')

    @unittest.skipUnless((ROOT/'assets/USA/main.exe').is_file() and
                         (ROOT/'build/USA/main.exe').is_file(), 'images unavailable')
    def test_callbacks(self):
        try:
            from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN, UC_HOOK_CODE
            from unicorn import mips_const as R
        except ImportError:
            self.skipTest('unicorn unavailable')
        images = [(ROOT/p).read_bytes() for p in ('assets/USA/main.exe', 'build/USA/main.exe')]
        base, size = 0x8008214C, 0xB8
        offset = base-0x8000F800
        self.assertEqual(images[0][offset:offset+size], images[1][offset:offset+size])
        start, data, stop, stack = 0x8009B6DC, 0x80100020, 0x80010000, 0x801F0000
        pending, restart, sync, ready, callback = 0x8007F778, 0x80082204, 0x800824C8, 0x800824DC, 0x80010040
        saved = [getattr(R, f'UC_MIPS_REG_S{i}') for i in range(8)]+[R.UC_MIPS_REG_GP, R.UC_MIPS_REG_FP]
        events = list(range(256))+[0x102, 0x105, 0xFFFFFFFF]
        for image in images:
            m = Uc(UC_ARCH_MIPS, UC_MODE_MIPS32 | UC_MODE_LITTLE_ENDIAN)
            m.mem_map(0, 0x200000)
            def put(a, v): m.mem_write(a & 0x1FFFFFFF, bytes(v))
            def read(a, n): return bytes(m.mem_read(a & 0x1FFFFFFF, n))
            def word(a, v): put(a, struct.pack('<I', v))
            def reg(name): return m.reg_read(getattr(R, 'UC_MIPS_REG_'+name))
            def change(a, v):
                word(a, v); expected[a-start:a-start+4] = struct.pack('<I', v)
            put(0x8000F800, image)
            for address in (pending, restart, sync, ready, callback):
                put(address, struct.pack('<III', 0, 0x03E00008, 0))
            calls = []
            def hook(machine, pc, count, user):
                if pc not in (pending, restart, sync, ready, callback): return
                calls.append(pc)
                if pc == callback:
                    expected[0x30:0x34] = b'\0'*4
                self.assertEqual(read(start, 0x60), bytes(expected), case)
                result = 0xDEADBEEF
                if pc == pending:
                    result = count_value
                    if mutate: change(0x8009B70C, 0)
                elif pc == restart:
                    change(0x8009B708, 0x77889900)
                elif pc == sync:
                    self.assertEqual(reg('A0'), 0x80011000, case)
                    if mutate: change(0x8009B704, 0x80012020)
                elif pc == ready:
                    self.assertEqual(reg('A0'), 0x80012020 if mutate else 0x80012000, case)
                    if mutate: change(0x8009B6F4, 0 if present else callback)
                else:
                    self.assertEqual(tuple(reg(r) for r in ('A0', 'A1', 'A2')), (event & 255, data, 0), case)
                    change(0x8009B70C, 7)
                for name in ('V0', 'V1', 'A0', 'A1', 'A2', 'A3', 'T0', 'T1', 'T2', 'T3', 'T4', 'T5', 'T6', 'T7', 'T8', 'T9'):
                    machine.reg_write(getattr(R, 'UC_MIPS_REG_'+name), 0xBADC0000)
                machine.reg_write(R.UC_MIPS_REG_V0, result)
            m.hook_add(UC_HOOK_CODE, hook)
            for event, enabled, active, count_value, present, mutate in itertools.product(events, (0, 1), (0, 1), (0, 3), (False, True), (False, True)):
                case = (event, enabled, active, count_value, present, mutate)
                put(start, b'\xA5'*0x60); put(data-0x20, b'\x5A'*0x50)
                for a, v in ((0x8009B708, enabled), (0x8009B70C, active), (0x8009B700, 0x80011000), (0x8009B704, 0x80012000), (0x8009B6F4, callback if present else 0)):
                    word(a, v)
                expected = bytearray(read(start, 0x60)); calls.clear()
                for name, value in (('A0', event), ('A1', data), ('SP', stack), ('RA', stop)):
                    m.reg_write(getattr(R, 'UC_MIPS_REG_'+name), value)
                for i, r in enumerate(saved): m.reg_write(r, 0xABCD0000+i)
                m.emu_start(base, stop, count=200)
                wanted = []
                if enabled and active:
                    if event & 255 == 2:
                        wanted = [pending]+([restart] if count_value == 0 else [])
                    else:
                        fired = present != mutate
                        wanted = [sync, ready]+([callback] if fired else [])
                        if not fired: expected[0x30:0x34] = b'\0'*4
                self.assertEqual(calls, wanted, case)
                self.assertEqual(read(start, 0x60), bytes(expected), case)
                self.assertEqual(read(data-0x20, 0x50), b'\x5A'*0x50, case)
                self.assertEqual(reg('PC'), stop, case); self.assertEqual(reg('SP'), stack, case)
                for i, r in enumerate(saved): self.assertEqual(m.reg_read(r), 0xABCD0000+i, case)


if __name__ == '__main__': unittest.main()
