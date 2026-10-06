"""Full linked-byte regression for four Psy-Q LIBPAD/LIBDS translation units."""
import hashlib
import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]

# Entire linked text ranges at their retail addresses.
CASES = [('libds/dssys_1_6',
  1368,
  'dce5b5d86421ed0faa869592a01a82b86f177654f0df0c1a0fbc90e0aefbc1d2',
  'D_8009B558 = 0x8009B558;\nD_8009B574 = 0x8009B574;\nD_8009B578 = 0x8009B578;\nD_8009B581 = 0x8009B581;\nD_8009B582 = 0x8009B582;\nD_8009B586 = 0x8009B586;\nD_8009B587 = 0x8009B587;\nD_800A36A4 = 0x800A36A4;\nD_800A36A8 = 0x800A36A8;\nD_800A36AC = 0x800A36AC;\ng_CdSeekState = 0x8009B56C;\ng_DsReadSysEnabled = 0x8009B554;\nSECTIONS { .text 0x80080220 : SUBALIGN(4) { *(.text .text.*) } .rodata 0x80011d0c : SUBALIGN(4) { *(.rodata) } /DISCARD/ : { *(.reginfo) *(.mdebug) } }'),
 ('libpad/padseqd',
  1140,
  'e080b43d7fdd01895057432b2eb8509d80c259ecb79f25aa77b5711f3411b504',
  'D_8009B728 = 0x8009B728;\n_padCmdParaMode = 0x80083E50;\n_padRecvAtLoadInfo = 0x80083644;\n_padSendAtLoadInfo = 0x800835C0;\ng_MemCardIsTransferActiveFn = 0x8009B740;\ng_MemCardResponseHandler = 0x8009B744;\ng_MemCardStateDispatchFn = 0x8009B73C;\nSECTIONS { .text 0x80084b44 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) *(.mdebug) } }'),
 ('libds/dssys_2_3',
  1380,
  'b7050732b8e92fb15e3d90b4f1314937417e99711c63adb0f137f19178bc1494',
  'CQ_delete_command = 0x8007E5C4;\nDS_cw = 0x8007FB44;\nDS_system_status = 0x8007FBF0;\nD_800A3510 = 0x800A3510;\nD_800A3540 = 0x800A3540;\nD_800A3600 = 0x800A3600;\nD_800A3604 = 0x800A3604;\nD_800A3608 = 0x800A3608;\nD_800A3610 = 0x800A3610;\nD_800A3690 = 0x800A3690;\nD_800B8AB0 = 0x800B8AB0;\ng_CdPendingReadCount = 0x800A3608;\nrescpy = 0x80080998;\nSECTIONS { .text 0x8007e6b0 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) *(.mdebug) } }'),
 ('libpad/padmain',
  3036,
  '2eae8f307282ab6fe44724b5d8c17ad6ff39316533ed88103ef9de565b4799a0',
  'ChangeClearRCnt = 0x80073C84;\nD_8009B724 = 0x8009B724;\nD_8009B728 = 0x8009B728;\nD_8009B744 = 0x8009B744;\nD_8009B748 = 0x8009B748;\nD_8009B74C = 0x8009B74C;\nD_8009B758 = 0x8009B758;\nD_8009B75C = 0x8009B75C;\nD_8009B764 = 0x8009B764;\nD_8009B768 = 0x8009B768;\nD_8009B76C = 0x8009B76C;\nD_8009B774 = 0x8009B774;\nD_8009B778 = 0x8009B778;\nD_8009B77C = 0x8009B77C;\nD_8009B784 = 0x8009B784;\nD_8009B788 = 0x8009B788;\nD_8009B7A8 = 0x8009B7A8;\nD_800A5AB0 = 0x800A5AB0;\nD_800A5AB4 = 0x800A5AB4;\nD_800A5AC0 = 0x800A5AC0;\nD_800A76D0 = 0x800A76D0;\nD_800BD02C = 0x800BD02C;\nEnterCriticalSection = 0x80072714;\nExitCriticalSection = 0x80072724;\nSysDeqIntRP = 0x8007E1F4;\nSysEnqIntRP = 0x8007E1E4;\nchkRC2wait = 0x80084FE4;\ng_MemCardCallbackPending = 0x8009B78C;\ng_MemCardObjArray = 0x8009B758;\ng_MemCardObjResetFn = 0x8009B728;\ng_MemCardPort1Present = 0x8009B774;\ng_MemCardPort2Present = 0x8009B778;\ng_MemCardServiceReady = 0x8009B75C;\ng_MemCardSioRegs = 0x8009B788;\ng_MemCardState = 0x8009B784;\nsetRC2wait = 0x80084FC4;\nSECTIONS { .text 0x800829c4 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) *(.mdebug) } }')]


class PsyqPadDsNextFourTests(unittest.TestCase):
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

                if name == "libds/dssys_1_6":
                    subprocess.run(["mipsel-none-elf-objcopy", "-O", "binary",
                                    "--only-section=.rodata", str(elf), str(data)],
                                   check=True)
                    table = data.read_bytes()
                    self.assertEqual(len(table), 104)
                    self.assertEqual(hashlib.sha256(table).hexdigest(),
                                     "236c0e52fbe57b0c488c89ed3b79342be22b3d94f3bd2b3c9f8a08a938425317")
