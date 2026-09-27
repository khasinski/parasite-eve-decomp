"""Regression gates for partial sample-upload register-constraint cleanup."""
from pathlib import Path
import re
import unittest

ROOT = Path(__file__).resolve().parents[2]


class AkaoSampleUploadConstraintTests(unittest.TestCase):
    def test_redundant_pins_are_absent(self):
        source = (ROOT/'src/main/akao/Akao_StepNoteSequencer.c').read_text()
        source = re.sub(r'/\*.*?\*/|//[^\n]*', '', source, flags=re.S)
        for name in ('remaining_payload','dest_base'):
            self.assertNotRegex(source, name+r'\s+asm\s*\(')
        # This is partial cleanup, not a plain-C status assertion.
        self.assertLessEqual(len(re.findall(r'asm\("\$\d+"\)',source)),8)

    @unittest.skipUnless((ROOT/'assets/USA/main.exe').is_file() and
                         (ROOT/'build/USA/main.exe').is_file(), 'retail/build unavailable')
    def test_entire_function_matches_retail(self):
        offset,size = 0x800871AC-0x8000F800,0x268
        retail = (ROOT/'assets/USA/main.exe').read_bytes()[offset:offset+size]
        built = (ROOT/'build/USA/main.exe').read_bytes()[offset:offset+size]
        self.assertEqual(len(retail),size)
        self.assertEqual(built,retail)


if __name__ == '__main__':
    unittest.main()
