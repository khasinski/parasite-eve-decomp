import unittest

from tools.scripts import gen_expected


class SymbolTableTests(unittest.TestCase):
    def test_harvested_name_replaces_a_generic_one(self):
        table = gen_expected.SymbolTable()
        table.add("func_80020DD0", 0x80020DD0, "// type:func")
        table.add("Battle_InitEntityColors", 0x80020DD0, "// type:func",
                  replace_generic=True)

        self.assertEqual(table.by_addr[0x80020DD0][0], "Battle_InitEntityColors")
        self.assertNotIn("func_80020DD0", table.names)

    def test_a_deliberate_name_is_not_replaced(self):
        table = gen_expected.SymbolTable()
        table.add("Entity_SetActionMode", 0x8001A680, "// type:func")
        table.add("SomethingElse", 0x8001A680, "", replace_generic=True)

        self.assertEqual(table.by_addr[0x8001A680][0], "Entity_SetActionMode")

    def test_one_name_never_lands_at_two_addresses(self):
        table = gen_expected.SymbolTable()
        table.add("shared_static", 0x80010000)
        table.add("shared_static", 0x80020000)

        self.assertEqual(table.names["shared_static"], 0x80010000)
        self.assertNotIn(0x80020000, table.by_addr)

    def test_compiler_bookkeeping_names_are_dropped(self):
        table = gen_expected.SymbolTable()
        table.add("gcc2_compiled.", 0x80010000)
        table.add("Real_Name.NON_MATCHING", 0x80010004)

        self.assertEqual(table.by_addr, {})

    def test_an_overlay_window_is_carved_out_of_the_main_table(self):
        table = gen_expected.SymbolTable()
        table.add("MainFunction", 0x80020000)
        table.add("BufferResident", 0x8018F000)

        carved = table.without_range([(0x8018EFE8, 0x801A0000)])

        self.assertIn("MainFunction", carved.names)
        self.assertNotIn("BufferResident", carved.names)


class SubsegmentSliceTests(unittest.TestCase):
    def test_file_offsets_map_to_vram_by_one_affine_shift(self):
        config = {
            "segments": [
                {"name": "header", "type": "header", "start": 0},
                {
                    "start": 0x800,
                    "vram": 0x80010000,
                    "subsegments": [
                        [0x800, ".rodata", "task/Task_EvalExpr"],
                        [0x3050, "c", "task/Task_EvalExpr"],
                        [0x38B4, "asm", "task/Task_DispatchCmd"],
                    ],
                },
            ]
        }

        slices, spans = gen_expected.subsegment_slices(config)

        self.assertEqual(slices["task/Task_EvalExpr"][".text"], 0x80012850)
        self.assertEqual(slices["task/Task_EvalExpr"][".rodata"], 0x80010000)
        self.assertEqual(slices["task/Task_DispatchCmd"][".text"], 0x800130B4)
        self.assertEqual(spans, [(0x80010000, 0x800130B4)])


class DisassemblyRewriteTests(unittest.TestCase):
    def test_differ_aliases_are_stripped(self):
        text = "nonmatching func_80010000, 0x10\nglabel func_80010000\n"

        self.assertEqual(gen_expected.strip_differ_aliases(text),
                         "glabel func_80010000\n")

    def test_data_in_text_keeps_the_base_objects_typing(self):
        text = "glabel WordTable\n    .word 0\nendlabel WordTable\n"

        out = gen_expected.retype_data_in_text(text, {"WordTable": "STT_OBJECT"})

        self.assertIn("dlabel WordTable", out)
        self.assertIn("enddlabel WordTable", out)

    def test_untyped_base_symbols_get_a_bare_label(self):
        text = "glabel Stub\n    .word 0\nendlabel Stub\n"

        out = gen_expected.retype_data_in_text(text, {"Stub": "STT_NOTYPE"})

        self.assertIn(".global Stub\nStub:", out)
        self.assertNotIn("endlabel", out)

    def test_addresses_below_the_load_address_become_constants_again(self):
        constants = gen_expected.invented_constants(
            "D_7FFFFF = 0x7FFFFF;\nD_80020000 = 0x80020000;\n"
        )

        self.assertEqual(constants, {"D_7FFFFF": 0x7FFFFF})

        out = gen_expected.inline_constant_pairs(
            "  lui $v0, %hi(D_7FFFFF)\n  addiu $v0, $v0, %lo(D_7FFFFF)\n",
            constants,
        )

        self.assertIn("lui $v0, 0x80", out)
        self.assertIn("addiu $v0, $v0, -1", out)

    def test_constant_halves_carry_the_assemblers_sign_adjustment(self):
        high, low = gen_expected.halves(0x7FFFFF)

        self.assertEqual((high << 16) + low, 0x7FFFFF)
        self.assertEqual(high, 0x80)


if __name__ == "__main__":
    unittest.main()
