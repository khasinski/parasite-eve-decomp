"""Retail-free exact-byte checks for counter masking and SPU address rounding, including relocations."""
import hashlib
import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]

# Complete retail TUs: counter at 0x80085814 (StopRCnt at +0x104),
# SPU register helpers at 0x8007DAE0 (_spu_FsetRXXa at +0x44).
CASES = [('libapi/counter',
  364,
  'be6a3c32260308417c8b34ea9797a82befe166d3bb582ccc42a8043bc9ce3b90',
  'D_8009B7D0 = 0x8009B7D0;\n'
  'D_8009B7CC = 0x8009B7CC;\n'
  'D_8009B7D4 = 0x8009B7D4;\n'
  'SECTIONS { .text 0x80085814 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }'),
 ('libspu/spu_register_write',
  552,
  '51d8aaa835d28a54bae2e84948b52c900867d28f05b95065295c2c3fdb78f3e0',
  '_spu_RXX = 0x8009B3FC;\n'
  '_spu_mem_mode_plus = 0x8009B424;\n'
  '_spu_mem_mode = 0x8009B420;\n'
  '_spu_mem_mode_unit = 0x8009B428;\n'
  '_spu_mem_mode_unitM = 0x8009B42C;\n'
  '_spu_sys_pcr = 0x8009B40C;\n'
  'D_8009B410 = 0x8009B410;\n'
  'SECTIONS { .text 0x8007dae0 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }')]


class CounterSpuAddressTests(unittest.TestCase):
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
