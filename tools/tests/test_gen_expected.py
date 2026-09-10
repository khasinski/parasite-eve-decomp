import unittest
from pathlib import Path

import yaml

from tools.scripts import gen_expected


class MainDataLayoutTests(unittest.TestCase):
    def test_gp_data_is_not_text_and_engine_keeps_its_retail_address(self):
        root = Path(__file__).resolve().parents[2]
        config = yaml.safe_load((root / "configs/USA/main.yaml").read_text())
        segments = {s["name"]: s for s in config["segments"]
                    if isinstance(s, dict)}
        self.assertEqual(segments["main"]["subsegments"][-5:], [
            [0x818A0, "data", "main/dtail_gp_pre_s016"],
            [0x8B72C, ".data", "psyq/libcd/CdControl"],
            [0x8B7AC, "data", "main/dtail_gp_post_s016_pre_bios"],
            [0x8B7B4, ".data", "psyq/libcd/bios"],
            [0x8BABC, "data", "main/dtail_gp_post_bios"],
        ])
        engine = segments["field_engine"]
        self.assertEqual(engine["start"], 0xB24A0)
        self.assertEqual(engine["vram"], 0x800C1CA0)
        self.assertEqual(engine["subsegments"][0],
                         [0xB24A0, "asm", "engine/engine_800C1CA0"])


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

    def test_verified_c_function_can_replace_a_stale_alias(self):
        table = gen_expected.SymbolTable()
        table.add("StaleAlias", 0x80020DD0, "// type:func")
        table.add("ActualCFunction", 0x80020DD0, "// type:func",
                  replace_existing=True)

        self.assertEqual(table.by_addr[0x80020DD0][0], "ActualCFunction")

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

    def test_configured_padding_counts_unnamed_ranges(self):
        config = {
            "segments": [{
                "start": 0x800,
                "vram": 0x80010000,
                "subsegments": [
                    [0x1000, "c", "before"],
                    [0x1010, "pad"],
                    [0x1014, "c", "middle"],
                    [0x1020, "pad"],
                    [0x1028, "asm", "after"],
                ],
            }]
        }

        self.assertEqual(gen_expected.configured_pad_bytes(config), 12)


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

    def test_untyped_base_symbols_lose_their_size_too(self):
        # splat closes a glabel with enddlabel for data in .text; the .size
        # it would emit pairs a sized symbol against gcc's sizeless one,
        # which objdiff refuses to diff.
        text = "glabel Words\n    .word 0\nenddlabel Words\n"

        out = gen_expected.retype_data_in_text(text, {"Words": "STT_NOTYPE"})

        self.assertIn(".global Words\nWords:", out)
        self.assertNotIn("enddlabel", out)

    def test_internal_function_guess_becomes_a_plain_label(self):
        text = (
            "glabel Real\n  nop\nendlabel Real\n"
            "glabel FalseGuess\n  nop\nendlabel FalseGuess\n"
        )

        out = gen_expected.normalize_c_function_labels(text, {"Real"})

        self.assertIn("glabel Real", out)
        self.assertIn(".global FalseGuess\nFalseGuess:", out)
        self.assertEqual(out.count("endlabel Real"), 1)
        self.assertNotIn("endlabel FalseGuess", out)

    def test_internal_alternative_entry_loses_function_type(self):
        text = "glabel Real\n  nop\n  alabel FalseGuess\n  nop\nendlabel Real\n"

        out = gen_expected.normalize_c_function_labels(text, {"Real"})

        self.assertIn(".global FalseGuess\nFalseGuess:", out)
        self.assertNotIn("alabel FalseGuess", out)

    def test_trailing_text_data_does_not_extend_function_size(self):
        for kind in ("STT_OBJECT", "STT_NOTYPE"):
            with self.subTest(kind=kind):
                text = (
                    "glabel Real\n  jr $ra\n  nop\nendlabel Real\n"
                    "glabel Padding\n  .word 0\nenddlabel Padding\n"
                )
                text = gen_expected.retype_data_in_text(text, {"Padding": kind})
                out = gen_expected.normalize_c_function_labels(
                    text, {"Real"}, {"Padding"}
                )
                self.assertEqual(out.count("endlabel Real"), 1)
                self.assertLess(out.index("endlabel Real"), out.index("  .word 0"))
                self.assertIn("  .word 0", out)

    def test_text_data_between_functions_is_not_code_credit(self):
        text = (
            "glabel First\n  nop\nendlabel First\n"
            "glabel Padding\n  .word 0\nenddlabel Padding\n"
            "glabel Second\n  nop\nendlabel Second\n"
        )
        text = gen_expected.retype_data_in_text(text, {"Padding": "STT_NOTYPE"})
        out = gen_expected.normalize_c_function_labels(
            text, {"First", "Second"}, {"Padding"}
        )
        self.assertLess(out.index("endlabel First"), out.index("Padding:"))
        self.assertEqual(out.count("endlabel First"), 1)
        self.assertEqual(out.count("endlabel Second"), 1)

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
