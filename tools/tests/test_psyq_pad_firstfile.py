"""Full linked-byte regression for four Psy-Q functions in three C units."""
import hashlib
import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]

# Entire linked text ranges at their retail addresses.
CASES = [('libpad/padmain',
  3036,
  '2eae8f307282ab6fe44724b5d8c17ad6ff39316533ed88103ef9de565b4799a0',
  'ChangeClearRCnt = 0x80073C84;\nD_8009B724 = 0x8009B724;\nD_8009B728 = 0x8009B728;\nD_8009B744 = 0x8009B744;\nD_8009B748 = 0x8009B748;\nD_8009B74C = 0x8009B74C;\nD_8009B758 = 0x8009B758;\nD_8009B75C = 0x8009B75C;\nD_8009B764 = 0x8009B764;\nD_8009B768 = 0x8009B768;\nD_8009B76C = 0x8009B76C;\nD_8009B774 = 0x8009B774;\nD_8009B778 = 0x8009B778;\nD_8009B77C = 0x8009B77C;\nD_8009B784 = 0x8009B784;\nD_8009B788 = 0x8009B788;\nD_8009B7A8 = 0x8009B7A8;\nD_800A5AB0 = 0x800A5AB0;\nD_800A5AB4 = 0x800A5AB4;\nD_800A5AC0 = 0x800A5AC0;\nD_800A76D0 = 0x800A76D0;\nD_800BD02C = 0x800BD02C;\nEnterCriticalSection = 0x80072714;\nExitCriticalSection = 0x80072724;\nSysDeqIntRP = 0x8007E1F4;\nSysEnqIntRP = 0x8007E1E4;\nchkRC2wait = 0x80084FE4;\ng_MemCardCallbackPending = 0x8009B78C;\ng_MemCardObjArray = 0x8009B758;\ng_MemCardObjResetFn = 0x8009B728;\ng_MemCardPort1Present = 0x8009B774;\ng_MemCardPort2Present = 0x8009B778;\ng_MemCardServiceReady = 0x8009B75C;\ng_MemCardSioRegs = 0x8009B788;\ng_MemCardState = 0x8009B784;\nsetRC2wait = 0x80084FC4;\nSECTIONS { .text 0x800829c4 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) *(.mdebug) } }'),
 ('libapi/first',
  668,
  '54cf62beee29111f4732b454b4ccce07e6d3a611b4d0ab06412c87c69fe2cdba',
  'D_800A32D0 = 0x800A32D0;\nD_800A32D8 = 0x800A32D8;\nfirstfile2 = 0x80072A64;\nstrcmp = 0x80072A54;\nSECTIONS { .text 0x800727b4 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) *(.mdebug) } }')]


class PsyqPadFirstfileTests(unittest.TestCase):
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

                if name == "libpad/padmain_7":
                    self.assertEqual(hashlib.sha256(code[:200]).hexdigest(),
                                     "c3ba3f12547108abdc045aa509fd58c8873482a66bffc381af9b3dc6c01861dc")
                    self.assertEqual(hashlib.sha256(code[200:]).hexdigest(),
                                     "481ec7da58da60808de11d9bf5c47c8eb3781eb80d6f31459513131603634a03")
