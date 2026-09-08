import pathlib
import tempfile
import unittest

from tools.scripts import audit_tu_candidates


class TranslationUnitCandidateTests(unittest.TestCase):
    def test_defined_functions_finds_named_definitions_but_not_control_flow(self):
        source = """\\
static void First(void) {
}

s32 Second(s32 value) {
    if (value) {
        return First();
    } else if (!value) {
        return 0;
    }
    return 0;
}
"""
        self.assertEqual(audit_tu_candidates.defined_functions(source),
                         {"First", "Second"})

    def test_entries_preserve_non_c_ranges_between_c_entries(self):
        with tempfile.TemporaryDirectory() as directory:
            config = pathlib.Path(directory) / "main.yaml"
            config.write_text("""\
      - [0x100, c, main/First]
      - [0x120, asm, main/Gap]
      - [0x140, c, main/Second]
""")
            self.assertEqual(audit_tu_candidates.entries(config), [
                (0x100, "c", "main/First"),
                (0x120, "asm", "main/Gap"),
                (0x140, "c", "main/Second"),
            ])


if __name__ == "__main__":
    unittest.main()
