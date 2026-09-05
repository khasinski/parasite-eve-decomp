import hashlib
import pathlib
import tempfile
import unittest
from unittest import mock

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
            "semantic_c",
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
        self.assertEqual(entry["metadata"]["source_kind"], "semantic_c")

    def test_asm_constrained_c_gets_no_semantic_progress_base(self):
        entry = objdiff_config.unit(
            "src/main/test.c.o", "build/USA/", "main/test", "main-game",
            "src/main/test.c", True, "asm_constrained",
        )

        self.assertNotIn("base_path", entry)
        self.assertNotIn("complete", entry["metadata"])

    def test_text_data_gets_no_code_progress_base_or_override(self):
        entry = objdiff_config.unit(
            "src/main/words.c.o", "build/USA/", "main/words", "main-game",
            "src/main/words.c", True, "text_data",
        )

        self.assertNotIn("base_path", entry)
        self.assertNotIn("complete", entry["metadata"])

    def test_unverified_module_gets_no_semantic_progress_base(self):
        entry = objdiff_config.unit(
            "src/main/test.c.o", "build/USA/", "main/test", "main-game",
            "src/main/test.c", None, "semantic_c",
        )

        self.assertNotIn("base_path", entry)

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


class ModuleVerificationTests(unittest.TestCase):
    def config(self, digest):
        return {
            "sha1": digest,
            "options": {"build_path": "build/USA", "basename": "main"},
        }

    def test_missing_binary_is_not_verified(self):
        with tempfile.TemporaryDirectory() as directory:
            with mock.patch.object(objdiff_config, "ROOT", pathlib.Path(directory)):
                self.assertFalse(objdiff_config.module_verified(self.config("0" * 40)))

    def test_wrong_binary_hash_is_not_verified(self):
        with tempfile.TemporaryDirectory() as directory:
            root = pathlib.Path(directory)
            output = root / "build/USA/main.exe"
            output.parent.mkdir(parents=True)
            output.write_bytes(b"wrong")
            with mock.patch.object(objdiff_config, "ROOT", root):
                self.assertFalse(objdiff_config.module_verified(self.config("0" * 40)))

    def test_exact_binary_hash_is_verified(self):
        with tempfile.TemporaryDirectory() as directory:
            root = pathlib.Path(directory)
            output = root / "build/USA/main.exe"
            output.parent.mkdir(parents=True)
            payload = b"retail bytes"
            output.write_bytes(payload)
            digest = hashlib.sha1(payload).hexdigest()
            with mock.patch.object(objdiff_config, "ROOT", root):
                self.assertTrue(objdiff_config.module_verified(self.config(digest)))


if __name__ == "__main__":
    unittest.main()
