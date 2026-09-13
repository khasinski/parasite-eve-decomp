"""Full linked-byte regression for four Psy-Q LIBPAD/LIBDS functions."""
import hashlib
import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]

# Entire linked text ranges at their retail addresses.
CASES = [('libds/dssys1_poll',
  648,
  'c1e84ce0b64fd483bcd85563e24d4cff81d138cfaefafdce980fbf26de32fa9f',
  'D_8009B578 = 0x8009B578;\n'
  'D_800A36AC = 0x800A36AC;\n'
  'D_800A36A8 = 0x800A36A8;\n'
  'D_800A36A4 = 0x800A36A4;\n'
  'g_DsReadSysEnabled = 0x8009B554;\n'
  'SECTIONS { .text 0x80080404 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }'),
 ('libpad/padseqd_response',
  612,
  '47c760223bf315e045fd97140986def718e5d1852e262838a9e3760a18a433b8',
  'CardObj_IsTransferActive = 0x80084F8C;\n'
  'D_8009B728 = 0x8009B728;\n'
  'CardObj_AdvanceReadLayout = 0x80083644;\n'
  'SECTIONS { .text 0x80084c4c : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }'),
 ('libds/dssys2_sync',
  548,
  '70a363e79ed71476b8961d2cb1c2a537c881a4c232bd3902a148f31bef32fc11',
  'D_800A3604 = 0x800A3604;\n'
  'D_800A3510 = 0x800A3510;\n'
  'Util_Copy8 = 0x80080998;\n'
  'CdRom_EnqueueCmd = 0x8007EB88;\n'
  'Spu_DrainQueueEntry = 0x8007E5C4;\n'
  'D_800B8AB0 = 0x800B8AB0;\n'
  'DsSync = 0x8007FBF0;\n'
  'D_800A3608 = 0x800A3608;\n'
  'CdRom_TryIssueCmd = 0x8007FB44;\n'
  'SECTIONS { .text 0x8007e964 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }'),
 ('libpad/PadInitSioMode',
  532,
  '38dae3a9445d323f640baf1405b4c53ff44fc742dc7298f0d2a062266e1be513',
  'D_8009B788 = 0x8009B788;\n'
  'Timer_StartTimeout = 0x80084FC4;\n'
  'D_8009B764 = 0x8009B764;\n'
  'D_8009B77C = 0x8009B77C;\n'
  'D_8009B744 = 0x8009B744;\n'
  'D_8009B748 = 0x8009B748;\n'
  'MemCard_WaitStatusBit2 = 0x80083578;\n'
  'D_8009B784 = 0x8009B784;\n'
  'SECTIONS { .text 0x80082e00 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }')]


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
