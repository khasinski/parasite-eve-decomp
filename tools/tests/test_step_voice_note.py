"""Voice allocation, callback reloads, and plain-C whole-function matching."""
from pathlib import Path
import itertools
import random
import re
import struct
import unittest

ROOT = Path(__file__).resolve().parents[2]


class StepVoiceNoteTests(unittest.TestCase):
    def test_plain_c(self):
        source = (ROOT / 'src/main/akao/Akao_StepVoiceNote.c').read_text()
        source = re.sub(r'/\*.*?\*/|//[^\n]*', '', source, flags=re.S)
        self.assertIsNone(re.search(r'\b(?:asm|__asm__|INCLUDE_ASM|CC_POSTPASS|REGALLOC_BARRIER)\b', source))

    @unittest.skipUnless((ROOT / 'assets/USA/main.exe').is_file() and
                         (ROOT / 'build/USA/main.exe').is_file(), 'retail/build unavailable')
    def test_allocation_and_callbacks(self):
        try:
            from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN, UC_HOOK_CODE
            from unicorn import mips_const as R
        except ImportError:
            self.skipTest('unicorn unavailable')
        base, size = 0x8008900C, 0x20C
        offset = base - 0x8000F800
        bodies = [(ROOT / path).read_bytes()[offset:offset + size]
                  for path in ('assets/USA/main.exe', 'build/USA/main.exe')]
        self.assertEqual(len(bodies[0]), size)
        self.assertEqual(*bodies)
        tracks, banks, output = 0x80100020, 0x80110020, 0x80120020
        slots, controls = 0x800B002C, 0x8009D2B0
        cur, updates, reset = 0x8009D2C8, 0x8009D2C4, 0x8009D2B8
        callbacks = {0x80088344: 'off', 0x800878F0: 'write'}
        stop, stack = 0x80010000, 0x801F0000
        saved = [getattr(R, f'UC_MIPS_REG_S{i}') for i in range(8)] + [R.UC_MIPS_REG_GP, R.UC_MIPS_REG_FP]
        rng = random.Random(base)
        template = {tracks - 0x20: rng.randbytes(24 * 0x11C + 0x40),
                    banks - 0x20: rng.randbytes(0x110),
                    output - 0x20: rng.randbytes(0x44),
                    slots - 0x20: rng.randbytes(24 * 8 + 0x40),
                    controls: rng.randbytes(0x30)}

        def access(regions):
            def read(a, n):
                for start, data in regions.items():
                    if start <= a and a + n <= start + len(data):
                        return bytes(data[a - start:a - start + n])
                raise AssertionError(hex(a))
            def put(a, value, n=4):
                for start, data in regions.items():
                    if start <= a and a + n <= start + len(data):
                        data[a - start:a - start + n] = (value & ((1 << (8*n))-1)).to_bytes(n, 'little')
                        return
                raise AssertionError(hex(a))
            return read, put, lambda a: int.from_bytes(read(a, 4), 'little')

        for mask, requested, direct, first_free, mutate in itertools.product(
                (0, 1, 5, 15, 0x800000), (0, 5, 0xFFFFFF), (0, 0xFFFFFF), range(25), (False, True)):
            initial = {a: bytearray(data) for a, data in template.items()}
            read, put, get = access(initial)
            put(cur, banks); put(updates, 0x81); put(reset, 5); put(output, 0x80000000)
            put(banks + 0x10, requested)
            for i in range(24):
                put(tracks + i*0x11C + 0xF0, i if i % 2 else 24)
                put(tracks + i*0x11C + 0xF4, i % 3)
                put(slots + i*8, 0 if i == first_free else 0x8001, 2)
            expected = {a: bytearray(data) for a, data in initial.items()}
            read, put, get = access(expected)
            events = []

            def callback(name, args, get, put):
                if not mutate: return
                if name == 'off':
                    put(args[0] + 0xF4, get(args[0] + 0xF4) ^ 1)
                    put(cur, banks + 0x68)
                    put(banks + 0x10, 0)
                else:
                    put(args[1] + 4, 0)
                    put(reset, get(reset) ^ 5)

            def event(name, args):
                events.append((name, args, {a: bytes(data) for a, data in expected.items()}))
                callback(name, args, get, put)

            # Independent bounded allocation oracle; no emulated instruction reuse.
            for index in range(24):
                bit = 1 << index
                if not mask & bit: continue
                track = tracks + index*0x11C
                event('off', (track, bit, index))
                if not get(track + 0xF4): continue
                if requested & bit:
                    if direct & bit:
                        put(output, get(output) | bit)
                        put(track + 0xF0, index)
                        put(track + 0xF4, get(track + 0xF4) | 0x1FF93)
                    else:
                        available = next((i for i in range(24) if read(slots+i*8, 2) == b'\0\0'), 24)
                        if available < 24:
                            put(track + 0xF4, get(track + 0xF4) | 0x1FF93)
                            put(output, get(output) | (1 << available))
                            put(track + 0xF0, available)
                            put(slots + available*8, 0x7FFF, 2)
                            put(updates, get(updates) | 0x100)
                        else:
                            put(track + 0xF0, 24)
                            put(get(cur), get(get(cur)) | 1)
                if get(reset) & bit:
                    put(track + 0x118, 0)
                voice = get(track + 0xF0)
                if voice < 24: event('write', (voice, track + 0xF0, get(track + 0x38)))

            for body in bodies:
                m = Uc(UC_ARCH_MIPS, UC_MODE_MIPS32 | UC_MODE_LITTLE_ENDIAN)
                m.mem_map(0, 0x200000)
                def memread(a, n): return bytes(m.mem_read(a & 0x1FFFFFFF, n))
                def memput(a, value): m.mem_write(a & 0x1FFFFFFF, struct.pack('<I', value & 0xFFFFFFFF))
                def memget(a): return int.from_bytes(memread(a, 4), 'little')
                m.mem_write(base & 0x1FFFFFFF, body)
                for a, data in initial.items(): m.mem_write(a & 0x1FFFFFFF, bytes(data))
                for a in callbacks: m.mem_write(a & 0x1FFFFFFF, struct.pack('<III', 0, 0x03E00008, 0))
                observed = []
                def hook(machine, pc, size, user):
                    if pc - 4 not in callbacks: return
                    name = callbacks[pc - 4]
                    args = tuple(machine.reg_read(getattr(R, 'UC_MIPS_REG_' + r)) for r in ('A0', 'A1', 'A2'))
                    observed.append((name, args, {a: memread(a, len(data)) for a, data in initial.items()}))
                    callback(name, args, memget, memput)
                    for r in ('V0', 'V1', 'A0', 'A1', 'A2', 'A3', 'T0', 'T1', 'T2', 'T3', 'T4', 'T5', 'T6', 'T7', 'T8', 'T9'):
                        machine.reg_write(getattr(R, 'UC_MIPS_REG_' + r), 0xDEADCAFE)
                m.hook_add(UC_HOOK_CODE, hook)
                for name, value in (('A0', tracks), ('A1', mask), ('A2', direct), ('A3', output), ('SP', stack), ('RA', stop)):
                    m.reg_write(getattr(R, 'UC_MIPS_REG_' + name), value)
                for i, reg in enumerate(saved): m.reg_write(reg, 0xABCD0000 + i)
                m.emu_start(base, stop, count=10000)
                case = (mask, requested, direct, first_free, mutate)
                self.assertEqual(observed, events, case)
                for a, data in expected.items(): self.assertEqual(memread(a, len(data)), data, case)
                self.assertEqual(m.reg_read(R.UC_MIPS_REG_PC), stop, case)
                self.assertEqual(m.reg_read(R.UC_MIPS_REG_SP), stack, case)
                for i, reg in enumerate(saved): self.assertEqual(m.reg_read(reg), 0xABCD0000 + i, case)


if __name__ == '__main__': unittest.main()
