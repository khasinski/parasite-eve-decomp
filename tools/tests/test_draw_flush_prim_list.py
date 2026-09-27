"""Plain-C and retail-match regression gates for text centering and rendering."""
from pathlib import Path
import re
import unittest

ROOT = Path(__file__).resolve().parents[2]


class DrawFlushPrimListTests(unittest.TestCase):
    def test_translation_unit_has_no_assembly(self):
        source = (ROOT/'src/main/gpu/Draw_FlushPrimList.c').read_text()
        prefix = source.split('    if (cursor != 0) {', 1)[0]
        self.assertIn('g_TextCursorStackPtr = cursor + 2;', prefix)
        self.assertIn('g_TextCursorX = x + center;', prefix)
        source = re.sub(r'/\*.*?\*/|//[^\n]*', '', source, flags=re.S)
        self.assertNotRegex(source, r'\b(?:asm|__asm__|INCLUDE_ASM|CC_POSTPASS)\b')

    @unittest.skipUnless((ROOT/'assets/USA/main.exe').is_file() and
                         (ROOT/'build/USA/main.exe').is_file(), 'retail/build unavailable')
    def test_entire_built_function_matches_retail(self):
        offset, size = 0x80062A7C - 0x8000F800, 0x23C
        retail = (ROOT/'assets/USA/main.exe').read_bytes()[offset:offset+size]
        built = (ROOT/'build/USA/main.exe').read_bytes()[offset:offset+size]
        self.assertEqual(len(retail), size)
        self.assertEqual(built, retail)


if __name__ == '__main__':
    unittest.main()
