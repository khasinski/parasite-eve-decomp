"""Full linked-byte regression for all five retail Psy-Q RotMatrix variants."""
import hashlib
import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]

# Entire linked text ranges at their retail addresses.
CASES = [('libgte/RotMatrix',
  652,
  '19a788c448e9f29638e673adaa9be46de34a81b53a99fbb389b4833a7a77ac7c',
  'D_800966EC = 0x800966EC;\n'
  'SECTIONS { .text 0x800794c4 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }'),
 ('libgte/RotMatrixYXZ',
  652,
  '3f3b635a4043f2431742fdd17f3c0b6151693091b20996071e6f360e41ab915d',
  'D_800966EC = 0x800966EC;\n'
  'SECTIONS { .text 0x80079754 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }'),
 ('libgte/RotMatrixZYX',
  652,
  'eb29f1f088bca83efc058df957cfe2cd49bcfbd3a215d41df7986888d2b9bba3',
  'D_800966EC = 0x800966EC;\n'
  'SECTIONS { .text 0x800799e4 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }'),
 ('libgte/RotMatrixY',
  408,
  '6ec71ba3f3bc10c8c31db8b2e186f0609c31ba04a46bfe124ea36da8ae2d1d89',
  'D_800966EC = 0x800966EC;\n'
  'SECTIONS { .text 0x80079c74 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }'),
 ('libgte/RotMatrixZ',
  408,
  '01dc9ce87f9f78efdbd99d5740e80f3d5a92d6973a621874ea178834426546f2',
  'D_800966EC = 0x800966EC;\n'
  'SECTIONS { .text 0x80079e14 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }')]


class PsyqRotationMatrixTests(unittest.TestCase):
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
