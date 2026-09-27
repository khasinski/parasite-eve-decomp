"""Read-progress branches, timeout boundary and mutating callbacks."""
from pathlib import Path
import itertools
import struct
import unittest

ROOT = Path(__file__).resolve().parents[2]


class CDReadProgressTests(unittest.TestCase):
    def test_plain_source(self):
        self.assertNotRegex((ROOT/'src/main/cdrom/CdRom_ReadProgressCallback.c').read_text(), r'\b(?:asm|__asm__)\b')

    @unittest.skipUnless((ROOT/'assets/USA/main.exe').is_file() and
                         (ROOT/'build/USA/main.exe').is_file(), 'images unavailable')
    def test_progress(self):
        try:
            from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN, UC_HOOK_CODE
            from unicorn import mips_const as R
        except ImportError:
            self.skipTest('unicorn unavailable')
        images = [(ROOT/p).read_bytes() for p in ('assets/USA/main.exe', 'build/USA/main.exe')]
        base, size = 0x80080F98, 0x190
        offset = base-0x8000F800
        self.assertEqual(images[0][offset:offset+size], images[1][offset:offset+size])
        start, data, stop, stack = 0x8009B694, 0x80100020, 0x80010000, 0x801F0000
        vsync, busy, busy2, save, complete = 0x80073A44, 0x80080AE4, 0x80080B04, 0x80081268, 0x80010040
        sector, dest, remaining, flags, event_data, since, now, cb = (0x8009B6AC, 0x8009B6B0, 0x8009B6B4, 0x8009B6B8, 0x8009B6BC, 0x8009B6C4, 0x8009B6C8, 0x8009B6D0)
        saved = [getattr(R, f'UC_MIPS_REG_S{i}') for i in range(8)]+[R.UC_MIPS_REG_GP, R.UC_MIPS_REG_FP]
        cases = itertools.product((0, 1, 2, 3), (-1, 0, 1, 2), (0, 512), (2, 5, 0x105), ((500, 1300, 1300), (500, 1301, 1301), (500, 1300, 1301)), (False, True), (False, True))
        cases = list(cases)
        for image in images:
            m = Uc(UC_ARCH_MIPS, UC_MODE_MIPS32 | UC_MODE_LITTLE_ENDIAN)
            m.mem_map(0, 0x200000)
            def put(a, value): m.mem_write(a & 0x1FFFFFFF, bytes(value))
            def read(a, count): return bytes(m.mem_read(a & 0x1FFFFFFF, count))
            def reg(name): return m.reg_read(getattr(R, 'UC_MIPS_REG_'+name))
            put(0x8000F800, image)
            for address in (vsync, busy, busy2, save, complete):
                put(address, struct.pack('<III', 0, 0x03E00008, 0))
            queue = []
            def hook(machine, pc, count, user):
                if pc not in (vsync, busy, busy2, save, complete): return
                self.assertTrue(queue, case)
                address, args, before, after, result = queue.pop(0)
                self.assertEqual(pc, address, case)
                self.assertEqual(tuple(reg(r) for r in ('A0', 'A1', 'A2')[:len(args)]), args, case)
                self.assertEqual(read(start, 0x60), before, case)
                put(start, after)
                for name in ('V0', 'V1', 'A0', 'A1', 'A2', 'A3', 'T0', 'T1', 'T2', 'T3', 'T4', 'T5', 'T6', 'T7', 'T8', 'T9'):
                    machine.reg_write(getattr(R, 'UC_MIPS_REG_'+name), 0xBADC0000)
                machine.reg_write(R.UC_MIPS_REG_V0, result)
            m.hook_add(UC_HOOK_CODE, hook)
            for flag_value, count_value, sector_value, event, ticks, present, mutate in cases:
                case = (flag_value, count_value, sector_value, event, ticks, present, mutate)
                model = bytearray(b'\xA5'*0x60)
                def set_word(a, value): model[a-start:a-start+4] = struct.pack('<I', value & 0xFFFFFFFF)
                def get_word(a): return struct.unpack('<i', model[a-start:a-start+4])[0]
                for a, value in ((flags, flag_value), (remaining, count_value), (sector, sector_value), (dest, 0x80102000), (since, 100), (cb, complete if present else 0)):
                    set_word(a, value)
                initial = bytes(model)
                queue.clear(); tick_index = 0
                def call(address, args):
                    nonlocal tick_index
                    args = tuple(value & 0xFFFFFFFF for value in args)
                    before = bytes(model); result = 0
                    if address == vsync:
                        result = ticks[tick_index]; tick_index += 1
                    elif mutate and address in (busy, busy2):
                        set_word(remaining, 2); set_word(sector, 32); set_word(dest, 0x80103000)
                    elif mutate and address == save:
                        set_word(remaining, -1); set_word(cb, 0 if present else complete)
                    elif address == complete:
                        set_word(remaining, 9)
                    queue.append((address, args, before, bytes(model), result))
                    return result
                set_word(now, call(vsync, (0xFFFFFFFF,)))
                if flag_value & 1:
                    if get_word(remaining) > 0:
                        call(busy2, (get_word(dest), get_word(sector)))
                        set_word(event_data, data)
                    else:
                        call(save, ())
                        if get_word(cb): call(complete, ((5 if get_word(remaining) < 0 else event) & 255, data))
                else:
                    if get_word(remaining) > 0:
                        call(busy, (get_word(dest), get_word(sector)))
                        set_word(dest, get_word(dest)+get_word(sector)*4)
                        set_word(remaining, get_word(remaining)-1)
                    if call(vsync, (0xFFFFFFFF,)) > get_word(since)+1200: set_word(remaining, -1)
                    if get_word(remaining) == 0 or call(vsync, (0xFFFFFFFF,)) > get_word(since)+1200:
                        call(save, ())
                        if get_word(cb): call(complete, (5 if get_word(remaining) < 0 else 2, data))
                put(start, initial); put(data-0x20, b'\x5A'*0x50)
                for name, value in (('A0', event), ('A1', data), ('A2', 0x80104000), ('SP', stack), ('RA', stop)):
                    m.reg_write(getattr(R, 'UC_MIPS_REG_'+name), value)
                for i, r in enumerate(saved): m.reg_write(r, 0xABCD0000+i)
                m.emu_start(base, stop, count=400)
                self.assertEqual(queue, [], case)
                self.assertEqual(read(start, 0x60), bytes(model), case)
                self.assertEqual(read(data-0x20, 0x50), b'\x5A'*0x50, case)
                self.assertEqual(reg('PC'), stop, case); self.assertEqual(reg('SP'), stack, case)
                for i, r in enumerate(saved): self.assertEqual(m.reg_read(r), 0xABCD0000+i, case)


if __name__ == '__main__': unittest.main()
