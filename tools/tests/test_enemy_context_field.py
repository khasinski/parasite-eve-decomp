"""All enemy-field selectors, signed limits, and the consuming status flag."""
from pathlib import Path
import random
import re
import struct
import unittest

ROOT = Path(__file__).resolve().parents[2]


class EnemyContextFieldTests(unittest.TestCase):
    def test_plain_c(self):
        source = (ROOT / 'src/main/battle/Battle_GetEnemyContextField.c').read_text()
        source = re.sub(r'/\*.*?\*/|//[^\n]*', '', source, flags=re.S)
        self.assertIsNone(re.search(r'\b(?:asm|__asm__|INCLUDE_ASM|CC_POSTPASS|REGALLOC_BARRIER)\b', source))

    @unittest.skipUnless((ROOT / 'assets/USA/main.exe').is_file() and
                         (ROOT / 'build/USA/main.exe').is_file(), 'retail/build unavailable')
    def test_all_selectors(self):
        try:
            from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN
            from unicorn import mips_const as R
        except ImportError:
            self.skipTest('unicorn unavailable')
        base, size = 0x8003010C, 0x114
        images = [(ROOT / path).read_bytes() for path in ('assets/USA/main.exe', 'build/USA/main.exe')]
        offset = base - 0x8000F800
        self.assertEqual(images[0][offset:offset+size], images[1][offset:offset+size])
        ctx, holder, stop, stack = 0x80180020, 0x80190020, 0x80010000, 0x801F0000
        saved = [getattr(R, f'UC_MIPS_REG_S{i}') for i in range(8)] + [R.UC_MIPS_REG_GP, R.UC_MIPS_REG_FP]
        rng = random.Random(base)
        initial = rng.randbytes(0x118)
        holder_data = bytearray(rng.randbytes(0x44))
        struct.pack_into('<I', holder_data, 0x20, ctx)
        limits = (-2147483648, -1, 0, 1, 2147483647)
        for image in images:
            m = Uc(UC_ARCH_MIPS, UC_MODE_MIPS32 | UC_MODE_LITTLE_ENDIAN)
            m.mem_map(0, 0x200000)
            # Load the linked jump table as well as instructions.
            m.mem_write(0xF800, image)
            m.mem_write((holder - 0x20) & 0x1FFFFFFF, bytes(holder_data))
            for pattern in range(32):
                before = bytearray(initial)
                core = ((pattern & 7) << 21) | ((pattern >> 3) << 13) | ((pattern & 3) << 18) | 0xA5000000
                flags = 0x80F01234 | ((pattern & 1) << 24)
                hp, field = limits[pattern % 5], limits[(pattern+2) % 5]
                rank = (0, 127, 128, 255)[pattern % 4]
                struct.pack_into('<I', before, 0x20, core)
                before[0x24] = rank
                struct.pack_into('<H', before, 0x2C, pattern * 2114)
                struct.pack_into('<i', before, 0x30, hp)
                struct.pack_into('<i', before, 0xA8, field)
                struct.pack_into('<H', before, 0xAC, (65535 - pattern*2114) & 65535)
                struct.pack_into('<I', before, 0xEC, flags)
                for selector in range(256):
                    expected = bytearray(before)
                    result = -1000
                    case = selector - 0x29
                    if case == 0: result = rank if rank < 128 else rank - 256
                    elif case == 2: result = pattern*2114
                    elif case == 3: result = max(0, hp)
                    elif case == 7: result = before[0x20 + 0x1C + ((core >> 17) & 0x70)]
                    elif case == 19: result = max(0, field)
                    elif case == 20: result = (65535 - pattern*2114) & 65535
                    elif case == 36: result = (core >> 13) & 3
                    elif case == 41: result = (core >> 24) & 63
                    elif case == 89:
                        result = int(bool(flags & 0x1000000) and (core & 0xC0000) == 0xC0000)
                        if result: struct.pack_into('<I', expected, 0xEC, flags & ~0x1000000)
                    for high in (0, 0xABCDE000):
                        m.mem_write((ctx - 0x20) & 0x1FFFFFFF, bytes(before))
                        for name, value in (('A0', holder), ('A1', high | selector), ('SP', stack), ('RA', stop)):
                            m.reg_write(getattr(R, 'UC_MIPS_REG_' + name), value)
                        for i, reg in enumerate(saved): m.reg_write(reg, 0xABCD0000 + i)
                        m.emu_start(base, stop, count=100)
                        detail = (pattern, selector, high)
                        self.assertEqual(m.reg_read(R.UC_MIPS_REG_V0), result & 0xFFFFFFFF, detail)
                        self.assertEqual(bytes(m.mem_read((ctx-0x20) & 0x1FFFFFFF, len(expected))), expected, detail)
                        self.assertEqual(bytes(m.mem_read((holder-0x20) & 0x1FFFFFFF, len(holder_data))), holder_data, detail)
                        self.assertEqual(m.reg_read(R.UC_MIPS_REG_PC), stop, detail)
                        self.assertEqual(m.reg_read(R.UC_MIPS_REG_SP), stack, detail)
                        for i, reg in enumerate(saved): self.assertEqual(m.reg_read(reg), 0xABCD0000 + i, detail)


if __name__ == '__main__': unittest.main()
