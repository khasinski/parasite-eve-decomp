"""Full linked-byte regressions for Psy-Q LIBPAD/LIBDS functions."""
import hashlib
import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]

# Entire linked text ranges at their retail addresses.
CASES = [('libpad/padseqd',
  1140,
  'e080b43d7fdd01895057432b2eb8509d80c259ecb79f25aa77b5711f3411b504',
  'D_8009B728 = 0x8009B728;\n_padCmdParaMode = 0x80083E50;\n_padRecvAtLoadInfo = 0x80083644;\n_padSendAtLoadInfo = 0x800835C0;\ng_MemCardIsTransferActiveFn = 0x8009B740;\ng_MemCardResponseHandler = 0x8009B744;\ng_MemCardStateDispatchFn = 0x8009B73C;\nSECTIONS { .text 0x80084b44 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) *(.mdebug) } }'),
 ('libpad/padcmd',
  2388,
  'f4d9851b3c04b09b55295b3653fb9b46c24182d46125ce755235a0d4ac168e3e',
  'D_8009B728 = 0x8009B728;\nD_8009B740 = 0x8009B740;\nD_800A5AD0 = 0x800A5AD0;\nfunc_80083C20 = 0x80083C20;\nfunc_80083C3C = 0x80083C3C;\ng_MemCardIsTransferActiveFn = 0x8009B740;\nSECTIONS { .text 0x800835a4 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) *(.mdebug) } }'),
 ('libds/dssys_2_3',
  1380,
  'b7050732b8e92fb15e3d90b4f1314937417e99711c63adb0f137f19178bc1494',
  'CQ_delete_command = 0x8007E5C4;\nDS_cw = 0x8007FB44;\nDS_system_status = 0x8007FBF0;\nD_800A3510 = 0x800A3510;\nD_800A3540 = 0x800A3540;\nD_800A3600 = 0x800A3600;\nD_800A3604 = 0x800A3604;\nD_800A3608 = 0x800A3608;\nD_800A3610 = 0x800A3610;\nD_800A3690 = 0x800A3690;\nD_800B8AB0 = 0x800B8AB0;\ng_CdPendingReadCount = 0x800A3608;\nrescpy = 0x80080998;\nSECTIONS { .text 0x8007e6b0 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) *(.mdebug) } }')]


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
