"""Regression gates for partial key-on/off constraint removal, not clean matches."""
from pathlib import Path
import re
import unittest

ROOT = Path(__file__).resolve().parents[2]


class AkaoVoiceKeyConstraintTests(unittest.TestCase):
    def test_redundant_constraints_are_absent(self):
        for name,limit,unpinned in (
            ('Akao_SetVoiceKeyOn',9,('flags','base_volume','value','next')),
            ('Akao_SetVoiceKeyOff',19,('voice','one','raw_pan','bias','pitch')),
        ):
            with self.subTest(function=name):
                source = (ROOT/f'src/main/akao/{name}.c').read_text()
                source = re.sub(r'/\*.*?\*/|//[^\n]*','',source,flags=re.S)
                self.assertNotRegex(source,r'asm\s+volatile')
                self.assertLessEqual(len(re.findall(r'asm\("\$\d+"\)',source)),limit)
                for variable in unpinned:
                    self.assertNotRegex(source,r'\b'+variable+r'\s+asm\s*\(')

    @unittest.skipUnless((ROOT/'assets/USA/main.exe').is_file() and
                         (ROOT/'build/USA/main.exe').is_file(), 'retail/build unavailable')
    def test_entire_functions_match_retail(self):
        # Full functions, including macro-expanded pan and LFO paths. These
        # byte gates are not an independent audio/hardware behavior model.
        retail = (ROOT/'assets/USA/main.exe').read_bytes()
        built = (ROOT/'build/USA/main.exe').read_bytes()
        for entry,size in ((0x80088344,0x63C),(0x80088980,0x4E4)):
            with self.subTest(entry=hex(entry)):
                offset = entry-0x8000F800
                self.assertEqual(len(retail[offset:offset+size]),size)
                self.assertEqual(built[offset:offset+size],retail[offset:offset+size])


if __name__ == '__main__': unittest.main()
