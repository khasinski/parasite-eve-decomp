import pathlib
import tempfile
import unittest

from tools.scripts import source_quality


class SourceQualityTests(unittest.TestCase):
    def classify(self, text):
        with tempfile.TemporaryDirectory() as directory:
            path = pathlib.Path(directory) / "unit.c"
            path.write_text(text)
            return source_quality.classify(path)

    def test_plain_c_is_semantic(self):
        self.assertEqual(self.classify("int f(void) { return 1; }"), "semantic_c")

    def test_pins_aliases_and_empty_barriers_remain_semantic(self):
        text = '''
        extern int value asm("D_80010000");
        int f(void) {
            register int result asm("$v0") = value;
            asm("");
            return result;
        }
        '''
        self.assertEqual(self.classify(text), "semantic_c")

    def test_instruction_asm_is_constrained(self):
        self.assertEqual(self.classify('void f(void) { asm("nop"); }'),
                         "asm_constrained")

    def test_bios_trampoline_is_original_asm(self):
        self.assertEqual(self.classify("PSYQ_BIOS_TRAMPOLINE(open, 0x32, 0);"),
                         "original_asm")

    def test_text_resident_array_is_data(self):
        self.assertEqual(
            self.classify('int words[] __attribute__((section(".text"))) = { 0 };'),
            "text_data",
        )

    def test_function_with_text_resident_data_is_still_semantic_c(self):
        text = '''
        int f(void) { return 1; }
        int words[] __attribute__((section(".text"))) = { 0 };
        '''
        self.assertEqual(self.classify(text), "semantic_c")


if __name__ == "__main__":
    unittest.main()
