"""GTE_LOAD_ROTATION/TRANSLATION_WINDOW are limited to evidenced functions."""
import copy
import json
import pathlib
import tempfile
import unittest

from tools.scripts import check_source_policy

ROOT = pathlib.Path(__file__).resolve().parents[2]
HEADER = (ROOT / check_source_policy.GTE_WINDOW_HEADER).read_text()

USER = '''#include "pe1/gte_window.h"
void helper(void) {
}

int draw(int mode) {
    helper();
    if (mode == 0) {
        GTE_LOAD_ROTATION_WINDOW(D_800BCFA4.value);
        GTE_LOAD_TRANSLATION_WINDOW(D_800BCFA4.value);
    } else {
        GTE_LOAD_ROTATION_WINDOW(D_800BCFA4.value);
        GTE_LOAD_TRANSLATION_WINDOW(D_800BCFA4.value);
    }
    return 0;
}
'''

OVERLAY_YAML = '''segments:
  - name: scene_x
    type: code
    start: 0x0
    vram: 0x8018EFE8
    subsegments:
      - [0x40, c, Flare]
      - [0x800, c, Other]
  - [0x1000]
'''

SOURCE = "overlays/scene_x/Flare.c"
ENTRIES = [
    {"name": "draw", "module": "scene_x", "source": SOURCE,
     "address": "0x8018F028", "size": 0x100,
     "windows": ["0x8018F0E0..0x8018F134", "0x8018F180..0x8018F1CC"],
     "evidence": ["view matrix moved into the GTE through t4..t6"],
     "verified_by": "test"},
]


class GteWindowPolicyTests(unittest.TestCase):
    def errors(self, files=None, entries=None, header=HEADER, headers=None, pending=None):
        files = {SOURCE: USER} if files is None else files
        with tempfile.TemporaryDirectory() as tmp:
            root = pathlib.Path(tmp)
            src = root / "src"
            for name, text in files.items():
                path = src / name
                path.parent.mkdir(parents=True, exist_ok=True)
                path.write_text(text)
            include = root / "include" / "pe1"
            include.mkdir(parents=True)
            (include / "gte_window.h").write_text(header)
            for name, text in (headers or {}).items():
                (include / name).write_text(text)
            configs = root / "configs"
            (configs / "overlays").mkdir(parents=True)
            (configs / "overlays" / "scene_x.yaml").write_text(OVERLAY_YAML)
            manifest = configs / "original_asm_evidence.json"
            manifest.write_text(json.dumps({
                "functions": [],
                "gte_matrix_windows": ENTRIES if entries is None else entries,
                "gte_matrix_windows_pending": pending or []}))
            return check_source_policy.gte_window_errors(src, configs, manifest)

    def assertError(self, errors, text):
        self.assertTrue(any(text in error for error in errors), errors)

    def test_listed_function_passes(self):
        self.assertEqual(self.errors(), [])

    def test_unlisted_unit_is_rejected(self):
        self.assertError(self.errors({SOURCE: USER, "overlays/scene_x/Other.c": USER}),
                         "without evidence: overlays/scene_x/Other.c")

    def test_use_in_another_function_is_rejected(self):
        text = USER.replace("void helper(void) {\n",
                            "void helper(void) {\n    GTE_LOAD_ROTATION_WINDOW(p);\n")
        self.assertError(self.errors({SOURCE: text}), "helper rotation x1")

    def test_extra_window_is_rejected(self):
        text = USER.replace("    helper();\n",
                            "    helper();\n    GTE_LOAD_ROTATION_WINDOW(p);\n"
                            "    GTE_LOAD_TRANSLATION_WINDOW(p);\n")
        self.assertError(self.errors({SOURCE: text}), "draw rotation x3")

    def test_unpaired_window_is_rejected(self):
        text = USER.replace("    } else {\n        GTE_LOAD_ROTATION_WINDOW(D_800BCFA4.value);\n",
                            "    } else {\n")
        self.assertError(self.errors({SOURCE: text}), "draw rotation x1")

    def test_other_instruction_asm_is_rejected(self):
        text = USER + 'void tail(void) { asm volatile("nop"); }\n'
        self.assertError(self.errors({SOURCE: text}), "has other instruction asm")

    def test_evidence_and_windows_are_required(self):
        for key in ("evidence", "windows"):
            entries = copy.deepcopy(ENTRIES)
            entries[0][key] = []
            self.assertError(self.errors(entries=entries), "lacks " + key)

    def test_function_must_lie_inside_the_unit(self):
        entries = copy.deepcopy(ENTRIES)
        entries[0]["size"] = 0x1000
        self.assertError(self.errors(entries=entries), "outside the unit range")

    def test_stale_entry_is_rejected(self):
        self.assertError(self.errors(files={"overlays/scene_x/Other.c": "int f(void) { return 0; }\n"}),
                         "does not use the macros")

    def test_pending_entry_cannot_also_be_listed(self):
        self.assertEqual(self.errors(pending=[{"name": "other", "module": "scene_x"}]), [])
        self.assertError(self.errors(pending=[{"name": "draw", "module": "scene_x"}]),
                         "both as a user and as pending")

    def test_header_windows_cannot_grow(self):
        header = HEADER.replace('"ctc2 $14,$7"', '"ctc2 $14,$7\\n"\n "nop"')
        self.assertError(self.errors(header=header), "no longer holds exactly")

    def test_macros_cannot_be_redefined_elsewhere(self):
        self.assertError(self.errors(headers={
            "other.h": "#define GTE_LOAD_ROTATION_WINDOW(m) ((void)(m))\n"}),
            "redefined outside")
        text = "#define GTE_LOAD_TRANSLATION_WINDOW(m) ((void)(m))\n" + USER
        self.assertError(self.errors({SOURCE: text}), "redefined outside")

    def test_repository_windows_and_users_are_consistent(self):
        self.assertEqual(check_source_policy.stack_switch_window(HEADER),
                         check_source_policy.GTE_WINDOW_TEXT)
        self.assertEqual(check_source_policy.gte_window_errors(), [])


if __name__ == "__main__":
    unittest.main()
