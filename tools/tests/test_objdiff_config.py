import unittest

from tools.scripts import objdiff_config


MAP_TEXT = """
 .text          0x80012850      0x3d0 build/USA/src/main/task/Task_EvalExpr.c.o
 .rodata        0x80010000       0x60 build/USA/src/main/task/Task_EvalExpr.c.o
 .text          0x800c3324      0x7e0 build/USA/asm/USA/main/engine/engine_800C3324.s.o
 .text          0x80010060        0x0 build/USA/asm/USA/main/data/main/rodata_0009AC.rodata.s.o
 .rodata        0x80010060      0x538 build/USA/asm/USA/main/data/main/rodata_0009AC.rodata.s.o
"""


class LinkedObjectTests(unittest.TestCase):
    def test_the_map_says_which_objects_carry_code(self):
        objects = objdiff_config.linked_objects(MAP_TEXT)

        self.assertTrue(objects["build/USA/src/main/task/Task_EvalExpr.c.o"]["code"])
        self.assertTrue(
            objects["build/USA/asm/USA/main/engine/engine_800C3324.s.o"]["code"]
        )
        self.assertFalse(
            objects["build/USA/asm/USA/main/data/main/rodata_0009AC.rodata.s.o"]["code"]
        )


class CategoryTests(unittest.TestCase):
    def test_sony_library_objects_report_under_their_own_category(self):
        self.assertEqual(
            objdiff_config.main_category("src/main/psyq/libcd/cd3.c.o"), "main-psyq"
        )
        self.assertEqual(
            objdiff_config.main_category("src/main/battle/battle1.c.o"), "main-game"
        )

    def test_overlays_group_by_their_role(self):
        self.assertEqual(objdiff_config.overlay_category("room_m005"), "rooms")
        self.assertEqual(objdiff_config.overlay_category("scene_e01"), "scenes")
        self.assertEqual(objdiff_config.overlay_category("fx_field"), "system")
        self.assertEqual(objdiff_config.overlay_category("sys_reset"), "system")

    def test_every_category_used_is_declared(self):
        # objdiff silently drops a unit's category when the config never
        # declares it, so the declaration list is load-bearing.
        declared = {category["id"] for category in objdiff_config.CATEGORIES}
        used = {
            objdiff_config.main_category("src/main/psyq/libcd/cd3.c.o"),
            objdiff_config.main_category("src/main/battle/battle1.c.o"),
            objdiff_config.overlay_category("room_m005"),
            objdiff_config.overlay_category("scene_e01"),
            objdiff_config.overlay_category("fx_field"),
        }

        self.assertLessEqual(used, declared)


class UnitShapeTests(unittest.TestCase):
    def test_a_decompiled_unit_compares_source_against_retail(self):
        entry = objdiff_config.unit(
            "src/main/task/Task_EvalExpr.c.o",
            "build/USA/",
            "main/task/Task_EvalExpr",
            "main-game",
            "src/main/task/Task_EvalExpr.c",
            True,
        )

        self.assertEqual(
            entry["target_path"],
            "expected/build/USA/src/main/task/Task_EvalExpr.c.o",
        )
        self.assertEqual(
            entry["base_path"], "build/USA/src/main/task/Task_EvalExpr.c.o"
        )
        self.assertEqual(
            entry["metadata"]["source_path"], "src/main/task/Task_EvalExpr.c"
        )
        self.assertTrue(entry["metadata"]["complete"])

    def test_an_undecompiled_unit_gets_no_base_to_match(self):
        # Scoring the tree's own assembly against the retail disassembly
        # would count original bytes as decompilation that never happened.
        entry = objdiff_config.unit(
            "asm/USA/main/engine/engine_800C3324.s.o",
            "build/USA/",
            "main/engine/engine_800C3324",
            "main-game",
            None,
            None,
        )

        self.assertNotIn("base_path", entry)
        self.assertNotIn("source_path", entry["metadata"])
        self.assertNotIn("complete", entry["metadata"])


class HasmUnitTests(unittest.TestCase):
    def test_a_hasm_unit_is_a_source_backed_unit(self):
        entry = objdiff_config.unit(
            "src/main/psyq/libapi/EnterCriticalSection.s.o",
            "build/USA/",
            "main/psyq/libapi/EnterCriticalSection",
            "main-psyq",
            "src/main/psyq/libapi/EnterCriticalSection.s",
            None,
        )

        self.assertEqual(
            entry["base_path"],
            "build/USA/src/main/psyq/libapi/EnterCriticalSection.s.o",
        )
        self.assertEqual(
            entry["metadata"]["source_path"],
            "src/main/psyq/libapi/EnterCriticalSection.s",
        )


if __name__ == "__main__":
    unittest.main()
