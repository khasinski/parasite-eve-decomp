"""Exact retail bytes and independent story-day/flag state transitions."""
from pathlib import Path
import random
import shutil
import struct
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[2]


@unittest.skipUnless(shutil.which('mipsel-none-elf-ld') and (ROOT/'assets/USA/main.exe').is_file(), 'retail/toolchain unavailable')
class SceneSetStoryDayTests(unittest.TestCase):
    def test_retail_bytes_and_state_model(self):
        try:
            from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN
            from unicorn import mips_const as R
        except ImportError:
            self.skipTest('unicorn unavailable')
        entry, stop, state, flags_address, lock = 0x8006C4C4, 0x80010000, 0x800B0CD8, 0x8009D1A0, 0x8009D2E8
        retail = (ROOT/'assets/USA/main.exe').read_bytes()[0x5CCC4:0x5CDBC]
        symbols = dict(g_GameState=state, g_GameStateFlags=flags_address,
                       D_8009D2E8=lock, D_800B0CE4=state+12,
                       D_800B0CE5=state+13, D_800B0CE6=state+14)
        with tempfile.TemporaryDirectory() as directory:
            work = Path(directory)
            source = ROOT/'src/main/gpu/Scene_SetStoryDay.c'
            subprocess.run([str(ROOT/'tools/scripts/cc.sh'),str(source),str(work/'test.o')],check=True,capture_output=True)
            script = 'SECTIONS { .text 0x8006C4C4 : SUBALIGN(4) { *(.text) } /DISCARD/ : { *(.reginfo) *(.mdebug) *(.pdr) *(.MIPS.abiflags) } }\n'
            (work/'test.ld').write_text(script+'\n'.join(f'{name} = 0x{address:X};' for name,address in symbols.items()))
            subprocess.run(['mipsel-none-elf-ld','-EL','-T',str(work/'test.ld'),str(work/'test.o'),'-o',str(work/'test.elf')],check=True,capture_output=True)
            subprocess.run(['mipsel-none-elf-objcopy','-O','binary','-j','.text',str(work/'test.elf'),str(work/'test.bin')],check=True,capture_output=True)
            compiled = (work/'test.bin').read_bytes()
        self.assertEqual(compiled, retail)
        rng = random.Random(entry)
        cases = [(pending, bits, day) for pending in range(256) for bits in range(8) for day in (-1, 1, 8, 9)]
        edge_days = (-2147483648, -2147483647, -129, -128, -2, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 127, 128, 255, 256, 2147483647)
        cases += [(rng.randrange(256), rng.randrange(256), day) for day in edge_days for _ in range(32)]
        machines = []
        for body in (retail, compiled):
            machine = Uc(UC_ARCH_MIPS, UC_MODE_MIPS32 | UC_MODE_LITTLE_ENDIAN)
            machine.mem_map(0, 0x200000)
            machine.mem_write(entry & 0x1FFFFFFF, body)
            machines.append(machine)
        coverage = set()
        for pending, bits, day in cases:
            original = bytearray(rng.randbytes(0x95C))
            original[13] = pending
            original[14] = bits
            global_flags, lock_word = rng.getrandbits(32), rng.getrandbits(32)
            state_flags = struct.unpack_from('<I', original)[0]
            coverage.add((bool(global_flags & 2), bool(state_flags & 2)))
            expected = original.copy()
            effective_day = day
            if day == -1:
                effective_day = pending if pending < 128 else pending - 256
                expected[12] = pending
                expected[14] |= 3
            expected_lock = lock_word
            if (global_flags | state_flags) & 2:
                expected[14] |= 2
                expected_lock &= ~2
            if expected[14] & 4:
                expected[14] = (expected[14] | 3) & ~4
            signed_pending = pending if pending < 128 else pending - 256
            if 1 <= effective_day <= 8 and effective_day != signed_pending:
                expected[12] = expected[13] = effective_day
                expected[14] |= 1
            for machine in machines:
                machine.mem_write(state & 0x1FFFFFFF, bytes(original))
                machine.mem_write(flags_address & 0x1FFFFFFF, struct.pack('<I', global_flags))
                machine.mem_write(lock & 0x1FFFFFFF, struct.pack('<I', lock_word))
                machine.reg_write(R.UC_MIPS_REG_A0, day & 0xFFFFFFFF)
                machine.reg_write(R.UC_MIPS_REG_RA, stop)
                machine.reg_write(R.UC_MIPS_REG_SP, 0x801F0000)
                for i in range(8):
                    machine.reg_write(getattr(R, f'UC_MIPS_REG_S{i}'), 0xABCD0000+i)
                machine.emu_start(entry, stop, count=200)
                self.assertEqual(machine.reg_read(R.UC_MIPS_REG_PC), stop)
                self.assertEqual(machine.reg_read(R.UC_MIPS_REG_V0), 0)
                self.assertEqual(machine.reg_read(R.UC_MIPS_REG_SP), 0x801F0000)
                for i in range(8):
                    self.assertEqual(machine.reg_read(getattr(R, f'UC_MIPS_REG_S{i}')), 0xABCD0000+i)
                self.assertEqual(bytes(machine.mem_read(state & 0x1FFFFFFF, len(expected))), expected, (pending, bits, day))
                self.assertEqual(bytes(machine.mem_read(flags_address & 0x1FFFFFFF, 4)), struct.pack('<I', global_flags))
                self.assertEqual(bytes(machine.mem_read(lock & 0x1FFFFFFF, 4)), struct.pack('<I', expected_lock))
        self.assertEqual(len(coverage), 4)
