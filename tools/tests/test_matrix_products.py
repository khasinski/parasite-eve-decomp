"""Exact linked-byte regressions for the two GTE matrix multiplication objects."""
import hashlib
import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]

# Whole retail TUs, including their original trailing alignment words.
CASES = [('libgte/MulMatrix0',
  272,
  '7a49e69bb3a862fd80e3ad3cfb677ca2a9a1700cb4b77c185d22661a19ebbef4',
  'SECTIONS { .text 0x800785d4 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }'),
 ('libgte/MulRotMatrix',
  240,
  '5feab422d94002fbb586c007cc2de09ccca57fa5dfae96755e38c67bda55ad49',
  'SECTIONS { .text 0x800786e4 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }')]


class MatrixProductTests(unittest.TestCase):
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
