"""Byte and symbolic-relocation regressions for the SPU callback and transfer-mode setters."""
import hashlib
import pathlib
import shutil
import subprocess
import tempfile
import unittest

from elftools.elf.elffile import ELFFile

ROOT = pathlib.Path(__file__).resolve().parents[2]

# Complete retail code ranges, including both delay-slot stores.
CASES = [('akao/Spu_SetTransferMode',
  60,
  'debb78329dd39e6add2ccd1078286a5de18c83f920d7935ff1c65a77a1b75856',
  '_spu_IRQCallback = 0x8009B438;\n'
  '_SpuCallback = 0x80085DC4;\n'
  'SECTIONS { .text 0x80085d84 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }'),
 ('akao/Spu_SetReverbMode',
  48,
  '134e29bc16aa82935dc5bf1526921eca7242026ae951ebcb936eeb9cd001121e',
  'D_8009B38C = 0x8009B38C;\n'
  'D_8009B418 = 0x8009B418;\n'
  'SECTIONS { .text 0x80085f14 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }')]

# HI16/LO16 must remain relocations rather than hard-coded RAM addresses.
RELOCATIONS = {
    "akao/Spu_SetTransferMode": [
        (8, 5, "_spu_IRQCallback"), (12, 6, "_spu_IRQCallback"),
        (28, 5, "_spu_IRQCallback"), (32, 4, "_SpuCallback"),
        (36, 6, "_spu_IRQCallback"),
    ],
    "akao/Spu_SetReverbMode": [
        (16, 4, ".text"), (28, 5, "D_8009B38C"),
        (32, 6, "D_8009B38C"), (36, 5, "D_8009B418"),
        (44, 6, "D_8009B418"),
    ],
}


class SpuSymbolicSettersTests(unittest.TestCase):
    @unittest.skipUnless(shutil.which("mipsel-none-elf-ld"), "MIPS binutils unavailable")
    def test_all_linked_bytes_match_retail(self):
        for name, size, digest, script in CASES:
            with self.subTest(function=name), tempfile.TemporaryDirectory() as directory:
                work = pathlib.Path(directory)
                obj, elf, data = work / "code.o", work / "code.elf", work / "code.bin"
                subprocess.run(["tools/scripts/cc.sh", f"src/main/{name}.c", str(obj)],
                               cwd=ROOT, check=True, capture_output=True)
                with obj.open("rb") as stream:
                    elf_object = ELFFile(stream)
                    symbols = elf_object.get_section_by_name(".symtab")
                    actual = []
                    for relocation in elf_object.get_section_by_name(".rel.text").iter_relocations():
                        symbol = symbols.get_symbol(relocation["r_info_sym"])
                        target = symbol.name or elf_object.get_section(symbol["st_shndx"]).name
                        actual.append((relocation["r_offset"], relocation["r_info_type"], target))
                    self.assertEqual(sorted(actual), RELOCATIONS[name])
                linker_script = work / "code.ld"
                linker_script.write_text(script)
                subprocess.run(["mipsel-none-elf-ld", "-T", str(linker_script),
                                str(obj), "-o", str(elf)], check=True)
                subprocess.run(["mipsel-none-elf-objcopy", "-O", "binary",
                                "--only-section=.text", str(elf), str(data)], check=True)
                code = data.read_bytes()
                self.assertEqual(len(code), size)
                self.assertEqual(hashlib.sha256(code).hexdigest(), digest)
