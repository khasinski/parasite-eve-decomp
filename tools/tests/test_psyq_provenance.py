"""SDK attribution must survive decompilation and reject uncovered code."""
import unittest
from tools.scripts.psyq_provenance import identity


def record(start=0x80001000, size=0x40, library='LIBPAD', obj='PADSEQD'):
    return dict(address=hex(start), size=size, library=library, object=obj,
                labels=[dict(offset=0, name='_dirFailAuto')])


class PsyqProvenanceTests(unittest.TestCase):
    def test_original_asm_and_c_have_the_same_sdk_identity(self):
        evidence = [record()]
        before = identity([(0x80001000, 0x40)], 'main/memcard/mem_1000', evidence)
        after = identity([(0x80001000, 0x40)], 'main/psyq/libpad/DirFailAuto', evidence)
        self.assertEqual(before, after)
        self.assertEqual(after, 'main/psyq/libpad/PADSEQD/_dirFailAuto')

    def test_partial_overlap_cannot_relabel_game_code(self):
        for span in [(0x80000FFC, 0x44), (0x80001000, 0x44)]:
            self.assertIsNone(identity([span], 'main/game', [record()]))

    def test_gap_between_sdk_objects_is_not_proof(self):
        evidence = [record(size=0x10), record(start=0x80001020, size=0x20)]
        self.assertIsNone(identity([(0x80001000, 0x40)], 'main/game', evidence))

    def test_all_text_sections_must_be_covered(self):
        self.assertIsNone(identity([(0x80001000, 0x40), (0x80002000, 4)],
                                   'main/mixed', [record()]))

    def test_ambiguous_library_match_does_not_invent_an_object_name(self):
        evidence = [record(), record(library='LIBCD', obj='SYS')]
        self.assertEqual(identity([(0x80001000, 0x40)], 'main/unknown', evidence),
                         'main/psyq/shared/unknown')

    def test_private_label_is_preserved_without_inventing_a_public_name(self):
        e = record()
        e['labels'] = [dict(offset=0x10, name='text_3A0')]
        self.assertEqual(identity([(0x80001010, 0x20)], 'main/Render', [e]),
                         'main/psyq/libpad/PADSEQD/text_3A0')
