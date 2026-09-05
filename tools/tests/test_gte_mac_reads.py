"""Guard the instruction windows extracted from the LoadAverage helpers."""
import ast
import pathlib
import re
import unittest

from tools.scripts import source_quality


ROOT = pathlib.Path(__file__).resolve().parents[2]


class GteMacReadTests(unittest.TestCase):
    def test_retired_cpu_store_helpers_are_absent(self):
        header = source_quality.strip_comments(
            (ROOT / "include/pe1/gte.h").read_text())
        for name in ("gte_store_mac12_byte2", "gte_store_mac123_byte3",
                     "gte_store_third_output", "gte_store_flag_bound",
                     "gte_load_packed_short3", "gte_store_ir123_packed_short3"):
            with self.subTest(name=name):
                self.assertNotRegex(header, r"\b" + name + r"\s*\(")

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
