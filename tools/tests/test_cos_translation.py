"""Exact linked-byte regressions for the cosine lookup and matrix translation."""
import hashlib
import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]

# Complete retail TUs: rcos at 0x80077DC4 and TransMatrix at 0x80078C94.
CASES = [('libgte/geo_01',
  160,
  '42a01d993875f0235428e98e55a064a5ece684098ceefe8a4b7152870e7959af',
  'D_8009589C = 0x8009589C;\n'
  'D_8009509C = 0x8009509C;\n'
  'D_8009409C = 0x8009409C;\n'
  'SECTIONS { .text 0x80077dc4 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }'),
 ('libgte/mtx_07',
  36,
  'e7c83d5bccd6bc52a2f15133283c464864c5087e31234fa07c436e2946e8eac4',
  'SECTIONS { .text 0x80078c94 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }')]


class CosTranslationTests(unittest.TestCase):
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
