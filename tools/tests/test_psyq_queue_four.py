"""Full linked text and jump-table regressions for Psy-Q DSSYS_2 and LIBCARD units."""
import hashlib
import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]

# Entire linked text ranges at their retail addresses.
CASES = [('libds/DsCommand',
  1924,
  'dfbaaa331bd8eccbd38bacf391474e1c10d75a9cbe543f29b312bc2d27093171',
  'D_8009B4BC = 0x8009B4BC;\n'
  'D_800A3608 = 0x800A3608;\n'
  'D_8009B53C = 0x8009B53C;\n'
  'CQ_last_queue = 0x8007E6B0;\n'
  'parcpy = 0x80080950;\n'
  'rescpy = 0x80080998;\n'
  'DS_system_status = 0x8007FBF0;\n'
  'DS_sync = 0x8007FC64;\n'
  'DsPosToInt = 0x80080C48;\n'
  'D_800A3604 = 0x800A3604;\n'
  'D_800A3540 = 0x800A3540;\n'
  'D_800A3690 = 0x800A3690;\n'
  'D_800A3610 = 0x800A3610;\n'
  'D_800A3500 = 0x800A3500;\n'
  'D_800A3614 = 0x800A3614;\n'
  'D_800A3618 = 0x800A3618;\n'
  'D_800A361C = 0x800A361C;\n'
  'CQ_execute = 0x8007E8F4;\n'
  'SECTIONS { .text 0x8007ee84 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }\n'
  'SECTIONS { .rodata 0x80011C9C : SUBALIGN(4) { *(.rodata .rodata.*) } }'),
 ('libcard/patch_4',
  52,
  '0bc985425d463a2e112f769b960b427690574663c0f7db56c71594c0467fb564',
  'func_8007E344 = 0x8007E344;\n'
  'func_8007E3B4 = 0x8007E3B4;\n'
  'SECTIONS { .text 0x8007e4e0 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }')]


class PsyqQueueFourTests(unittest.TestCase):
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

                if name == "libds/DsCommand":
                    subprocess.run(["mipsel-none-elf-objcopy", "-O", "binary",
                                    "--only-section=.rodata", str(elf), str(data)], check=True)
                    table = data.read_bytes()
                    self.assertEqual(len(table), 100)
                    self.assertEqual(hashlib.sha256(table).hexdigest(),
                                     "77a186cc8a445c7ed8ce06f500af33176871016a514d08ce756ebc3b887d33af")
