"""Retail-free exact-byte checks for SPU startup and shutdown, including relocations."""
import hashlib
import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]

# SpuStart at 0x8007D15C, SpuQuit at 0x80085984.
CASES = [('SpuStart',
  112,
  '7f204cbc87e3cfa107a01cb94317e7bfd2733e7cc2aba8c89edbf7c8c287ef7e',
  'D_8009B3EC = 0x8009B3EC;\n'
  'EnterCriticalSection = 0x80072714;\n'
  '_spu_FiDMA = 0x8007D614;\n'
  '_SpuDataCallback = 0x8007DD14;\n'
  'OpenEvent = 0x800726E4;\n'
  '_spu_EVdma = 0x8009B384;\n'
  'EnableEvent = 0x80072704;\n'
  'ExitCriticalSection = 0x80072724;\n'
  'SECTIONS { .text 0x8007d15c : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }'),
 ('s_q',
  116,
  '2a097f77cd77ee87e6df9187c0ec4a3524d90ecc3d95fab8c6b876438fcfcb4a',
  'D_8009B3EC = 0x8009B3EC;\n'
  'EnterCriticalSection = 0x80072714;\n'
  '_spu_transferCallback = 0x8009B434;\n'
  '_spu_IRQCallback = 0x8009B438;\n'
  '_SpuDataCallback = 0x8007DD14;\n'
  '_spu_EVdma = 0x8009B384;\n'
  'CloseEvent = 0x800857E4;\n'
  'DisableEvent = 0x800857F4;\n'
  'ExitCriticalSection = 0x80072724;\n'
  'SECTIONS { .text 0x80085984 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }')]


class SpuLifecycleTests(unittest.TestCase):
    @unittest.skipUnless(shutil.which("mipsel-none-elf-ld"), "MIPS binutils unavailable")
    def test_all_linked_bytes_match_retail(self):
        for name, size, digest, script in CASES:
            with self.subTest(function=name), tempfile.TemporaryDirectory() as directory:
                work = pathlib.Path(directory)
                obj, elf, data = work / "code.o", work / "code.elf", work / "code.bin"
                subprocess.run(["tools/scripts/cc.sh", f"src/main/psyq/libspu/{name}.c", str(obj)],
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
