"""PSYQ_ASM_FUNCTION is only for evidenced PSY-Q assembler objects."""
import json
import pathlib
import tempfile
import unittest

from tools.scripts import check_source_policy

ROOT = pathlib.Path(__file__).resolve().parents[2]

GOOD = '''/* ASSEMBLER: GNU */
#include "pe1/psyq_asm.h"
PSYQ_ASM_OBJECT(LIBGTE, MSC00)
PSYQ_ASM_FUNCTION(InitGeom,
    "    jr      $ra\\n"
    "    nop\\n");
'''

MAIN_YAML = '''segments:
  - name: main
    type: code
    subsegments:
      - [0x1000, c, psyq/libgte/msc00]
      - [0x1080, c, gte/game]
      - [0x1100, c, psyq/libgte/other]
      - [0x1200, pad]
'''


class PsyqAsmPolicyTests(unittest.TestCase):
    def errors(self, files, provenance=None):
        with tempfile.TemporaryDirectory() as tmp:
            root = pathlib.Path(tmp)
            src = root / "src"
            for name, text in files.items():
                path = src / name
                path.parent.mkdir(parents=True, exist_ok=True)
                path.write_text(text)
            config = root / "main.yaml"
            config.write_text(MAIN_YAML)
            manifest = root / "provenance.json"
            manifest.write_text(json.dumps({"evidence": provenance or [{
                "address": "0x%08X" % (0x1000 + 0x8000F800), "size": 0x80,
                "library": "LIBGTE", "object": "MSC00", "labels": []}]}))
            return check_source_policy.psyq_asm_errors(src, config, manifest)

    def test_evidenced_sdk_object_passes(self):
        self.assertEqual(self.errors({"main/psyq/libgte/msc00.c": GOOD}), [])

    def test_game_code_cannot_use_the_macro(self):
        errors = self.errors({"main/gte/game.c": GOOD})
        self.assertTrue(any("outside src/main/psyq" in e for e in errors), errors)

    def test_unit_must_lie_inside_the_named_object(self):
        errors = self.errors({"main/psyq/libgte/other.c": GOOD})
        self.assertTrue(any("not inside SDK object" in e for e in errors), errors)

    def test_object_must_be_named(self):
        text = GOOD.replace("PSYQ_ASM_OBJECT(LIBGTE, MSC00)\n", "")
        errors = self.errors({"main/psyq/libgte/msc00.c": text})
        self.assertTrue(any("one PSYQ_ASM_OBJECT" in e for e in errors), errors)

    def test_gnu_assembler_path_is_required(self):
        text = GOOD.replace("/* ASSEMBLER: GNU */\n", "")
        errors = self.errors({"main/psyq/libgte/msc00.c": text})
        self.assertTrue(any("ASSEMBLER: GNU" in e for e in errors), errors)

    def test_c_functions_cannot_share_the_unit(self):
        text = GOOD + "int helper(void) { return 0; }\n"
        errors = self.errors({"main/psyq/libgte/msc00.c": text})
        self.assertTrue(any("mixed with C functions" in e for e in errors), errors)

    def test_repository_sources_satisfy_the_policy(self):
        self.assertEqual(check_source_policy.psyq_asm_errors(), [])


if __name__ == "__main__":
    unittest.main()
