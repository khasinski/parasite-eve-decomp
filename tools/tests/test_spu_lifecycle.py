"""Retail-free exact-byte checks for SPU startup and shutdown, including relocations."""
import hashlib
import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]

# Full S_INI (_SpuInit + SpuStart) at 0x8007D074, SpuQuit at 0x80085984.
CASES = [('s_ini',
  344,
  '97918bec7c23678f83611325741017d2af8343a3064ef950570f3bf61acedf86',
  'ResetCallback = 0x80073C94;\n'
  '_spu_init = 0x8007D1D4;\n'
  'D_8009B3B8 = 0x8009B3B8;\n'
  'D_8009B3A0 = 0x8009B3A0;\n'
  'D_8009B46C = 0x8009B46C;\n'
  '_spu_rev_flag = 0x8009B390;\n'
  '_spu_rev_reserve_wa = 0x8009B394;\n'
  '_spu_rev_offsetaddr = 0x8009B398;\n'
  '_spu_FsetRXX = 0x8007DAE0;\n'
  'D_8009B45C = 0x8009B45C;\n'
  'D_8009B460 = 0x8009B460;\n'
  'D_8009B464 = 0x8009B464;\n'
  'D_8009B38C = 0x8009B38C;\n'
  'D_8009B418 = 0x8009B418;\n'
  'D_8009B388 = 0x8009B388;\n'
  'D_8009B3B4 = 0x8009B3B4;\n'
  'D_8009B3B0 = 0x8009B3B0;\n'
  'D_8009B3E8 = 0x8009B3E8;\n'
  'D_8009B3EC = 0x8009B3EC;\n'
  'EnterCriticalSection = 0x80072714;\n'
  '_spu_FiDMA = 0x8007D614;\n'
  '_SpuDataCallback = 0x8007DD14;\n'
  'OpenEvent = 0x800726E4;\n'
  '_spu_EVdma = 0x8009B384;\n'
  'EnableEvent = 0x80072704;\n'
  'ExitCriticalSection = 0x80072724;\n'
  'SECTIONS { .text 0x8007d074 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }'),
 ('s_q',
  116,
  '2a097f77cd77ee87e6df9187c0ec4a3524d90ecc3d95fab8c6b876438fcfcb4a',
  'D_8009B3EC = 0x8009B3EC;\n'
  'EnterCriticalSection = 0x80072714;\n'
  '_spu_transferCallback = 0x8009B434;\n'
  '_spu_IRQCallback = 0x8009B438;\n'
  '_SpuDataCallback = 0x8007DD14;\n'
  '_spu_EVdma = 0x8009B384;\n'
  'CloseEvent = 0x800857E4;\n'
  'DisableEvent = 0x800857F4;\n'
  'ExitCriticalSection = 0x80072724;\n'
  'SECTIONS { .text 0x80085984 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }')]


class SpuLifecycleTests(unittest.TestCase):
    @unittest.skipUnless(shutil.which("mipsel-none-elf-ld"), "MIPS binutils unavailable")
    def test_all_linked_bytes_match_retail(self):
        for name, size, digest, script in CASES:
            with self.subTest(function=name), tempfile.TemporaryDirectory() as directory:
                work = pathlib.Path(directory)
                obj, elf, data = work / "code.o", work / "code.elf", work / "code.bin"
                subprocess.run(["tools/scripts/cc.sh", f"src/main/psyq/libspu/{name}.c", str(obj)],
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
