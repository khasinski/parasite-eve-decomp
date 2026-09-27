"""Regression gates for the partial frame-orchestration cleanup."""
from pathlib import Path
import re
import unittest

ROOT = Path(__file__).resolve().parents[2]


class GpuRenderFrameTests(unittest.TestCase):
    def test_address_helpers_need_no_argument_register_pin(self):
        source = (ROOT/'src/main/gpu/Gpu_RenderFrame.c').read_text()
        source = re.sub(r'/\*.*?\*/|//[^\n]*', '', source, flags=re.S)
        self.assertIn('PutDispEnv((DISPENV *)DispAddress(idx));', source)
        self.assertIn('PutDrawEnv(DrawAddress(idx));', source)
        self.assertNotIn('asm("$4")', source)
        # The linked-draw temporary and barrier are still unresolved.
        self.assertLessEqual(source.count('asm('), 1)
        self.assertLessEqual(source.count('asm volatile('), 1)

    @unittest.skipUnless((ROOT/'assets/USA/main.exe').is_file() and
                         (ROOT/'build/USA/main.exe').is_file(), 'retail/build unavailable')
    def test_entire_built_function_matches_retail(self):
        offset, size = 0x80070E54 - 0x8000F800, 0x158
        retail = (ROOT/'assets/USA/main.exe').read_bytes()[offset:offset+size]
        built = (ROOT/'build/USA/main.exe').read_bytes()[offset:offset+size]
        self.assertEqual(len(retail), size)
        self.assertEqual(built, retail)


if __name__ == '__main__':
    unittest.main()
