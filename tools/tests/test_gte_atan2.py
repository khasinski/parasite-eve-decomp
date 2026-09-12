"""Retail-free regression for the full Gte_Atan2 instruction stream."""
import hashlib
import pathlib
import shutil
import subprocess
import tempfile
import unittest

from tools.scripts import gen_expected, objdiff_config

ROOT = pathlib.Path(__file__).resolve().parents[2]


class GteAtan2Tests(unittest.TestCase):
    def test_native_pipeline_matches_every_linked_byte(self):
        if not shutil.which('mipsel-none-elf-ld'):
            self.skipTest('MIPS binutils unavailable')
        with tempfile.TemporaryDirectory() as directory:
            work = pathlib.Path(directory)
            obj, linked, data = work / 'gte.o', work / 'gte.elf', work / 'gte.bin'
            subprocess.run(['tools/scripts/cc.sh', 'src/main/psyq/libgte/Gte_Atan2.c', str(obj)],
                           cwd=ROOT, check=True, capture_output=True)
            script = work / 'gte.ld'
            script.write_text('D_8009A6EC = 0x8009A6EC;\n'
                              'SECTIONS { .text 0x80079FB4 : SUBALIGN(4) '
                              '{ *(.text .text.*) } /DISCARD/ : { *(.reginfo) *(.mdebug) } }')
            subprocess.run(['mipsel-none-elf-ld', '-T', str(script), str(obj), '-o', str(linked)], check=True)
            subprocess.run(['mipsel-none-elf-objcopy', '-O', 'binary', '--only-section=.text',
                            str(linked), str(data)], check=True)
            code = data.read_bytes()
            self.assertEqual(len(code), 372)
            self.assertEqual(hashlib.sha256(code).hexdigest(),
                             'e5b0edc7308d715c3fd821bd7f75478555d6937ec97c9a8278f6c7746ad0f820')
            self.assertIn(('.text', 'Gte_Atan2', 0, 'STT_FUNC', 372), gen_expected.defined_symbols(obj))
            self.assertEqual(gen_expected.object_section_bytes(obj), 372)

    def test_function_section_is_counted_in_wrapped_link_map(self):
        obj = 'build/USA/src/main/psyq/libgte/Gte_Atan2.c.o'
        text = ' .text.Gte_Atan2\n                0x80079fb4 0x174 ' + obj + '\n'
        self.assertTrue(objdiff_config.linked_objects(text)[obj]['code'])
        self.assertEqual(gen_expected.parse_map_placement(text)[obj]['.text.Gte_Atan2'],
                         (0x80079fb4, 372))
