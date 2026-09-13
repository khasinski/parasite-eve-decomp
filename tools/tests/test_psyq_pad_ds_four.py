"""Full linked-byte regression for four Psy-Q LIBPAD/LIBDS functions."""
import hashlib
import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]

# Entire linked text ranges at their retail addresses.
CASES = [('libpad/DirFailAuto',
  220,
  '47e8a6fba0fec4977c4010c8939c74c7c6df252963140491477700e6124c0c7e',
  'D_8009B728 = 0x8009B728;\n'
  'SECTIONS { .text 0x80084eb0 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }'),
 ('libpad/PadReadPayload',
  628,
  'ba83b67dddbf9a9076dfcba0b38d448dbf688b2b114704e9ccf1f1732f54736d',
  'D_800A5AD0 = 0x800A5AD0;\n'
  'SECTIONS { .text 0x80083944 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }'),
 ('libds/dssys2_flush',
  496,
  'cc34f5b089194c02b861b44bdc80af68287db290a4f3fd5cf00e639d62e56e7e',
  'D_800A3600 = 0x800A3600;\n'
  'D_800A3540 = 0x800A3540;\n'
  'CdRom_EnqueueCmd = 0x8007EB88;\n'
  'D_800A3608 = 0x800A3608;\n'
  'D_800A3604 = 0x800A3604;\n'
  'SECTIONS { .text 0x8007e704 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }'),
 ('libds/dssys1_sync',
  484,
  'bac5290de82e938bb8034314056c4e9780ad94fab23e8224e2d4ad252fb94546',
  'D_8009B558 = 0x8009B558;\n'
  'D_8009B581 = 0x8009B581;\n'
  'D_8009B582 = 0x8009B582;\n'
  'D_8009B586 = 0x8009B586;\n'
  'D_8009B587 = 0x8009B587;\n'
  'D_8009B574 = 0x8009B574;\n'
  'g_CdSeekState = 0x8009B56C;\n'
  'D_800A36A4 = 0x800A36A4;\n'
  'g_DsReadSysEnabled = 0x8009B554;\n'
  'SECTIONS { .text 0x80080220 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }\n'
  'SECTIONS { .rodata 0x80011D0C : SUBALIGN(4) { *(.rodata .rodata.*) } }')]


class PsyqPadDsFourTests(unittest.TestCase):
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

                if name == "libds/dssys1_sync":
                    subprocess.run(["mipsel-none-elf-objcopy", "-O", "binary",
                                    "--only-section=.rodata", str(elf), str(data)],
                                   check=True)
                    table = data.read_bytes()
                    self.assertEqual(len(table), 104)
                    self.assertEqual(hashlib.sha256(table).hexdigest(),
                                     "236c0e52fbe57b0c488c89ed3b79342be22b3d94f3bd2b3c9f8a08a938425317")
