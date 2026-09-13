"""Full linked-byte regression for four Psy-Q LIBMATH/LIBGPU/LIBCD functions."""
import hashlib
import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]

# Entire linked text ranges at their retail addresses.
CASES = [('libmath/__divdf3',
  736,
  '566525621fb9cefc8d54330a0eceaab78727efcf5e25149fe242fdd3b5e69a6f',
  'Math_Cmp64Pair = 0x80073244;\n'
  'Math_Add64WithShift = 0x80072EA4;\n'
  'Math_Neg64 = 0x80073554;\n'
  'Math_Add64 = 0x80072DF4;\n'
  'Math_Shift64 = 0x80073294;\n'
  'Evt_Deliver = 0x800739C4;\n'
  'SECTIONS { .text 0x80072f64 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }'),
 ('libmath/__adddf3',
  884,
  '3060f098b67be7758d285c025aca8d2dfc4db7a0b07501b9c929bdb914f11835',
  'Math_Neg64 = 0x80073554;\n'
  'Math_Add64WithShift = 0x80072EA4;\n'
  'Math_Add64 = 0x80072DF4;\n'
  'Evt_Deliver = 0x800739C4;\n'
  'SECTIONS { .text 0x80072a74 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }'),
 ('libgpu/PutDispEnv',
  1272,
  'c8dc390773e463aadf9158ed73f3cb4467a6c8b2af8f853c3950512ef0577ca1',
  'D_8009574E = 0x8009574E;\n'
  'D_80011970 = 0x80011970;\n'
  'D_80095748 = 0x80095748;\n'
  'D_80095744 = 0x80095744;\n'
  'GetVideoMode = 0x80074A28;\n'
  'D_8009574F = 0x8009574F;\n'
  'D_800957C0 = 0x800957C0;\n'
  'D_80095848 = 0x80095848;\n'
  'D_80095820 = 0x80095820;\n'
  'D_80095822 = 0x80095822;\n'
  'D_800957B8 = 0x800957B8;\n'
  'memcpy = 0x80071A34;\n'
  'SECTIONS { .text 0x800755f0 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }'),
 ('libcd/StCdInterrupt',
  2332,
  '7c4ca8096a8921313e5b9a5f036f2b9341d656633afed2132b62af8d40aa8c74',
  'D_800B89F4 = 0x800B89F4;\n'
  'D_800A801C = 0x800A801C;\n'
  'D_8009B34C = 0x8009B34C;\n'
  'D_800C0DB8 = 0x800C0DB8;\n'
  'D_800B0CD0 = 0x800B0CD0;\n'
  'D_800BCD7C = 0x800BCD7C;\n'
  'D_8009B374 = 0x8009B374;\n'
  'CdReady = 0x8007A488;\n'
  'D_800BE998 = 0x800BE998;\n'
  'D_800C0DC8 = 0x800C0DC8;\n'
  'D_800A34A0 = 0x800A34A0;\n'
  'D_8009B32C = 0x8009B32C;\n'
  'D_8009B338 = 0x8009B338;\n'
  'D_8009B33C = 0x8009B33C;\n'
  'D_8009B340 = 0x8009B340;\n'
  'D_800A8020 = 0x800A8020;\n'
  'D_8009B334 = 0x8009B334;\n'
  'mem2mem = 0x8007CE80;\n'
  'dma_execute = 0x8007CEAC;\n'
  'D_8009B35C = 0x8009B35C;\n'
  'D_800C0DC0 = 0x800C0DC0;\n'
  'D_800B6918 = 0x800B6918;\n'
  'D_800B8620 = 0x800B8620;\n'
  'D_800A8018 = 0x800A8018;\n'
  'D_800A5D54 = 0x800A5D54;\n'
  'D_800BE9E4 = 0x800BE9E4;\n'
  'init_ring_status = 0x8007C444;\n'
  'D_800C0DBC = 0x800C0DBC;\n'
  'D_800B0CCC = 0x800B0CCC;\n'
  'D_800C20C4 = 0x800C20C4;\n'
  'D_800C0DC4 = 0x800C0DC4;\n'
  'D_800B6914 = 0x800B6914;\n'
  'data_ready_callback = 0x8007C214;\n'
  'SECTIONS { .text 0x8007c564 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }')]


class PsyqMathDisplayStreamFourTests(unittest.TestCase):
    @unittest.skipUnless(shutil.which("mipsel-none-elf-ld"), "MIPS binutils unavailable")
    def test_all_linked_bytes_match_retail(self):
        for name, size, digest, script in CASES:
            with self.subTest(function=name), tempfile.TemporaryDirectory() as directory:
                work = pathlib.Path(directory)
                obj, elf, data = work / "code.o", work / "code.elf", work / "code.bin"
                subprocess.run(["tools/scripts/cc.sh", f"src/main/psyq/{name}.c", str(obj)],
                               cwd=ROOT, check=True, capture_output=True)
                linker_script = work / "code.ld"
                linker_script.write_text(script)
                subprocess.run(["mipsel-none-elf-ld", "-T", str(linker_script),
                                str(obj), "-o", str(elf)], check=True)
                subprocess.run(["mipsel-none-elf-objcopy", "-O", "binary",
                                "--only-section=.text", str(elf), str(data)], check=True)
                code = data.read_bytes()
                self.assertEqual(len(code), size)
                self.assertEqual(hashlib.sha256(code).hexdigest(), digest)
