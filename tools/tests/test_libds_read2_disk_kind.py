"""Retail-free exact-byte checks for LIBDS read setup and disk identification, including relocations."""
import hashlib
import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]

# Retail at 0x80081314 and 0x80082400. The latter TU includes the unchanged
# 68-byte GD_cbsync followed by the newly reconstructed 100-byte GD_cbready.
CASES = [('dsread2',
  244,
  '023e6db0a0d5e4be4ed2b3797b3ff9ea2952fcf85a07c93d035fd771b19b0346',
  'DsDataCallback = 0x800824F0;\nDsPacket = 0x8007F0C8;\nDsReadyCallback = 0x800824C8;\nStCdInterrupt = 0x8007C564;\ndata_ready_callback = 0x8007C214;\ng_DsStreamNoLocFlag = 0x800A8020;\nSECTIONS { .text 0x80081314 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) *(.mdebug) } }'),
 ('dstype',
  168,
  '3afbfb2c6c3f80f7089a34d24020ab2509d7c1a66b92d76e5e353fa067dd4ce0',
  'DsStartReadySystem = 0x80081D74;\ng_DsDiskType = 0x800B28F8;\nDsGetSector = 0x80080AE4;\nD_8001205C = 0x8001205C;\nstrncmp = 0x80071A04;\nDsEndReadySystem = 0x80081DF8;\nSECTIONS { .text 0x80082400 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) *(.mdebug) } }')]


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
