"""Regression gates for texture-window command register cleanup."""
from pathlib import Path
import re
import unittest

ROOT = Path(__file__).resolve().parents[2]


class GpuTexWindowTests(unittest.TestCase):
    def test_no_local_register_pins(self):
        source = (ROOT/'src/main/gpu/gpu_build.c').read_text()
        source = re.sub(r'/\*.*?\*/|//[^\n]*', '', source, flags=re.S)
        function = source.split('u32 Gpu_BuildTexWindowCmd(', 1)[1]
        self.assertNotRegex(function, r'\b(?:asm|__asm__)\b')
        # The translation unit still has its unresolved global stack pin.
        self.assertLessEqual(len(re.findall(r'\b(?:asm|__asm__)\b', source)), 1)

    @unittest.skipUnless((ROOT/'assets/USA/main.exe').is_file() and
                         (ROOT/'build/USA/main.exe').is_file(), 'retail/build unavailable')
    def test_entire_translation_unit_matches_retail(self):
        offset, size = 0x800762A0 - 0x8000F800, 0x9C
        retail = (ROOT/'assets/USA/main.exe').read_bytes()[offset:offset+size]
        built = (ROOT/'build/USA/main.exe').read_bytes()[offset:offset+size]
        self.assertEqual(len(retail), size)
        self.assertEqual(built, retail)


if __name__ == '__main__':
    unittest.main()
