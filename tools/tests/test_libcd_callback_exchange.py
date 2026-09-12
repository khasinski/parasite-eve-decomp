"""Retail-free exact-byte checks for LIBCD callback exchange, including relocations."""
import hashlib
import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]

# Entire sys TU at 0x8007A3CC, including the unchanged six neighbors.
# CdSyncCallback and CdReadyCallback occupy its final 40 bytes.
CASES = [('sys',
  260,
  '8a39029de84423013c1f0477058380a6d1b63ac8b7a3ab85696f9de8eae81f91',
  'CD_flush = 0x8007B9EC;\n'
  'g_CdReadCallback = 0x8009AFC0;\n'
  'g_CdCmdNameTable = 0x8009AFDC;\n'
  'func_800119CC = 0x800119CC;\n'
  'g_CdIntrStringTable = 0x8009B05C;\n'
  'CD_sync = 0x8007B010;\n'
  'CD_ready = 0x8007B290;\n'
  'D_8009AFB4 = 0x8009AFB4;\n'
  'D_8009AFB8 = 0x8009AFB8;\n'
  'SECTIONS { .text 0x8007A3CC : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }')]


class LibcdCallbackExchangeTests(unittest.TestCase):
    @unittest.skipUnless(shutil.which("mipsel-none-elf-ld"), "MIPS binutils unavailable")
    def test_all_linked_bytes_match_retail(self):
        for name, size, digest, script in CASES:
            with self.subTest(function=name), tempfile.TemporaryDirectory() as directory:
                work = pathlib.Path(directory)
                obj, elf, data = work / "code.o", work / "code.elf", work / "code.bin"
                subprocess.run(["tools/scripts/cc.sh", f"src/main/psyq/libcd/{name}.c", str(obj)],
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
