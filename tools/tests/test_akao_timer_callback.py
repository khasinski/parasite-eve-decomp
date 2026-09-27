"""Regression gates for timer-history scalar declarations (one pin remains)."""
from pathlib import Path
import re
import unittest

ROOT = Path(__file__).resolve().parents[2]


class AkaoTimerCallbackTests(unittest.TestCase):
    def test_scalar_history_declarations(self):
        source = (ROOT/'src/main/akao/Akao_TimerCallback.c').read_text()
        source = re.sub(r'/\*.*?\*/|//[^\n]*', '', source, flags=re.S)
        self.assertNotIn('__asm__',source)
        self.assertNotIn('char _[16]',source)
        for name in ('D_8009B7EC','g_AkaoTimerDeltaHist2','g_AkaoTimerDeltaHist1','g_AkaoTimerDeltaHist0','D_8009CDE4'):
            self.assertIn('extern s32 '+name+';',source)
        # Partial cleanup, not a claim that the remaining pin is required.
        self.assertLessEqual(len(re.findall(r'\basm\s*\(',source)),1)

    @unittest.skipUnless((ROOT/'assets/USA/main.exe').is_file() and
                         (ROOT/'build/USA/main.exe').is_file(), 'retail/build unavailable')
    def test_retail_text_is_unchanged(self):
        offset,size = 0x8008E23C-0x8000F800,0xA0
        retail = (ROOT/'assets/USA/main.exe').read_bytes()[offset:offset+size]
        built = (ROOT/'build/USA/main.exe').read_bytes()[offset:offset+size]
        self.assertEqual(len(retail),size)
        self.assertEqual(built,retail)


if __name__ == '__main__':
    unittest.main()
