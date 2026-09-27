"""Voice sequencer symbol declarations and mutating-callback regression."""
from pathlib import Path
import itertools
import random
import re
import struct
import unittest

ROOT = Path(__file__).resolve().parents[2]


class VoiceSequencerAliasTests(unittest.TestCase):
    def test_aliases_removed(self):
        source = (ROOT / 'src/main/akao/voice_sequencer.c').read_text()
        self.assertIsNone(re.search(r'extern[^;]*__asm__', source))
        self.assertEqual(source.count('extern void *g_AkaoCurTrack;'), 1)
        self.assertEqual(source.count('asm("$'), 3)

    @unittest.skipUnless((ROOT / 'assets/USA/main.exe').is_file() and
                         (ROOT / 'build/USA/main.exe').is_file(), 'retail/build unavailable')
    def test_sequencer_callbacks(self):
        try:
            from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN, UC_HOOK_CODE
            from unicorn import mips_const as R
        except ImportError:
            self.skipTest('unicorn unavailable')
        base, size = 0x80089F58, 0x3FC
        offset = base - 0x8000F800
        bodies = [(ROOT / path).read_bytes()[offset:offset + size]
                  for path in ('assets/USA/main.exe', 'build/USA/main.exe')]
        self.assertEqual(len(bodies[0]), size)
        self.assertEqual(*bodies)
        tracks, secondary, banks, script = 0x800B8AC0, 0x800BA560, 0x80100020, 0x80110020
        stop, stack = 0x80010000, 0x801F0000
        saved = [getattr(R, f'UC_MIPS_REG_S{i}') for i in range(8)] + [R.UC_MIPS_REG_GP, R.UC_MIPS_REG_FP]
        callbacks = {0x8008F178: ('pitch', 2), 0x80089960: ('volume', 1),
                     0x80089B28: ('adsr', 0), 0x80089CF0: ('start', 0)}
        rng = random.Random(base)
        # The two fixed 24-track tables are contiguous; one region guards both.
        template = {tracks - 0x20: rng.randbytes(48 * 0x11C + 0x40),
                    banks - 0x20: rng.randbytes(0x1C0),
                    script - 0x20: rng.randbytes(0x100),
                    0x8009D2B0: rng.randbytes(0x40),
                    0x8009CDE0: rng.randbytes(0x20),
                    0x800BCD40: rng.randbytes(0x30)}
        for mask, pending, secondary_mask, mutate in itertools.product(
                (0, 1, 2, 5, 0x800000, 0x555555, 0xFFFFFF, 0xAB00000F),
                (0, 1), (0, 5, 0xFFFFFF), (False, True)):
            results = []
            for body in bodies:
                m = Uc(UC_ARCH_MIPS, UC_MODE_MIPS32 | UC_MODE_LITTLE_ENDIAN)
                m.mem_map(0, 0x200000)
                def read(a, n): return bytes(m.mem_read(a & 0x1FFFFFFF, n))
                def put(a, data): m.mem_write(a & 0x1FFFFFFF, bytes(data))
                def word(a, v): put(a, struct.pack('<I', v & 0xFFFFFFFF))
                def get(a): return int.from_bytes(read(a, 4), 'little')
                put(base, body)
                for a, data in template.items(): put(a, data)
                word(0x8009D2C8, banks)
                word(0x8009D2DC, pending)
                word(0x8009CDE8, 0x100 if mutate else 0)
                word(0x800BCD50, 0xA55A)
                word(banks + 0x6C, secondary_mask)
                for i in range(24): word(secondary + i*0x11C + 0xF0, (i*7) % 27)
                put(script, struct.pack('<IIII', mask, 0xAA55AA55, 0x55AA55AA, 0xDEADBEEF))
                for i in range(24): put(script + 16 + 2*i, struct.pack('<H', (i*257) & 0xFFFF))
                program = read(script - 0x20, 0x100)
                for a in callbacks: put(a, struct.pack('<III', 0, 0x03E00008, 0))
                events = []
                selected = [i for i in range(24) if mask & (1 << i)]
                def snapshot(): return {a: read(a, len(data)) for a, data in template.items()}
                def hook(machine, pc, size, user):
                    if pc - 4 not in callbacks: return
                    name, count = callbacks[pc - 4]
                    args = tuple(machine.reg_read(getattr(R, 'UC_MIPS_REG_' + r)) for r in ('A0', 'A1')[:count])
                    events.append((name, args, snapshot()))
                    if name == 'pitch':
                        self.assertEqual(args[1], 0)
                        index = (args[0] - tracks) // 0x11C
                        self.assertIn(index, selected)
                        ordinal = selected.index(index)
                        self.assertEqual(get(args[0]), script + 18 + 2*ordinal + ordinal*257)
                        self.assertEqual(get(args[0] + 0xF0), 24)
                        self.assertEqual(get(args[0] + 0x14), script)
                        if mutate:
                            word(args[0] + 0x38, 0x12340000 | index)
                            word(0x8009D2C8, banks + 0x100)
                    elif name == 'volume':
                        self.assertEqual(args, (0xFFFFFF,))
                        current = get(0x8009D2C8)
                        self.assertEqual(get(current + 0x14), 0xFFFFFF)
                        self.assertEqual(get(current + 0x20), 0xFFFF0000)
                        self.assertEqual(get(0x8009D2C4), 0)
                    if mutate and name != 'pitch':
                        word(get(0x8009D2C8) + 0x40, get(get(0x8009D2C8) + 0x40) + 1)
                    for r in ('V0', 'V1', 'A0', 'A1', 'A2', 'A3', 'T0', 'T1', 'T2', 'T3', 'T4', 'T5', 'T6', 'T7', 'T8', 'T9'):
                        machine.reg_write(getattr(R, 'UC_MIPS_REG_' + r), 0xDEADCAFE)
                m.hook_add(UC_HOOK_CODE, hook)
                for name, value in (('A0', script), ('SP', stack), ('RA', stop)):
                    m.reg_write(getattr(R, 'UC_MIPS_REG_' + name), value)
                for i, reg in enumerate(saved): m.reg_write(reg, 0xABCD0000 + i)
                m.emu_start(base + 0x110, stop, count=20000)
                case = (mask, pending, secondary_mask, mutate)
                self.assertEqual([e[0] for e in events], ['pitch'] * len(selected) + ['volume', 'adsr', 'start'], case)
                self.assertEqual(read(script - 0x20, 0x100), program, case)
                self.assertEqual(m.reg_read(R.UC_MIPS_REG_PC), stop, case)
                self.assertEqual(m.reg_read(R.UC_MIPS_REG_SP), stack, case)
                for i, reg in enumerate(saved): self.assertEqual(m.reg_read(reg), 0xABCD0000 + i, case)
                results.append((events, snapshot()))
            self.assertEqual(*results, case)


if __name__ == '__main__': unittest.main()
