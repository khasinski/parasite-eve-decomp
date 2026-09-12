"""Retail-free exact-byte checks for CD read setup and progress, including relocations."""
import hashlib
import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]

# SHA-256 of retail bytes at 0x80080E34 and 0x80080F98, including relocations.
CASES = [('CdRom_StartRead',
  304,
  '942dcdff96fef7244271766c9c10825fd90c12df3d5e507a0e3a9d4f05646d51',
  'D_8009B6CC = 0x8009B6CC;\n'
  'DsRead_IsBusy = 0x800822AC;\n'
  'CdRom_GetCurrentPos = 0x80080CDC;\n'
  'CdRom_SetMode2Callback = 0x80080F64;\n'
  'Render_BuildParticleFrame = 0x8007F0C8;\n'
  'VSync = 0x80073A44;\n'
  'D_8009B6C4 = 0x8009B6C4;\n'
  'Render_StepParticleCallback = 0x80081110;\n'
  'DsDataCallback = 0x800824F0;\n'
  'SECTIONS { .text 0x80080e34 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }'),
 ('CdRom_ReadProgressCallback',
  376,
  '336ac704bb4b204ec45759f8ebaf655e591f5fd9d7b9913410a40c61f3d895dc',
  'VSync = 0x80073A44;\n'
  'D_8009B6C8 = 0x8009B6C8;\n'
  'CdRom_IsBusy2 = 0x80080B04;\n'
  'Save_ProcessDataCallback = 0x80081268;\n'
  'D_8009B6D0 = 0x8009B6D0;\n'
  'CdRom_IsBusy = 0x80080AE4;\n'
  'SECTIONS { .text 0x80080f98 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }')]


class CdReadSetupProgressTests(unittest.TestCase):
    @unittest.skipUnless(shutil.which("mipsel-none-elf-ld"), "MIPS binutils unavailable")
    def test_all_linked_bytes_match_retail(self):
        for name, size, digest, script in CASES:
            with self.subTest(function=name), tempfile.TemporaryDirectory() as directory:
                work = pathlib.Path(directory)
                obj, elf, data = work / "code.o", work / "code.elf", work / "code.bin"
                subprocess.run(["tools/scripts/cc.sh", f"src/main/cdrom/{name}.c", str(obj)],
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
