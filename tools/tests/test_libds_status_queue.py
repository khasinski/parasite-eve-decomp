"""Retail-free exact-byte checks for two LIBDS routines, including relocations."""
import hashlib
import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]

# Retail instruction streams at 0x800809E0 and 0x8007E5C4 respectively.
CASES = [('DS_status',
  220,
  'a293e10b7e80565ad5ddc9bd56b25ccf7d641d30b2a424264811f043dc7c4fef',
  'D_80011D74 = 0x80011D74;\n'
  'printf = 0x80071A74;\n'
  'D_8009B574 = 0x8009B574;\n'
  'D_80011D94 = 0x80011D94;\n'
  'D_80011DB0 = 0x80011DB0;\n'
  'D_80011DD8 = 0x80011DD8;\n'
  'D_80011DF8 = 0x80011DF8;\n'
  'D_800A36A0 = 0x800A36A0;\n'
  'D_800A36A4 = 0x800A36A4;\n'
  'D_800A36A8 = 0x800A36A8;\n'
  'D_80011E0C = 0x80011E0C;\n'
  'DsRead_IsBusy = 0x800822AC;\n'
  'D_80011E44 = 0x80011E44;\n'
  'D_80011E3C = 0x80011E3C;\n'
  'D_80011E4C = 0x80011E4C;\n'
  'SECTIONS { .text 0x800809e0 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }'),
 ('Spu_DrainQueueEntry',
  236,
  'e7239bb1c9421072ce01869eae99b016c60da5f87bdf6fbd1c2a1fbf5c3a47c6',
  'g_CdDsReadQueueState = 0x800A3600;\n'
  'g_CdDsReadQueue = 0x800A3540;\n'
  'g_CdDsReadIndex = 0x800A3604;\n'
  'SECTIONS { .text 0x8007e5c4 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }')]


class LibdsStatusQueueTests(unittest.TestCase):
    @unittest.skipUnless(shutil.which("mipsel-none-elf-ld"), "MIPS binutils unavailable")
    def test_all_linked_bytes_match_retail(self):
        for name, size, digest, script in CASES:
            with self.subTest(function=name), tempfile.TemporaryDirectory() as directory:
                work = pathlib.Path(directory)
                obj, elf, data = work / "code.o", work / "code.elf", work / "code.bin"
                subprocess.run(["tools/scripts/cc.sh", f"src/main/psyq/libds/{name}.c", str(obj)],
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
