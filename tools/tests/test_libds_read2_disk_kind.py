"""Retail-free exact-byte checks for LIBDS read setup and disk identification, including relocations."""
import hashlib
import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]

# Retail at 0x80081314 and 0x80082400. The latter TU includes the unchanged
# 68-byte GD_cbsync followed by the newly reconstructed 100-byte GD_disk_kind.
CASES = [('DsRead2',
  212,
  'fe43d63bd26dd2279dbdc1c8858998cace3f0da9d7cd16172aa61b8d32a6ce42',
  'g_DsStreamNoLocFlag = 0x800A8020;\n'
  'data_ready_callback = 0x8007C214;\n'
  'DsDataCallback = 0x800824F0;\n'
  'CdRom_BreakSyncCallback = 0x800813E8;\n'
  'DsSyncCallback = 0x800824C8;\n'
  'Render_BuildParticleFrame = 0x8007F0C8;\n'
  'SECTIONS { .text 0x80081314 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }'),
 ('dstype',
  168,
  '3afbfb2c6c3f80f7089a34d24020ab2509d7c1a66b92d76e5e353fa067dd4ce0',
  'CdRom_InitAsyncRead = 0x80081D74;\n'
  'g_DsDiskType = 0x800B28F8;\n'
  'CdRom_IsBusy = 0x80080AE4;\n'
  'D_8001205C = 0x8001205C;\n'
  'strncmp = 0x80071A04;\n'
  'DsReadBreak = 0x80081DF8;\n'
  'SECTIONS { .text 0x80082400 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }')]


class LibdsRead2DiskKindTests(unittest.TestCase):
    @unittest.skipUnless(shutil.which("mipsel-none-elf-ld"), "MIPS binutils unavailable")
    def test_all_linked_bytes_match_retail(self):
        for name, size, digest, script in CASES:
            with self.subTest(function=name), tempfile.TemporaryDirectory() as directory:
                work = pathlib.Path(directory)
                obj, elf, data = work / "code.o", work / "code.elf", work / "code.bin"
                subprocess.run(["tools/scripts/cc.sh", f"src/main/psyq/libds/{name}.c", str(obj)],
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
