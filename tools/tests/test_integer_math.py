"""Exact linked-byte regressions for the integer conversion and wide multiplication."""
import hashlib
import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]

# Entire linked text ranges at their retail addresses.
CASES = [('math/Math_Int32ToDouble',
  256,
  'e573de21ab0c5cc4ee09974e395737ca6cf3330c3cecc77d21b259f6365b46ba',
  'Math_Shift64 = 0x80073294;\n'
  'SECTIONS { .text 0x80073454 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }'),
 ('math/Math_Mul32To64',
  236,
  'c05005c411d44159dcc1fbe8fab722366870bea126932e8b3cc666b9897cbf05',
  'Math_Add64 = 0x80072DF4;\n'
  'SECTIONS { .text 0x800738d8 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }')]


class IntegerMathTests(unittest.TestCase):
    @unittest.skipUnless(shutil.which("mipsel-none-elf-ld"), "MIPS binutils unavailable")
    def test_all_linked_bytes_match_retail(self):
        for name, size, digest, script in CASES:
            with self.subTest(function=name), tempfile.TemporaryDirectory() as directory:
                work = pathlib.Path(directory)
                obj, elf, data = work / "code.o", work / "code.elf", work / "code.bin"
                subprocess.run(["tools/scripts/cc.sh", f"src/main/{name}.c", str(obj)],
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
