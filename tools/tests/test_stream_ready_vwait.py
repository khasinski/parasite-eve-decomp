"""Retail-free exact-byte checks for stream completion and VSync waiting, including relocations."""
import hashlib
import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]

# data_ready_callback at 0x8007C214 and v_wait at 0x80073BBC.
CASES = [('libcd/data_ready_callback',
  140,
  'bfe0812b6e5d671c9b90c55f63306291ed8a733953e57a1378930757146bc775',
  'g_CdStreamRingReadSlot = 0x800BE9E4;\n'
  'g_CdRingBufPtr = 0x800C0DC8;\n'
  'D_800A3490 = 0x800A3490;\n'
  'D_800BE998 = 0x800BE998;\n'
  'g_StrDataReadyCallback = 0x800B0CC8;\n'
  'D_800A3494 = 0x800A3494;\n'
  'g_CdStreamDataReadyFlag = 0x800B89F4;\n'
  'SECTIONS { .text 0x8007c214 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }'),
 ('libapi/v_wait',
  152,
  'd9e7e8c8bdce7a66b51154f3f3109caca27f68c5e6f8b469f716d4bfbd806277',
  'g_VSyncCount = 0x800956AC;\n'
  'D_800116FC = 0x800116FC;\n'
  'puts = 0x80073C5C;\n'
  'ChangeClearPAD = 0x80073C74;\n'
  'ChangeClearRCnt = 0x80073C84;\n'
  'SECTIONS { .text 0x80073bbc : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }')]


class StreamReadyVwaitTests(unittest.TestCase):
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
