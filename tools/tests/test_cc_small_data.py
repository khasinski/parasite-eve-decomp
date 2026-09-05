import os
import pathlib
import shutil
import subprocess
import tempfile
import unittest

from elftools.elf.elffile import ELFFile


ROOT = pathlib.Path(__file__).resolve().parents[2]


class SmallDataCompileTests(unittest.TestCase):
    def test_small_data_threshold_relocations(self):
        required = ("tools/old-gcc/cc1", "tools/psyq-gcc-2.7.2/cpp",
                    "tools/maspsx/maspsx.py", ".venv/bin/python")
        if (not all((ROOT / path).exists() for path in required)
                or shutil.which("mipsel-none-elf-as") is None):
            self.skipTest("native matching toolchain is not installed")

        env = {key: value for key, value in os.environ.items()
               if not key.startswith("PE_")}
        with tempfile.TemporaryDirectory() as directory:
            source = pathlib.Path(directory) / "threshold.c"
            obj = pathlib.Path(directory) / "threshold.o"
            for threshold in (0, 1, 2, 4, 8):
                with self.subTest(threshold=threshold):
                    source.write_text(
                        "/* CC1_FLAGS: -G%d */\n/* MASPSX_FLAGS: -G%d */\n"
                        "extern char small_byte;\nextern short small_half;\n"
                        "extern int small_word;\n"
                        "void store(void) { small_byte = 0; small_half = 0;"
                        " small_word = 0; }\n" % (threshold, threshold)
                    )
                    subprocess.run(
                        [str(ROOT / "tools/scripts/cc.sh"), str(source), str(obj)],
                        cwd=ROOT, env=env, check=True, capture_output=True, text=True,
                    )
                    with obj.open("rb") as handle:
                        elf = ELFFile(handle)
                        relocations = elf.get_section_by_name(".rel.text")
                        self.assertIsNotNone(relocations)
                        symbols = elf.get_section(relocations["sh_link"])
                        actual = {}
                        for relocation in relocations.iter_relocations():
                            name = symbols.get_symbol(relocation["r_info_sym"]).name
                            actual.setdefault(name, set()).add(relocation["r_info_type"])
                    for name, size in (("small_byte", 1), ("small_half", 2),
                                       ("small_word", 4)):
                        # R_MIPS_GPREL16 versus R_MIPS_HI16/R_MIPS_LO16.
                        self.assertEqual(actual[name], {7} if size <= threshold else {5, 6})


if __name__ == "__main__":
    unittest.main()
