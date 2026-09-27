"""CD state initialization, preserved padding and conversion-call ordering."""
from pathlib import Path
import random
import struct
import unittest

ROOT = Path(__file__).resolve().parents[2]


class CDInitCmdStateTests(unittest.TestCase):
    def test_plain_source(self):
        source = (ROOT/'src/main/cdrom/CdRom_InitCmdState.c').read_text()
        self.assertNotRegex(source, r'\b(?:asm|__asm__)\b')
        self.assertIn('extern u_int D_8009B560[3];', source)

    @unittest.skipUnless((ROOT/'assets/USA/main.exe').is_file() and
                         (ROOT/'build/USA/main.exe').is_file(), 'images unavailable')
    def test_initialization(self):
        try:
            from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN, UC_HOOK_CODE
            from unicorn import mips_const as R
        except ImportError:
            self.skipTest('unicorn unavailable')
        images = [(ROOT/p).read_bytes() for p in ('assets/USA/main.exe', 'build/USA/main.exe')]
        base, size = 0x8007FA2C, 0xD8
        offset = base-0x8000F800
        self.assertEqual(images[0][offset:offset+size], images[1][offset:offset+size])
        state, guard, convert = 0x8009B554, 0x8009B534, 0x80080B44
        stop, stack = 0x80010000, 0x801F0000
        saved = [getattr(R, f'UC_MIPS_REG_S{i}') for i in range(8)]+[R.UC_MIPS_REG_GP, R.UC_MIPS_REG_FP]
        for image in images:
            for mutate in (False, True):
                m = Uc(UC_ARCH_MIPS, UC_MODE_MIPS32 | UC_MODE_LITTLE_ENDIAN)
                m.mem_map(0, 0x200000)
                def put(address, value): m.mem_write(address & 0x1FFFFFFF, bytes(value))
                def read(address, count): return bytes(m.mem_read(address & 0x1FFFFFFF, count))
                put(0x8000F800, image)
                if mutate: put(convert, struct.pack('<III', 0, 0x03E00008, 0))
                calls = []
                def hook(machine, pc, count, user):
                    if pc != convert: return
                    calls.append(pc)
                    args = (machine.reg_read(R.UC_MIPS_REG_A0), machine.reg_read(R.UC_MIPS_REG_A1))
                    self.assertEqual(args, (0, state+0x2E), case)
                    self.assertEqual(read(guard, 0x90), bytes(before_call), case)
                    if mutate:
                        put(state, replacement)
                        for name in ('V0', 'V1', 'A0', 'A1', 'A2', 'A3', 'T0', 'T1', 'T2', 'T3', 'T4', 'T5', 'T6', 'T7', 'T8', 'T9'):
                            machine.reg_write(getattr(R, 'UC_MIPS_REG_'+name), 0xBADC0000)
                m.hook_add(UC_HOOK_CODE, hook)
                for seed in range(128):
                    case = (mutate, seed)
                    rng = random.Random(seed)
                    initial = rng.randbytes(0x90)
                    replacement = rng.randbytes(0x50)
                    before_call = bytearray(initial)
                    def set_field(buffer, off, value, width=4):
                        buffer[0x20+off:0x20+off+width] = value.to_bytes(width, 'little')
                    for off in (0, 0xC, 0x10, 0x14, 0x1C): set_field(before_call, off, 0)
                    for off in (4, 5, 6, 7, 8, 0x18, 0x2C, 0x2D): set_field(before_call, off, 0, 1)
                    for off, value in ((0x20, 2), (0x24, 14), (0x28, 21)): set_field(before_call, off, value)
                    expected = bytearray(before_call)
                    if mutate:
                        expected[0x20:0x70] = replacement
                    else:
                        # Sector zero is MSF 00:02:00; CdIntToPos leaves track unchanged.
                        expected[0x4E:0x51] = b'\0\x02\0'
                    for off in range(0x32, 0x38): set_field(expected, off, 0, 1)
                    for off in (0x38, 0x40, 0x44, 0x48, 0x4C): set_field(expected, off, 0)
                    set_field(expected, 0x3C, 1)
                    put(guard, initial)
                    calls.clear()
                    for name, value in (('SP', stack), ('RA', stop)):
                        m.reg_write(getattr(R, 'UC_MIPS_REG_'+name), value)
                    for i, reg in enumerate(saved): m.reg_write(reg, 0xABCD0000+i)
                    m.emu_start(base, stop, count=500)
                    self.assertEqual(calls, [convert], case)
                    self.assertEqual(read(guard, 0x90), bytes(expected), case)
                    self.assertEqual(m.reg_read(R.UC_MIPS_REG_PC), stop, case)
                    self.assertEqual(m.reg_read(R.UC_MIPS_REG_SP), stack, case)
                    for i, reg in enumerate(saved): self.assertEqual(m.reg_read(reg), 0xABCD0000+i, case)


if __name__ == '__main__': unittest.main()
