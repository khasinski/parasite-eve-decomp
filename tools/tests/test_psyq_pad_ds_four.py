"""Full linked-byte regressions for Psy-Q LIBPAD/LIBDS functions."""
import hashlib
import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]

# Entire linked text ranges at their retail addresses.
CASES = [('libpad/padseqd_2',
  1088,
  'b01296f0208d34d3dfd34331b3392e03e2303806107ae873853c5653b6d62c62',
  '_padCmdParaMode = 0x80083E50;\n'
  '_padRecvAtLoadInfo = 0x80083644;\n'
  '_padSendAtLoadInfo = 0x800835C0;\n'
  'D_8009B728 = 0x8009B728;\n'
  'SECTIONS { .text 0x80084b78 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }'),
 ('libpad/padcmd_3',
  796,
  'c58945e1ab65142acb12ac4c9699629869cc8c4c731ab2947a19bab8394ab2f0',
  'CardObj_EmitCommand46 = 0x80083EA4;\n'
  'CardObj_EmitCommand47 = 0x80083EC4;\n'
  'CardObj_EmitCommand4B = 0x80083EE4;\n'
  'CardObj_EmitCommand4C = 0x80083E84;\n'
  'D_800A5AD0 = 0x800A5AD0;\n'
  'SECTIONS { .text 0x8008389c : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }'),
 ('libds/dssys2_flush',
  496,
  'cc34f5b089194c02b861b44bdc80af68287db290a4f3fd5cf00e639d62e56e7e',
  'D_800A3600 = 0x800A3600;\n'
  'D_800A3540 = 0x800A3540;\n'
  'CQ_add_result = 0x8007EB88;\n'
  'D_800A3608 = 0x800A3608;\n'
  'D_800A3604 = 0x800A3604;\n'
  'SECTIONS { .text 0x8007e704 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }'),
]


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
