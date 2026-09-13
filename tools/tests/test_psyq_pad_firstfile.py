"""Full linked-byte regression for four Psy-Q functions in three C units."""
import hashlib
import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]

# Entire linked text ranges at their retail addresses.
CASES = [('libpad/PadStartCom',
  204,
  '076b8ee57e1d1411d223e73ccc826bf4bd9094e504ed1881540730f58d14f197',
  'D_8009B75C = 0x8009B75C;\n'
  'EnterCriticalSection = 0x80072714;\n'
  'D_800A5AB0 = 0x800A5AB0;\n'
  'SysDeqIntRP = 0x8007E1F4;\n'
  'SysEnqIntRP = 0x8007E1E4;\n'
  'D_8009B784 = 0x8009B784;\n'
  'ChangeClearRCnt = 0x80073C84;\n'
  'ExitCriticalSection = 0x80072724;\n'
  'D_8009B758 = 0x8009B758;\n'
  'D_8009B728 = 0x8009B728;\n'
  'D_800A5AC0 = 0x800A5AC0;\n'
  'SECTIONS { .text 0x80082cf0 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }'),
 ('libpad/padmain_sio',
  672,
  '7f8dcf592cc711bce3302338a4485ae1c66ea0d7a67818fd2263cc2aee1daecf',
  'D_8009B768 = 0x8009B768;\n'
  'D_8009B7A8 = 0x8009B7A8;\n'
  'Timer_StartTimeout = 0x80084FC4;\n'
  'MemCard_WaitReadyForTransfer = 0x800834E8;\n'
  'D_8009B724 = 0x8009B724;\n'
  'D_8009B788 = 0x8009B788;\n'
  'Spu_CheckTimerElapsed = 0x80084FE4;\n'
  'D_800BD02C = 0x800BD02C;\n'
  'D_800A76D0 = 0x800A76D0;\n'
  'D_8009B784 = 0x8009B784;\n'
  'SECTIONS { .text 0x80083014 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }'),
 ('libapi/FirstFileHook',
  412,
  'fb4487ddf042d75ff65995232a434817de7de7f5b772bee5593ad0e2e850e24a',
  'D_800A32D8 = 0x800A32D8;\n'
  'strcmp = 0x80072A54;\n'
  'D_800A32D0 = 0x800A32D0;\n'
  'Sys_FirstFileHookCallback = 0x80072950;\n'
  'firstfile = 0x80072A64;\n'
  'SECTIONS { .text 0x800727b4 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }')]


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

                if name == "libpad/padmain_sio":
                    self.assertEqual(hashlib.sha256(code[:200]).hexdigest(),
                                     "c3ba3f12547108abdc045aa509fd58c8873482a66bffc381af9b3dc6c01861dc")
                    self.assertEqual(hashlib.sha256(code[200:]).hexdigest(),
                                     "481ec7da58da60808de11d9bf5c47c8eb3781eb80d6f31459513131603634a03")
