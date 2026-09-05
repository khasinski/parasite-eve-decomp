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

    def test_all_template_literals_are_checked(self):
        for template in ('"" "nop"', '"" /* comment */ "nop"',
                         '"" \\\n "nop"', '"label: nop"', '"sync"'):
            with self.subTest(template=template):
                self.assertEqual(self.classify('void f(void) { asm(%s); }' % template),
                                 "asm_constrained")

    def test_comment_markers_in_strings_do_not_hide_instructions(self):
        text = 'const char *url = "https://example.invalid"; void f(void) { asm("nop"); }'
        self.assertEqual(self.classify(text), "asm_constrained")

    def test_gte_mnemonic_in_operand_does_not_exempt_template(self):
        text = 'void f(int mtc2) { asm("nop" : : "r"(mtc2)); }'
        self.assertEqual(self.classify(text), "asm_constrained")

    def test_gte_template_concatenation_remains_supported(self):
        text = r'void f(void *p) { asm("addiu $2,%0,32\n\t" "lwc2 $0,0($2)" : : "r"(p)); }'
        self.assertEqual(self.classify(text), "semantic_c")

    def test_extern_declaration_does_not_hide_file_scope_asm(self):
        text = 'extern int data __attribute__((unused)); asm(".word 0");'
        self.assertEqual(self.classify(text), "asm_constrained")

    def test_alternate_asm_keyword(self):
        self.assertEqual(self.classify('void f(void) { __asm("nop"); }'),
                         "asm_constrained")

    def test_anonymous_struct_and_function_aliases(self):
        text = '''
        extern struct { char padding[16]; } alias asm("real_symbol");
        int function(int x) asm("real_function");
        unsigned char array[3] asm("real_array");
        extern int scratch asm("0x1F800008");
        '''
        self.assertEqual(self.classify(text), "semantic_c")

    def test_macro_argument_does_not_hide_following_gte_literal(self):
        text = r'void f(void) { asm("addiu $2,$4," STR(OFFSET) "\n\tlwc2 $0,0($2)"); }'
        self.assertEqual(self.classify(text), "semantic_c")

    def test_opaque_template_is_not_assumed_empty(self):
        self.assertEqual(self.classify('void f(void) { asm(IMPLEMENTATION); }'),
                         "asm_constrained")

    def test_extern_before_macro_does_not_hide_instructions(self):
        text = r'''
        extern char *state;
        #define LOAD(dst) \
            asm volatile("lui\t%0, %%hi(state)\n" \
                         "lw\t%0, %%lo(state)(%0)" : "=r"(dst))
        int f(void) { char *p; LOAD(p); return p != 0; }
        '''
        self.assertEqual(self.classify(text), "asm_constrained")

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
