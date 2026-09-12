"""Exact linked-byte regressions for contiguous-slot search and slot compaction."""
import hashlib
import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]

# Entire linked text ranges at their retail addresses.
CASES = [('item/Inv_FindFirstEmptySlot',
  316,
  'c9d79218d158c686643796e7641610c0660987624d9e6e5eb192617e2f91379c',
  'D_8009D064 = 0x8009D064;\n'
  'D_8009D058 = 0x8009D058;\n'
  'D_8009D050 = 0x8009D050;\n'
  'D_8009D048 = 0x8009D048;\n'
  'D_800C0E48 = 0x800C0E48;\n'
  'Inv_GetAyaSlotLimit = 0x80052F70;\n'
  'D_8009D05C = 0x8009D05C;\n'
  '_gp = 0x8009CD70;\n'
  'SECTIONS { .text 0x8005382c : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }'),
 ('item/Inv_CompactActiveListSlots',
  340,
  '92e03cc5341f56da9b4f4785622a9aff53e901a49b0303791fca286ba255f95b',
  'D_8009D050 = 0x8009D050;\n'
  'D_8009D048 = 0x8009D048;\n'
  'D_8009D06C = 0x8009D06C;\n'
  'D_800C0E20 = 0x800C0E20;\n'
  'Inv_GetBonusSlotCount = 0x80051E58;\n'
  'D_800C0E0C = 0x800C0E0C;\n'
  '_gp = 0x8009CD70;\n'
  'SECTIONS { .text 0x80054cf8 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }')]


class InventorySlotPairTests(unittest.TestCase):
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
