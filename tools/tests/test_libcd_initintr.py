"""Retail-free exact-byte checks for LIBCD interrupt initialization, including relocations."""
import hashlib
import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]

# BIOS_1 text at 0x8007BAC0: unchanged CD_initvol (240 bytes),
# followed by CD_initintr (76 bytes). Data and strings retain retail addresses.
CASES = [('bios',
  316,
  '30f818c95d807b9a809ae43802eb7518448196b61ea18a466dc9c47e4ee45a3f',
  'D_8009B290 = 0x8009B290;\n'
  'g_CdRegIndexBase = 0x8009B27C;\n'
  'g_CdRegDataWrite = 0x8009B284;\n'
  'g_CdRegResponse = 0x8009B288;\n'
  'g_CdRegPort1 = 0x8009B280;\n'
  'g_CdReadyCallback = 0x8009AFB8;\n'
  'g_CdSyncCallback = 0x8009AFB4;\n'
  'D_8009AFC8 = 0x8009AFC8;\n'
  'D_8009AFC4 = 0x8009AFC4;\n'
  'ResetCallback = 0x80073C94;\n'
  'Cd_SetIntrMask = 0x8007C13C;\n'
  'InterruptCallback = 0x80073CC4;\n'
  'SECTIONS { .text 0x8007bac0 : SUBALIGN(4) { *(.text .text.*) } .data 0x8009AFB4 : SUBALIGN(4) { '
  '*(.data .data.*) } .rodata 0x800119DC : SUBALIGN(4) { *(.rodata .rodata.*) } /DISCARD/ : { '
  '*(.reginfo) *(.mdebug) } }')]


class LibcdInterruptInitTests(unittest.TestCase):
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
