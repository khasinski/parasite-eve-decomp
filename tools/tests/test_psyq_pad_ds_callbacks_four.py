"""Full linked-byte regression for four Psy-Q LIBPAD/LIBDS functions."""
import hashlib
import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]

# Entire linked text ranges at their retail addresses.
CASES = [('libpad/padportd_outputs',
  636,
  'cdd89c8623bb223d4d99006bab00bf286d6194f37e2685f4fa855a0d88db5b28',
  'bzero = 0x80071A24;\n'
  'D_8009B76C = 0x8009B76C;\n'
  'SECTIONS { .text 0x8008486c : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }'),
 ('libds/dssys1_vsync',
  720,
  '7df21a00b56e81e4d4a811bd38215db6ff65f4246173802e2e0beb262734fabd',
  'D_8009B598 = 0x8009B598;\n'
  'CdRom_RetryCmd = 0x800800F4;\n'
  'D_8009B594 = 0x8009B594;\n'
  'D_8009B6A4 = 0x8009B6A4;\n'
  'CdRom_SendCmd = 0x8007FCFC;\n'
  'D_800A36A0 = 0x800A36A0;\n'
  'g_DsReadSysEnabled = 0x8009B554;\n'
  'D_8009B574 = 0x8009B574;\n'
  'D_8009B570 = 0x8009B570;\n'
  'SECTIONS { .text 0x8007fe24 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }'),
 ('libds/dsready_callback',
  732,
  'dd749c65e05714e3bd6130961ed0cc79bdbbef7acfedd2fd2622c3e6f21af5a7',
  'D_8009B6EC = 0x8009B6EC;\n'
  'CdRom_GetCurrentPosPtr = 0x8007FC28;\n'
  'CdPosToInt = 0x80080C48;\n'
  'CdRom_GetCmdMode = 0x8007FC18;\n'
  'DsDataCallback = 0x800824F0;\n'
  'CdRom_IsBusy = 0x80080AE4;\n'
  'DsSyncCallback = 0x800824C8;\n'
  'DsReadyCallback = 0x800824DC;\n'
  'CdRom_GetPendingReadCount = 0x8007F778;\n'
  'D_8009B6F8 = 0x8009B6F8;\n'
  'CdRom_RestartSeek = 0x80082204;\n'
  'D_8009B70C = 0x8009B70C;\n'
  'D_8009B700 = 0x8009B700;\n'
  'D_8009B704 = 0x8009B704;\n'
  'Render_AllocParticleNode = 0x8007EE84;\n'
  'SECTIONS { .text 0x80081e70 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }'),
 ('libpad/padif_transfer',
  892,
  'a435b3916bf94b1ffe701eb59664b0f9a9b3b57710343a2f3e1a7f39070089c3',
  'D_8009B730 = 0x8009B730;\n'
  'D_8009B770 = 0x8009B770;\n'
  'D_8009B79C = 0x8009B79C;\n'
  'D_8009B72C = 0x8009B72C;\n'
  'MemCard_WriteByte = 0x800832B4;\n'
  'Timer_StartTimeout = 0x80084FC4;\n'
  'MemCard_WaitReadyForTransfer = 0x800834E8;\n'
  'D_8009B764 = 0x8009B764;\n'
  'D_8009B77C = 0x8009B77C;\n'
  'D_8009B758 = 0x8009B758;\n'
  'D_8009B744 = 0x8009B744;\n'
  'D_8009B748 = 0x8009B748;\n'
  'MemCard_ReadByte = 0x800830DC;\n'
  'MemCard_WaitStatusBit2 = 0x80083578;\n'
  'D_8009B7A0 = 0x8009B7A0;\n'
  'D_8009B724 = 0x8009B724;\n'
  'SECTIONS { .text 0x80084168 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }')]


class PsyqPadDsCallbacksFourTests(unittest.TestCase):
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
