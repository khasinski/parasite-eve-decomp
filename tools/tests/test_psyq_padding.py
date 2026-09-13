"""Padding must be an unlabeled SDK object tail, not hidden unmatched code."""
import json
import pathlib
import struct
import unittest

import yaml

ROOT = pathlib.Path(__file__).resolve().parents[2]
PROVENANCE = json.loads((ROOT / "configs/USA/psyq_provenance.json").read_text())


class PsyqPaddingTests(unittest.TestCase):
    def test_padding_manifest_has_sdk_boundary_evidence(self):
        config = yaml.safe_load((ROOT / "configs/USA/main.yaml").read_text())
        manifest = {}
        for segment in config["segments"]:
            if not isinstance(segment, dict):
                continue
            rows = segment.get("subsegments", [])
            for row, following in zip(rows, rows[1:]):
                if isinstance(row, list) and row[1] == "pad":
                    end = following["start"] if isinstance(following, dict) else following[0]
                    manifest[row[0] + 0x8000F800] = end - row[0]
        approved = {int(p["address"], 16): p["size"] for p in PROVENANCE["padding"]}
        self.assertEqual({start: manifest.get(start) for start in approved}, approved)
        for start, size in approved.items():
            with self.subTest(address=hex(start)):
                self.assertIn(size, (4, 12))
                owners = [e for e in PROVENANCE["evidence"]
                          if e["scope"] == "object"
                          and int(e["address"], 16) < start
                          and int(e["address"], 16) + e["size"] == start + size]
                self.assertTrue(owners)
                for owner in owners:
                    self.assertFalse(any(start <= int(owner["address"], 16) + label["offset"]
                                         < start + size for label in owner["labels"]))

    @unittest.skipUnless((ROOT / "assets/USA/main.exe").exists(), "retail image unavailable")
    def test_retail_padding_is_zero_and_follows_return_delay_slot(self):
        retail = (ROOT / "assets/USA/main.exe").read_bytes()
        for padding in PROVENANCE["padding"]:
            offset = int(padding["address"], 16) - 0x8000F800
            with self.subTest(address=padding["address"]):
                self.assertEqual(retail[offset:offset + padding["size"]],
                                 bytes(padding["size"]))
                self.assertEqual(struct.unpack_from("<I", retail, offset - 8)[0],
                                 0x03E00008)  # jr $ra, followed by its delay slot
