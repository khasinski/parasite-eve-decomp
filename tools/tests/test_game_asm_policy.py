"""GAME_ASM_FUNCTION (and overlay PSYQ_ASM_FUNCTION) need manifest evidence."""
import copy
import json
import pathlib
import tempfile
import unittest

from tools.scripts import check_source_policy

GOOD = '''/* ASSEMBLER: GNU */
#include "pe1/game_asm.h"
GAME_ASM_FUNCTION(Math_RoundA,
    "    ori     $at, $zero, 0x8000\\n"
    "    add     $v0, $a0, $at\\n"
    "    jr      $ra\\n"
    "    sra     $v0, $v0, 16\\n");

GAME_ASM_FUNCTION(Math_RoundB,
    "    ori     $at, $zero, 0x8000\\n"
    "    add     $v0, $a0, $at\\n"
    "    jr      $ra\\n"
    "    sra     $v0, $v0, 8\\n");
'''

OVERLAY_GOOD = '''/* ASSEMBLER: GNU */
#include "pe1/psyq_asm.h"
PSYQ_ASM_OBJECT(LIBPRESS, VLC)
PSYQ_ASM_FUNCTION(DecSize,
    "    jr      $ra\\n"
    "    nop\\n");
'''

MAIN_YAML = '''segments:
  - name: header
    type: header
    start: 0x0
  - name: main
    type: code
    start: 0x800
    vram: 0x80010000
    subsegments:
      - [0x1000, c, math/math_fixed]
      - [0x1020, c, math/other]
      - [0x1040, asm, math/tail]
  - [0x2000]
'''

OVERLAY_YAML = '''segments:
  - name: ovl_pad
    type: code
    start: 0x0
    vram: 0x80100000
    subsegments:
      - [0x0, data, ovl_pad]
  - name: ovl_vlc
    type: code
    start: 0x8
    vram: 0x80100008
    subsegments:
      - [0x8, c, vlc]
  - name: ovl_tables
    type: code
    start: 0x10
    vram: 0x80100010
    subsegments:
      - [0x10, data, tables]
  - [0x100]
'''

ENTRIES = [
    {"name": "Math_RoundA", "module": "main", "source": "main/math/math_fixed.c",
     "address": "0x80010800", "size": 16, "origin": "game",
     "evidence": ["trapping add with $at"], "verified_by": "test"},
    {"name": "Math_RoundB", "module": "main", "source": "main/math/math_fixed.c",
     "address": "0x80010810", "size": 16, "origin": "game",
     "evidence": ["trapping add with $at"], "verified_by": "test"},
    {"name": "DecSize", "module": "ovl", "source": "overlays/ovl/vlc.c",
     "address": "0x80100008", "size": 8, "origin": "psyq",
     "library": "LIBPRESS", "object": "VLC",
     "evidence": ["matches the SDK object"], "verified_by": "test"},
]


class GameAsmPolicyTests(unittest.TestCase):
    def errors(self, files, entries=None):
        with tempfile.TemporaryDirectory() as tmp:
            root = pathlib.Path(tmp)
            src = root / "src"
            for name, text in files.items():
                path = src / name
                path.parent.mkdir(parents=True, exist_ok=True)
                path.write_text(text)
            configs = root / "configs"
            (configs / "overlays").mkdir(parents=True)
            (configs / "main.yaml").write_text(MAIN_YAML)
            (configs / "overlays" / "ovl.yaml").write_text(OVERLAY_YAML)
            manifest = configs / "original_asm_evidence.json"
            manifest.write_text(json.dumps({"functions": ENTRIES if entries is None else entries}))
            return check_source_policy.original_asm_manifest_errors(src, configs, manifest)

    def good_files(self):
        return {"main/math/math_fixed.c": GOOD, "overlays/ovl/vlc.c": OVERLAY_GOOD}

    def assertError(self, errors, text):
        self.assertTrue(any(text in error for error in errors), errors)

    def test_listed_units_pass(self):
        self.assertEqual(self.errors(self.good_files()), [])

    def test_unlisted_game_asm_is_rejected(self):
        files = self.good_files()
        files["main/math/other.c"] = GOOD.replace("Math_Round", "Other_Round")
        self.assertError(self.errors(files), "without manifest evidence: main/math/other.c")

    def test_overlay_psyq_asm_needs_the_manifest(self):
        entries = [entry for entry in ENTRIES if entry["module"] == "main"]
        self.assertError(self.errors(self.good_files(), entries),
                         "without manifest evidence: overlays/ovl/vlc.c")

    def test_gnu_assembler_marker_is_required(self):
        files = self.good_files()
        files["main/math/math_fixed.c"] = GOOD.replace("/* ASSEMBLER: GNU */\n", "")
        self.assertError(self.errors(files), "without ASSEMBLER: GNU")

    def test_c_functions_cannot_share_the_unit(self):
        files = self.good_files()
        files["main/math/math_fixed.c"] = GOOD + "int helper(void) { return 0; }\n"
        self.assertError(self.errors(files), "mixed with C functions")

    def test_defined_functions_must_match_the_manifest(self):
        files = self.good_files()
        files["main/math/math_fixed.c"] = GOOD.replace("Math_RoundB", "Math_RoundC")
        self.assertError(self.errors(files), "manifest lists Math_RoundA, Math_RoundB")

    def test_yaml_range_must_equal_the_listed_functions(self):
        entries = copy.deepcopy(ENTRIES)
        entries[1]["size"] = 12
        self.assertError(self.errors(self.good_files(), entries),
                         "manifest functions end at 0x8001081C but the yaml unit ends at 0x80010820")

    def test_listed_functions_must_be_contiguous(self):
        entries = copy.deepcopy(ENTRIES)
        entries[1]["address"] = "0x80010814"
        entries[1]["size"] = 12
        self.assertError(self.errors(self.good_files(), entries), "expected 0x80010810")

    def test_evidence_is_required(self):
        entries = copy.deepcopy(ENTRIES)
        entries[0]["evidence"] = []
        self.assertError(self.errors(self.good_files(), entries), "lacks evidence")

    def test_game_macro_needs_game_origin(self):
        entries = copy.deepcopy(ENTRIES)
        for entry in entries[:2]:
            entry.update(origin="psyq", library="LIBX", object="OBJ")
        self.assertError(self.errors(self.good_files(), entries), "PSY-Q origin needs an overlay")

    def test_overlay_psyq_object_must_match(self):
        files = self.good_files()
        files["overlays/ovl/vlc.c"] = OVERLAY_GOOD.replace("VLC)", "VLC_C)")
        self.assertError(self.errors(files), "does not name the manifest object")

    def test_stale_manifest_entry_is_rejected(self):
        files = self.good_files()
        del files["overlays/ovl/vlc.c"]
        self.assertError(self.errors(files), "lists overlays/ovl/vlc.c, which does not reproduce it")

    def test_overlay_ranges_follow_segment_vram(self):
        with tempfile.TemporaryDirectory() as tmp:
            path = pathlib.Path(tmp) / "ovl.yaml"
            path.write_text(OVERLAY_YAML)
            self.assertEqual(check_source_policy.c_unit_ranges(path),
                             {"vlc": (0x80100008, 0x80100010)})

    def test_repository_manifest_is_consistent(self):
        self.assertEqual(check_source_policy.original_asm_manifest_errors(), [])


if __name__ == "__main__":
    unittest.main()
