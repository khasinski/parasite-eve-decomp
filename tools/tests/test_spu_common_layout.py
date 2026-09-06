import pathlib
import shutil
import subprocess
import unittest


ROOT = pathlib.Path(__file__).resolve().parents[2]


class SpuCommonLayoutTests(unittest.TestCase):
    @unittest.skipUnless(shutil.which("cc"), "host C compiler unavailable")
    def test_retail_field_offsets(self):
        offsets = {
            "mask": 0x00, "left": 0x04, "right": 0x06,
            "leftMode": 0x08, "rightMode": 0x0A,
            "currentLeft": 0x0C, "currentRight": 0x0E,
            "cdLeft": 0x10, "cdRight": 0x12,
            "cdReverb": 0x14, "cdMix": 0x18,
            "externalLeft": 0x1C, "externalRight": 0x1E,
            "externalReverb": 0x20, "externalMix": 0x24,
        }
        source = '#include <stddef.h>\n#include "pe1/akao/spu_common.h"\n'
        source += '_Static_assert(sizeof(SpuCommonSettings) == 0x28, "size");\n'
        source += (
            '_Static_assert(sizeof(SpuReverbRegisterAttrs) == 0x44, "reverb size");\n'
            '_Static_assert(offsetof(SpuReverbRegisterAttrs, mask) == 0, "mask");\n'
            '_Static_assert(offsetof(SpuReverbRegisterAttrs, regs) == 4, "regs");\n'
            '_Static_assert(offsetof(SpuReverbRegisterAttrs, regs[31]) == 0x42, '
            '"last reverb register");\n'
        )
        for field, offset in offsets.items():
            source += (
                f'_Static_assert(offsetof(SpuCommonSettings, {field}) == '
                f'{offset}, "{field}");\n'
            )
        result = subprocess.run(
            ["cc", "-std=c11", "-Werror", "-fsyntax-only", "-x", "c",
             "-I", str(ROOT / "include"), "-"],
            input=source, text=True, capture_output=True,
        )
        self.assertEqual(result.returncode, 0, result.stderr)


if __name__ == "__main__":
    unittest.main()
