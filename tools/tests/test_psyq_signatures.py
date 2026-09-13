"""SDK signature records retain their retail placement without code credit."""
import json
import pathlib
import unittest

import yaml

ROOT = pathlib.Path(__file__).resolve().parents[2]
CASES = [(0x64454, "A63", "puts", "50730121ad374200"),
         (0x68774, "MSC00", "InitGeom", "50730921ad424200")]


class PsyqSignaturesTests(unittest.TestCase):
    def test_records_precede_first_sdk_function_and_use_readonly_sections(self):
        manifest = yaml.safe_load((ROOT / "configs/USA/main.yaml").read_text())
        evidence = json.loads((ROOT / "configs/USA/psyq_provenance.json").read_text())["evidence"]
        rows = [r for s in manifest["segments"] if isinstance(s, dict)
                for r in s.get("subsegments", [])]
        for offset, obj, function, _ in CASES:
            with self.subTest(object=obj):
                index, row = next((i, r) for i, r in enumerate(rows)
                                  if isinstance(r, dict) and r.get("start") == offset)
                self.assertEqual(row["type"], "rodata")
                self.assertEqual(row["linker_section"], ".psyq_signature")
                self.assertEqual(row["linker_section_order"], ".text")
                self.assertEqual(rows[index + 1][0], offset + 8)
                owner = next(e for e in evidence if e["object"] == obj
                             and int(e["address"], 16) == offset + 0x8000F800)
                self.assertEqual(min(label["offset"] for label in owner["labels"]), 8)
                self.assertIn({"name": function, "offset": 8}, owner["labels"])

    @unittest.skipUnless((ROOT / "assets/USA/main.exe").exists(), "retail image unavailable")
    def test_signature_bytes(self):
        retail = (ROOT / "assets/USA/main.exe").read_bytes()
        for offset, obj, _, expected in CASES:
            with self.subTest(object=obj):
                self.assertEqual(retail[offset:offset + 8].hex(), expected)
