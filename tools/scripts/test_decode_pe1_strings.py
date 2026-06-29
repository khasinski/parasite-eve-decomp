#!/usr/bin/env python3
from __future__ import annotations

import importlib.util
import sys
import unittest
from pathlib import Path


SCRIPT = Path(__file__).with_name("decode_pe1_strings.py")
spec = importlib.util.spec_from_file_location("decode_pe1_strings", SCRIPT)
assert spec is not None and spec.loader is not None
decode_pe1_strings = importlib.util.module_from_spec(spec)
sys.modules["decode_pe1_strings"] = decode_pe1_strings
spec.loader.exec_module(decode_pe1_strings)


class TestDecodePe1Strings(unittest.TestCase):
    def test_decodes_status_effect_string(self) -> None:
        raw = bytes.fromhex(
            "22 44 42 32 34 3F 43 38 31 3B 34 0F 43 3E 0F 3F 3E 38 42 3E 3D"
        )

        text, printable_count, alpha_count = decode_pe1_strings.decode_bytes(raw)

        self.assertEqual(text, "Susceptible to poison")
        self.assertEqual(printable_count, len("Susceptible to poison"))
        self.assertEqual(alpha_count, len("Susceptibletopoison"))

    def test_decodes_digits_with_offset_mapping(self) -> None:
        raw = bytes([0x13, 0x18, 0x22, 0x12, 0x0F, 0x01])

        text, _, _ = decode_pe1_strings.decode_bytes(raw)

        self.assertEqual(text, "DISC 2")

    def test_renders_unmapped_high_bytes_as_tokens(self) -> None:
        text, printable_count, alpha_count = decode_pe1_strings.decode_bytes(
            bytes([0x0F, 0xFB, 0x10])
        )

        self.assertEqual(text, " <FB>A")
        self.assertEqual(printable_count, 2)
        self.assertEqual(alpha_count, 1)

    def test_scans_terminated_candidates(self) -> None:
        data = b"\xAA\xBB" + bytes.fromhex(
            "22 44 42 32 34 3F 43 38 31 3B 34 0F 43 3E 0F 3F 3E 38 42 3E 3D FF"
        )

        rows = decode_pe1_strings.scan_strings(data, min_chars=8)

        self.assertEqual(len(rows), 1)
        self.assertEqual(rows[0].offset, 2)
        self.assertEqual(rows[0].length, 22)
        self.assertEqual(rows[0].text, "Susceptible to poison")

    def test_scan_offsets_can_be_based_to_file_window(self) -> None:
        data = bytes.fromhex(
            "22 44 42 32 34 3F 43 38 31 3B 34 0F 43 3E 0F 3F 3E 38 42 3E 3D FF"
        )

        rows = decode_pe1_strings.scan_strings(
            data,
            min_chars=8,
            scan_mode="terminated-runs",
            base_offset=0x81C96,
        )

        self.assertEqual(len(rows), 1)
        self.assertEqual(rows[0].offset, 0x81C96)
        self.assertEqual(rows[0].text, "Susceptible to poison")


if __name__ == "__main__":
    unittest.main()
