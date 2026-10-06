"""Full linked text and jump-table regressions for Psy-Q DSSYS_2 and LIBCARD units."""
import hashlib
import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]

# Entire linked text ranges at their retail addresses.
CASES = [('libds/dssys_2_5',
  2864,
  'a3435b2be0385eea6217462a9a31e49f5c2172708ba9cf630b48ef548067df6f',
  'CQ_error_flush = 0x8007E704;\nCQ_execute = 0x8007E8F4;\nCQ_last_queue = 0x8007E6B0;\nDS_close = 0x8007FB04;\nDS_cw = 0x8007FB44;\nDS_lastcom = 0x8007FC08;\nDS_ready = 0x8007FC88;\nDS_restart = 0x80080930;\nDS_shell_open = 0x8007FCAC;\nDS_status = 0x8007FC54;\nDS_stop = 0x800808BC;\nDS_sync = 0x8007FC64;\nDS_system_status = 0x8007FBF0;\nD_8009B4BC = 0x8009B4BC;\nD_8009B53C = 0x8009B53C;\nD_800A3500 = 0x800A3500;\nD_800A3520 = 0x800A3520;\nD_800A3530 = 0x800A3530;\nD_800A3540 = 0x800A3540;\nD_800A3604 = 0x800A3604;\nD_800A3608 = 0x800A3608;\nD_800A3610 = 0x800A3610;\nD_800A3614 = 0x800A3614;\nD_800A3618 = 0x800A3618;\nD_800A361C = 0x800A361C;\nD_800A3690 = 0x800A3690;\nD_800B8AB4 = 0x800B8AB4;\nDsEndReadySystem = 0x80081DF8;\nDsPosToInt = 0x80080C48;\ng_CdDsReadIndex = 0x800A3604;\ng_CdDsReadQueue = 0x800A3540;\ng_CdDsReadQueueState = 0x800A3600;\ng_CdPendingReadCount = 0x800A3608;\ng_DsStartCallback = 0x800B8AB8;\nparcpy = 0x80080950;\nrescpy = 0x80080998;\nSECTIONS { .text 0x8007ee64 : SUBALIGN(4) { *(.text .text.*) } .rodata 0x80011c9c : SUBALIGN(4) { *(.rodata) } /DISCARD/ : { *(.reginfo) *(.mdebug) } }'),
 ('libcard/patch_4',
  52,
  '0bc985425d463a2e112f769b960b427690574663c0f7db56c71594c0467fb564',
  'func_8007E344 = 0x8007E344;\nfunc_8007E3B4 = 0x8007E3B4;\nSECTIONS { .text 0x8007e4e0 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) *(.mdebug) } }')]


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

                if name == "libds/dssys_2_5":
                    subprocess.run(["mipsel-none-elf-objcopy", "-O", "binary",
                                    "--only-section=.rodata", str(elf), str(data)], check=True)
                    table = data.read_bytes()
                    self.assertEqual(len(table), 100)
                    self.assertEqual(hashlib.sha256(table).hexdigest(),
                                     "77a186cc8a445c7ed8ce06f500af33176871016a514d08ce756ebc3b887d33af")
