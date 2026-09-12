"""Exact linked-byte regressions for the arithmetic shift and firstfile hook."""
import hashlib
import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]

# Entire linked text ranges at their retail addresses.
CASES = [('math/Math_Add64WithShift',
  188,
  'f5ef0b49b72cac65d49205ac7b5aff69bc77a89b1374c3a4f7d0a6b091f66076',
  'SECTIONS { .text 0x80072ea4 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }'),
 ('main/Sys_FirstFileHookCallback',
  256,
  'a6a416fc6d48b744dabdcde64ae5b8ba6287596747e601a0b1bc22cc1eef1711',
  'D_800A32D0 = 0x800A32D0;\n'
  'D_800A32D8 = 0x800A32D8;\n'
  'strcmp = 0x80072A54;\n'
  'SECTIONS { .text 0x80072950 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }')]


class ArithmeticShiftFirstFileTests(unittest.TestCase):
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
