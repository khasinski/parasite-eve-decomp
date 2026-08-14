import unittest

from tools.scripts import collapse_zero_data


class CollapseTests(unittest.TestCase):
    def test_zero_runs_become_one_space_directive(self):
        text = "dlabel D_1\n    .word 0x0\n    .word 0x0\n    .short 0x0\nenddlabel D_1\n"

        out = collapse_zero_data.collapse(text)

        self.assertIn(".space 0xA", out)
        self.assertNotIn(".word 0x0", out)

    def test_nonzero_data_and_labels_break_the_run(self):
        text = "    .word 0x0\ndlabel D_2\n    .word 0x1\n"

        out = collapse_zero_data.collapse(text)

        self.assertIn(".space 0x4\ndlabel D_2", out)
        self.assertIn(".word 0x1", out)

    def test_differ_aliases_are_dropped_from_data(self):
        # asm-differ only diffs functions, and objdiff declines to score a
        # section whose symbols come in address-sharing pairs.
        text = "nonmatching D_800101AC\n\ndlabel D_800101AC\n    .word 0x1\n"

        out = collapse_zero_data.DIFFER_ALIAS.sub("", collapse_zero_data.collapse(text))

        self.assertNotIn("nonmatching", out)
        self.assertIn("dlabel D_800101AC", out)


if __name__ == "__main__":
    unittest.main()
