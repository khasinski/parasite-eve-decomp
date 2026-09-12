"""Retail-free exact-byte checks for LIBCD ring teardown and access, including relocations."""
import hashlib
import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]

# Retail instruction streams at 0x8007A2A4 and 0x8007C484.
CASES = [('StUnSetRing',
  128,
  '82d7dbeefed8d22c2f5c115863222cae7bd34f45ee458cf35155659cf2bc74b9',
  'EnterCriticalSection = 0x80072714;\n'
  'D_8009AFD8 = 0x8009AFD8;\n'
  'DsDataCallback = 0x800824F0;\n'
  'DsSyncCallback = 0x800824C8;\n'
  'CdDataCallback = 0x8007A8EC;\n'
  'CdReadyCallback = 0x8007A4BC;\n'
  'D_8009AF1C = 0x8009AF1C;\n'
  'D_8009AF28 = 0x8009AF28;\n'
  'ExitCriticalSection = 0x80072724;\n'
  'SECTIONS { .text 0x8007a2a4 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }'),
 ('StGetNext',
  184,
  'ffecaf09d4884883f522487717299cec0311a543aadc4df6689919103ac0c1b7',
  'D_800BE9EC = 0x800BE9EC;\n'
  'StRingAddr = 0x800C0DC8;\n'
  'g_CdStreamEndSector = 0x800C0DBC;\n'
  'StRingSize = 0x800C20C4;\n'
  'SECTIONS { .text 0x8007c484 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }')]


class LibcdRingAccessTests(unittest.TestCase):
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
