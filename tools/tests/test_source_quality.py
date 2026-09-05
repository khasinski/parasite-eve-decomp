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

    def test_audited_cpu_helpers_are_not_gte_exemptions(self):
        for name in ("gte_ldv0_short3", "gte_load_packed_short3",
                     "gte_store_ir123_packed_short3", "gte_store_third_output",
                     "gte_store_flag_bound", "gte_store_mac12_byte2",
                     "gte_store_mac123_byte3", "gte_stir123_matrix_column",
                     "gte_stsz3_s16"):
            with self.subTest(name=name):
                self.assertEqual(self.classify("void f(void) { %s(0); }" % name),
                                 "asm_constrained")

    def test_cpu_helper_in_direct_template_is_quarantined(self):
        with tempfile.TemporaryDirectory() as directory:
            root = pathlib.Path(directory)
            (root / "body.inc").write_text("void f(void) { gte_ldv0_short3(0); }")
            source = root / "unit.c"
            source.write_text('#include "body.inc"')
            self.assertEqual(source_quality.classify(source), "asm_constrained")

    def test_room_header_cpu_helpers_are_quarantined(self):
        for name in ("ROOMLIB_LOAD_S16", "ROOMLIB_LOAD_PTR", "ROOMLIB_LOAD_U16",
                     "ROOMLIB_DIV_V0_A0_CHECKED", "ROOM_M089_LOAD_EFFECT_LOOKUP"):
            with self.subTest(name=name):
                self.assertEqual(self.classify("void f(void) { %s(a, b); }" % name),
                                 "asm_constrained")

    def test_room_helper_mentions_are_not_calls(self):
        text = '''
        /* ROOMLIB_LOAD_S16(a, b); */
        const char *s = "ROOMLIB_DIV_V0_A0_CHECKED(a, b)";
        int f(void) { return 1; }
        '''
        self.assertEqual(self.classify(text), "semantic_c")

    def test_retired_room_cpu_helpers_are_not_defined(self):
        root = pathlib.Path(__file__).resolve().parents[2]
        for header, names in (
            ("room_lib/room_lib.h", ("ROOMLIB_LOAD_S16", "ROOMLIB_LOAD_PTR",
                                    "ROOMLIB_LOAD_U16", "ROOMLIB_DIV_V0_A0_CHECKED")),
            ("room_m089/room_m089.h", ("ROOM_M089_LOAD_EFFECT_LOOKUP",)),
        ):
            text = (root / "src/overlays" / header).read_text()
            for name in names:
                with self.subTest(name=name):
                    self.assertNotIn(name, text)

    def test_arc_divisions_use_c_and_stock_expansion(self):
        root = pathlib.Path(__file__).resolve().parents[2] / "src/overlays"
        templates = ("RoomLib_AdvanceArcToTarget.inc", "RoomLib_AdvanceArcToTargetY.inc")
        for name in templates:
            text = (root / "room_lib" / name).read_text()
            self.assertEqual(text.count("value /= count;"), 2)
            self.assertNotIn("ROOMLIB_DIV_V0_A0_CHECKED", text)
        callers = 0
        for source in root.rglob("*.c"):
            text = source.read_text()
            if any('room_lib/' + name in text for name in templates):
                callers += 1
                with self.subTest(source=str(source)):
                    self.assertIn("/* MASPSX_FLAGS: --expand-div */", text)
        self.assertGreater(callers, 0)

    def test_cpu_helper_mentions_are_not_calls(self):
        text = '''
        /* gte_ldv0_short3(0); */
        const char *s = "gte_store_third_output(0)";
        int f(void) { return 1; }
        '''
        self.assertEqual(self.classify(text), "semantic_c")

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

    def test_gte_address_arithmetic_is_not_automatically_exempt(self):
        text = r'void f(void *p) { asm("addiu $2,%0,32\n\t" "lwc2 $0,0($2)" : : "r"(p)); }'
        self.assertEqual(self.classify(text), "asm_constrained")

    def test_gte_transfer_only_concatenation_remains_supported(self):
        text = r'void f(void) { asm("mfc2 $2,$9\n\t" "nop"); }'
        self.assertEqual(self.classify(text), "semantic_c")

    def test_gte_transfer_does_not_hide_cpu_work(self):
        for suffix in (r'\naddu $2,$3,$4', r'; sh $2,0($4)',
                       r'\nsll $2,$2,16', r'\n.word 0',
                       r'\nlabel: nop', r'\nnop # mtc2'):
            with self.subTest(suffix=suffix):
                text = 'void f(void) { asm("mtc2 $2,$0%s"); }' % suffix
                self.assertEqual(self.classify(text), "asm_constrained")

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
        self.assertEqual(self.classify(text), "asm_constrained")

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
