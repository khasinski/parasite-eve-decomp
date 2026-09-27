"""Regression gates for partial envelope register-constraint cleanup."""
from pathlib import Path
import re
import unittest

ROOT = Path(__file__).resolve().parents[2]


class SpuVoiceEnvelopeConstraintTests(unittest.TestCase):
    def test_redundant_pins_are_absent(self):
        source = (ROOT/'src/main/akao/Spu_TickVoiceEnvelopes.c').read_text()
        source = re.sub(r'/\*.*?\*/|//[^\n]*', '', source, flags=re.S)
        for name in ('delta','pitch_depth','volume_stage','wave_sample','wave','pitch_weight'):
            self.assertNotRegex(source,r'\b'+name+r'\s+asm\s*\(')
        self.assertLessEqual(len(re.findall(r'asm\("\$\d+"\)',source)),5)

    @unittest.skipUnless((ROOT/'assets/USA/main.exe').is_file() and
                         (ROOT/'build/USA/main.exe').is_file(), 'retail/build unavailable')
    def test_entire_function_matches_retail(self):
        offset,size = 0x80087FA0-0x8000F800,0x3A4
        retail = (ROOT/'assets/USA/main.exe').read_bytes()[offset:offset+size]
        built = (ROOT/'build/USA/main.exe').read_bytes()[offset:offset+size]
        self.assertEqual(len(retail),size)
        self.assertEqual(built,retail)


if __name__ == '__main__':
    unittest.main()
