"""Retail-free exact-byte checks for stream completion and VSync waiting, including relocations."""
import hashlib
import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]

# Complete C_004 (data_ready_callback + StGetBackloc) at 0x8007C214 and v_wait at 0x80073BBC.
CASES = [('libcd/c_004',
  228,
  'ce310e8f04723acbf9226a2db3c5ddb151b86678f24e94aeb51b6648e25df94c',
  'g_CdStreamRingReadSlot = 0x800BE9E4;\ng_CdRingBufPtr = 0x800C0DC8;\nD_800A3490 = 0x800A3490;\nD_800BE998 = 0x800BE998;\ng_StrDataReadyCallback = 0x800B0CC8;\nD_800A3494 = 0x800A3494;\ng_CdStreamDataReadyFlag = 0x800B89F4;\nD_800A8020 = 0x800A8020;\nCdPosToInt = 0x8007AA34;\nCdIntToPos = 0x8007A930;\nSECTIONS { .text 0x8007c214 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) *(.mdebug) } }'),
 ('libetc/vsync',
  528,
  'e356692b0a159f0f9e07da321a2ea515c4789379094ff39d6d77f42858169af4',
  'ChangeClearPAD = 0x80073C74;\nChangeClearRCnt = 0x80073C84;\nD_800116FC = 0x800116FC;\nD_80094574 = 0x80094574;\nD_80094578 = 0x80094578;\nD_8009457C = 0x8009457C;\nD_80094580 = 0x80094580;\ng_VSyncCount = 0x800956AC;\nputs = 0x80073C5C;\nSECTIONS { .text 0x80073a44 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) *(.mdebug) } }')]


class StreamReadyVwaitTests(unittest.TestCase):
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
