"""Regression gates for glyph packet load-delay cleanup."""
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[2]


class DrawEmitGlyphTests(unittest.TestCase):
    def test_typed_fields_remove_clut_register_pin(self):
        source = (ROOT/'src/main/gpu/Draw_EmitGlyph.c').read_text()
        self.assertIn('RenderTexturedQuad *packet;', source)
        self.assertIn('DrawGlyphDescriptor *glyph;', source)
        self.assertNotIn('M2C_FIELD', source)
        self.assertNotIn('m2c_macros.h', source)
        self.assertLessEqual(source.count('asm("$4")'), 1)

    def test_load_delays_need_no_explicit_nops(self):
        source = (ROOT/'src/main/gpu/Draw_EmitGlyph.c').read_text()
        self.assertNotIn('PE1_NOP', source)
        self.assertNotIn('psyq_nop.h', source)
        self.assertNotIn('asm volatile("nop")', source)

    @unittest.skipUnless((ROOT/'assets/USA/main.exe').is_file() and
                         (ROOT/'build/USA/main.exe').is_file(), 'retail/build unavailable')
    def test_entire_built_function_matches_retail(self):
        offset, size = 0x8005ED18 - 0x8000F800, 0x1B0
        retail = (ROOT/'assets/USA/main.exe').read_bytes()[offset:offset+size]
        built = (ROOT/'build/USA/main.exe').read_bytes()[offset:offset+size]
        self.assertEqual(len(retail), size)
        self.assertEqual(built, retail)


if __name__ == '__main__':
    unittest.main()
