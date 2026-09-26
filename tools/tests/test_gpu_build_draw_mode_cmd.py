"""Exact retail bytes and bit-mask model for the GPU drawing-mode command."""
from pathlib import Path
import random
import shutil
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[2]


@unittest.skipUnless(shutil.which('mipsel-none-elf-ld') and (ROOT/'assets/USA/main.exe').is_file(), 'retail image/toolchain unavailable')
class GpuBuildDrawModeCmdTests(unittest.TestCase):
    def test_retail_bytes_and_command_model(self):
        try:
            from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN
            from unicorn import mips_const as R
        except ImportError:
            self.skipTest('unicorn unavailable')
        ENTRY, EXIT = 0x80076150, 0x80010000
        retail = (ROOT/'assets/USA/main.exe').read_bytes()[0x66950:0x66970]
        source = ROOT/'src/main/gpu/Gpu_BuildDrawModeCmd.c'
        with tempfile.TemporaryDirectory() as directory:
            work = Path(directory)
            subprocess.run([str(ROOT/'tools/scripts/cc.sh'), str(source), str(work/'test.o')], check=True, capture_output=True)
            (work/'test.ld').write_text('SECTIONS { .text 0x80076150 : SUBALIGN(4) { *(.text) } /DISCARD/ : { *(.reginfo) *(.mdebug) *(.pdr) *(.MIPS.abiflags) } }')
            subprocess.run(['mipsel-none-elf-ld', '-EL', '-T', str(work/'test.ld'), str(work/'test.o'), '-o', str(work/'test.elf')], check=True, capture_output=True)
            subprocess.run(['mipsel-none-elf-objcopy', '-O', 'binary', '-j', '.text', str(work/'test.elf'), str(work/'test.bin')], check=True, capture_output=True)
            compiled = (work/'test.bin').read_bytes()
        self.assertEqual(compiled, retail)
        rng = random.Random(ENTRY)
        cases = [(a, b, bits) for bits in range(4096) for a, b in ((0, 0), (0, 1), (1, 0), (1, 1))]
        cases += [(rng.getrandbits(32), rng.getrandbits(32), rng.getrandbits(32)) for _ in range(1024)]
        for body in (retail, compiled):
            machine = Uc(UC_ARCH_MIPS, UC_MODE_MIPS32 | UC_MODE_LITTLE_ENDIAN)
            machine.mem_map(0, 0x200000)
            machine.mem_write(ENTRY & 0x1FFFFFFF, body)
            for a, b, bits in cases:
                machine.reg_write(R.UC_MIPS_REG_A0, a)
                machine.reg_write(R.UC_MIPS_REG_A1, b)
                machine.reg_write(R.UC_MIPS_REG_A2, bits)
                machine.reg_write(R.UC_MIPS_REG_RA, EXIT)
                machine.emu_start(ENTRY, EXIT, count=100)
                expected = 0xE1000000 | (0x200 if b else 0) | (bits & 0x9FF) | (0x400 if a else 0)
                self.assertEqual(machine.reg_read(R.UC_MIPS_REG_PC), EXIT)
                self.assertEqual(machine.reg_read(R.UC_MIPS_REG_V0), expected, (a, b, bits))
