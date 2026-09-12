"""Exact linked-byte regressions for sector DMA and the LIBDS file lookup."""
import hashlib
import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]

# Retail TUs at 0x8007BF44 (492 bytes) and 0x80081414 (736 bytes).
CASES = [('libcd/sector_read',
  492,
  'e9f1ff7e2cc8ea7a5bf11be1f838ef39ebab8d2906c4d0817f980aa9eb3a1bc3',
  'D_8009B27C = 0x8009B27C;\n'
  'D_8009B288 = 0x8009B288;\n'
  'D_8009B2B0 = 0x8009B2B0;\n'
  'D_8009B28C = 0x8009B28C;\n'
  'D_8009B2B4 = 0x8009B2B4;\n'
  'D_8009B2B8 = 0x8009B2B8;\n'
  'D_8009B2BC = 0x8009B2BC;\n'
  'D_8009B2C0 = 0x8009B2C0;\n'
  'g_CdRegIndexBase = 0x8009B27C;\n'
  'g_CdRegResponse = 0x8009B288;\n'
  'g_CdRegRequest = 0x8009B28C;\n'
  'g_CdRegDmaControl = 0x8009B2C0;\n'
  'SECTIONS { .text 0x8007bf44 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }'),
 ('libds/DsSearchFile',
  736,
  '886b6ae1a7bf15dc49aad5ae9616ca57211d475c883b28649dac25048849ba9b',
  'CdRom_GetDiskType = 0x8007F7A8;\n'
  'D_8009B6E0 = 0x8009B6E0;\n'
  'DS_newmedia = 0x80081714;\n'
  'DS_searchdir = 0x800819D8;\n'
  'D_8009AFC0 = 0x8009AFC0;\n'
  'D_80011E6C = 0x80011E6C;\n'
  'printf = 0x80071A74;\n'
  'D_80011E88 = 0x80011E88;\n'
  'DS_cachefile = 0x80081A7C;\n'
  'D_80011EA0 = 0x80011EA0;\n'
  'puts = 0x80073C5C;\n'
  'D_80011EBC = 0x80011EBC;\n'
  'D_800A36B8 = 0x800A36B8;\n'
  '_cmp = 0x800816F4;\n'
  'D_80011EDC = 0x80011EDC;\n'
  'D_80011EE8 = 0x80011EE8;\n'
  'SECTIONS { .text 0x80081414 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }')]


class SectorFileSearchTests(unittest.TestCase):
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
