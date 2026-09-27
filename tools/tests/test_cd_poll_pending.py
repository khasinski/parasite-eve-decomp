"""Pending queue poll with state changes across both synchronization calls."""
from pathlib import Path
import itertools
import struct
import unittest

ROOT = Path(__file__).resolve().parents[2]


class CDPollPendingTests(unittest.TestCase):
    def test_plain_source(self):
        source = (ROOT/'src/main/cdrom/CdRom_PollPendingDsRead.c').read_text()
        self.assertNotRegex(source, r'\b(?:asm|__asm__|PE1_COMPILER_MEMORY_BARRIER|PE1_COMPILER_LAUNDER)\b')

    @unittest.skipUnless((ROOT/'assets/USA/main.exe').is_file() and
                         (ROOT/'build/USA/main.exe').is_file(), 'images unavailable')
    def test_queue(self):
        try:
            from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN, UC_HOOK_CODE
            from unicorn import mips_const as R
        except ImportError:
            self.skipTest('unicorn unavailable')
        images = [(ROOT/p).read_bytes() for p in ('assets/USA/main.exe', 'build/USA/main.exe')]
        base, size = 0x8007F7E8, 0xA4
        offset = base-0x8000F800
        self.assertEqual(images[0][offset:offset+size], images[1][offset:offset+size])
        start, queue, index_address, count_address = 0x800A3530, 0x800A3540, 0x800A3604, 0x800A3608
        sync, send, stop, stack = 0x8007FBF0, 0x8007FB44, 0x80010000, 0x801F0000
        saved = [getattr(R, f'UC_MIPS_REG_S{i}') for i in range(8)]+[R.UC_MIPS_REG_GP, R.UC_MIPS_REG_FP]
        for image in images:
            m = Uc(UC_ARCH_MIPS, UC_MODE_MIPS32 | UC_MODE_LITTLE_ENDIAN)
            m.mem_map(0, 0x200000)
            def put(a, v): m.mem_write(a & 0x1FFFFFFF, bytes(v))
            def read(a, n): return bytes(m.mem_read(a & 0x1FFFFFFF, n))
            def word(a, v): put(a, struct.pack('<I', v & 0xFFFFFFFF))
            def reg(name): return m.reg_read(getattr(R, 'UC_MIPS_REG_'+name))
            def change(a, v):
                word(a, v); expected[a-start:a-start+4] = struct.pack('<I', v & 0xFFFFFFFF)
            put(0x8000F800, image)
            for address in (sync, send): put(address, struct.pack('<III', 0, 0x03E00008, 0))
            calls = []
            def hook(machine, pc, count, user):
                if pc not in (sync, send): return
                calls.append(pc)
                self.assertEqual(read(start, 0xF0), bytes(expected), case)
                result = 0xDEADBEEF
                if pc == sync:
                    self.assertEqual(reg('A0'), 0, case)
                    if calls.count(sync) == 1:
                        result = first
                        if mutate: change(count_address, pending)
                    else:
                        result = second
                        if mutate:
                            change(index_address, index)
                            change(count_address, 0)
                            change(queue+24*index, active)
                else:
                    self.assertEqual((reg('A0'), reg('A1')), (0x80+index, 0x80101000+index*0x20), case)
                    change(queue+24*index, 0)
                    change(count_address, 0xFFFFFFFF)
                for name in ('V0', 'V1', 'A0', 'A1', 'A2', 'A3', 'T0', 'T1', 'T2', 'T3', 'T4', 'T5', 'T6', 'T7', 'T8', 'T9'):
                    machine.reg_write(getattr(R, 'UC_MIPS_REG_'+name), 0xBADC0000)
                machine.reg_write(R.UC_MIPS_REG_V0, result)
            m.hook_add(UC_HOOK_CODE, hook)
            for first, second, pending, index, active, mutate in itertools.product((0, 1, 2, 0xFFFFFFFF), (0, 1, 2, 0xFFFFFFFF), (-1, 0, 1, 8), range(8), (0, 1), (False, True)):
                case = (first, second, pending, index, active, mutate)
                put(start, b'\xA5'*0xF0)
                for slot in range(8):
                    word(queue+slot*24, active if slot == index else 0)
                    put(queue+slot*24+4, bytes([0x80+slot]))
                    word(queue+slot*24+12, 0x80101000+slot*0x20)
                word(count_address, (0 if pending > 0 else 1) if mutate else pending)
                word(index_address, index ^ 7 if mutate else index)
                expected = bytearray(read(start, 0xF0)); calls.clear()
                for name, value in (('SP', stack), ('RA', stop)): m.reg_write(getattr(R, 'UC_MIPS_REG_'+name), value)
                for i, r in enumerate(saved): m.reg_write(r, 0xABCD0000+i)
                m.emu_start(base, stop, count=200)
                wanted = [sync]
                if first == 1 and pending > 0:
                    wanted.append(sync)
                    if second == 1 and active: wanted.append(send)
                self.assertEqual(calls, wanted, case)
                self.assertEqual(read(start, 0xF0), bytes(expected), case)
                self.assertEqual(reg('PC'), stop, case); self.assertEqual(reg('SP'), stack, case)
                for i, r in enumerate(saved): self.assertEqual(m.reg_read(r), 0xABCD0000+i, case)


if __name__ == '__main__': unittest.main()
