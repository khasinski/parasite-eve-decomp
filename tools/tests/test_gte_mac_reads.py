"""Guard the instruction windows extracted from the LoadAverage helpers."""
import ast
import pathlib
import re
import unittest

from tools.scripts import source_quality


ROOT = pathlib.Path(__file__).resolve().parents[2]


class GteMacReadTests(unittest.TestCase):
    def test_handler_b_and_c_addresses_are_c(self):
        directory = ROOT / "src/overlays/room_lib"
        for name in ("RoomLib_HandlerB.inc", "RoomLib_HandlerC.inc",
                     "RoomLib_HandlerBSceneReset.inc"):
            with self.subTest(name=name):
                text = (directory / name).read_text()
                self.assertFalse(source_quality.has_instruction_asm(text))
                self.assertNotIn("asm volatile(\"addiu", text)
        b = (directory / "RoomLib_HandlerB.inc").read_text()
        c = (directory / "RoomLib_HandlerC.inc").read_text()
        self.assertIn("gte_ldv0((char *)(base) + 0x20);", b)
        self.assertIn("gte_stmac((char *)scratch + 0x28);", b)
        self.assertIn("gte_ldv0((char *)scratch + 0x20);", c)
        self.assertIn("gte_stmac((char *)scratch + 0x38);", c)

    def test_handler_d_address_calculation_is_c(self):
        template = (ROOT / "src/overlays/room_lib/RoomLib_HandlerD.inc").read_text()
        self.assertFalse(source_quality.has_instruction_asm(template))
        self.assertIn("gte_stmac((char *)scratch + 8);", template)
        self.assertIn("gte_stmac((char *)scratch + 0x18);", template)
        self.assertNotIn("ROOMLIB_HANDLER_D_GTE_STORE_MAC", template)

    def test_retired_cpu_store_helpers_are_absent(self):
        header = source_quality.strip_comments(
            (ROOT / "include/pe1/gte.h").read_text())
        for name in ("gte_store_mac12_byte2", "gte_store_mac123_byte3",
                     "gte_store_third_output", "gte_store_flag_bound",
                     "gte_load_packed_short3", "gte_store_ir123_packed_short3",
                     "gte_ldv0_short3", "gte_stir123_matrix_column",
                     "gte_stsz3_s16"):
            with self.subTest(name=name):
                self.assertNotRegex(header, r"\b" + name + r"\s*\(")

    def test_column_and_depth_reads_do_not_emit_cpu_stores(self):
        header = source_quality.strip_comments(
            (ROOT / "include/pe1/gte.h").read_text())
        for name, expected in (
            ("gte_stir123_column", ["$12", "$13", "$14",
             "mfc2 %0,$9\n\tmfc2 %1,$10\n\tmfc2 %2,$11"]),
            ("gte_getsz3_for_store", ["mfc2 %0,$19\n\tnop"]),
        ):
            with self.subTest(name=name):
                definition = re.search(r"^#define " + name + r"\([^\n]*",
                                       header, re.MULTILINE)
                self.assertIsNotNone(definition)
                bodies = ["".join(ast.literal_eval(s) for s in re.findall(
                    source_quality.C_STRING, call.group("template")))
                    for call in source_quality.ASM_CALL.finditer(definition.group())]
                self.assertCountEqual(bodies, expected)

    def test_word_stride_vector_pack_has_only_cop2_instruction_asm(self):
        header = source_quality.strip_comments(
            (ROOT / "include/pe1/gte.h").read_text())
        definition = re.search(r"^#define gte_ldv0_word3\([^\n]*",
                               header, re.MULTILINE)
        self.assertIsNotNone(definition)
        bodies = ["".join(ast.literal_eval(s) for s in re.findall(
            source_quality.C_STRING, call.group("template")))
            for call in source_quality.ASM_CALL.finditer(definition.group())]
        self.assertCountEqual(bodies, ["$12", "$13", "", "",
                                      "mtc2 %0,$0\n\tlwc2 $1,8(%1)"])

    def test_staged_mac_reads_only_transfer_hardware_state(self):
        header = source_quality.strip_comments(
            (ROOT / "include/pe1/gte.h").read_text())
        for name, expected in (
            ("gte_getmac12_staged", ["mfc2 $8,$25", "mfc2 $9,$26"]),
            ("gte_getmac123_staged",
             ["mfc2 $8,$25", "mfc2 $9,$26", "mfc2 $10,$27"]),
        ):
            with self.subTest(name=name):
                definition = re.search(
                    r"^#define " + name + r"\([^\n]*", header, re.MULTILINE)
                self.assertIsNotNone(definition)
                calls = list(source_quality.ASM_CALL.finditer(definition.group()))
                self.assertEqual(len(calls), 1)
                body = "".join(ast.literal_eval(s) for s in re.findall(
                    source_quality.C_STRING, calls[0].group("template")))
                self.assertEqual([line.strip() for line in body.splitlines()], expected)


if __name__ == "__main__":
    unittest.main()
