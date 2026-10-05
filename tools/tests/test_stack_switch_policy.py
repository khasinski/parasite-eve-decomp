"""BOOT_CALL_ON_SCRATCHPAD_STACK is limited to evidenced functions."""
import copy
import json
import pathlib
import tempfile
import unittest

from tools.scripts import check_source_policy

ROOT = pathlib.Path(__file__).resolve().parents[2]
HEADER = (ROOT / check_source_policy.STACK_SWITCH_HEADER).read_text()

USER = '''#include "pe1/boot_stack.h"
void helper(void) {
}

void main(void) {
    helper();
    BOOT_CALL_ON_SCRATCHPAD_STACK(BOOT_SCRATCHPAD_STACK_TOP, func_8019234C());
}
'''

MAIN_YAML = '''segments:
  - name: main
    type: code
    start: 0x800
    vram: 0x80010000
    subsegments:
      - [0x1000, c, boot/Boot_MainLoop]
      - [0x1100, c, boot/Other]
  - [0x2000]
'''

ENTRIES = [
    {"name": "main", "module": "main", "source": "main/boot/Boot_MainLoop.c",
     "address": "0x80010810", "size": 0xF0,
     "evidence": ["stack pointer switched around one call"], "verified_by": "test"},
]


class StackSwitchPolicyTests(unittest.TestCase):
    def errors(self, files=None, entries=None, header=HEADER, headers=None):
        files = {"main/boot/Boot_MainLoop.c": USER} if files is None else files
        with tempfile.TemporaryDirectory() as tmp:
            root = pathlib.Path(tmp)
            src = root / "src"
            for name, text in files.items():
                path = src / name
                path.parent.mkdir(parents=True, exist_ok=True)
                path.write_text(text)
            include = root / "include" / "pe1"
            include.mkdir(parents=True)
            (include / "boot_stack.h").write_text(header)
            for name, text in (headers or {}).items():
                (include / name).write_text(text)
            configs = root / "configs"
            configs.mkdir()
            (configs / "main.yaml").write_text(MAIN_YAML)
            manifest = configs / "original_asm_evidence.json"
            manifest.write_text(json.dumps({
                "functions": [],
                "stack_switch_macros": ENTRIES if entries is None else entries}))
            return check_source_policy.stack_switch_errors(src, configs, manifest)

    def assertError(self, errors, text):
        self.assertTrue(any(text in error for error in errors), errors)

    def test_listed_function_passes(self):
        self.assertEqual(self.errors(), [])

    def test_unlisted_unit_is_rejected(self):
        self.assertError(self.errors({"main/boot/Boot_MainLoop.c": USER,
                                      "main/boot/Other.c": USER}),
                         "without evidence: main/boot/Other.c")

    def test_use_in_another_function_is_rejected(self):
        text = USER.replace("void helper(void) {\n",
                            "void helper(void) {\n    BOOT_CALL_ON_SCRATCHPAD_STACK("
                            "BOOT_SCRATCHPAD_STACK_TOP, g());\n")
        self.assertError(self.errors({"main/boot/Boot_MainLoop.c": text}),
                         "used in helper, main")

    def test_second_use_in_the_listed_function_is_rejected(self):
        text = USER.replace("    helper();\n",
                            "    BOOT_CALL_ON_SCRATCHPAD_STACK(BOOT_SCRATCHPAD_STACK_TOP, g());\n")
        self.assertError(self.errors({"main/boot/Boot_MainLoop.c": text}),
                         "used in main, main")

    def test_other_instruction_asm_is_rejected(self):
        text = USER + 'void tail(void) { asm volatile("nop"); }\n'
        self.assertError(self.errors({"main/boot/Boot_MainLoop.c": text}),
                         "has other instruction asm")

    def test_evidence_is_required(self):
        entries = copy.deepcopy(ENTRIES)
        entries[0]["evidence"] = []
        self.assertError(self.errors(entries=entries), "lacks evidence")

    def test_function_must_lie_inside_the_unit(self):
        entries = copy.deepcopy(ENTRIES)
        entries[0]["size"] = 0x200
        self.assertError(self.errors(entries=entries), "outside the unit range")

    def test_stale_entry_is_rejected(self):
        self.assertError(self.errors(files={"main/boot/Other.c": "int f(void) { return 0; }\n"}),
                         "does not use the macro")

    def test_header_window_cannot_grow(self):
        header = HEADER.replace('"lw $29,0($29)"', '"lw $29,0($29)\\n"\n "nop"')
        self.assertError(self.errors(header=header), "no longer holds exactly")

    def test_macro_cannot_be_redefined_elsewhere(self):
        self.assertError(self.errors(headers={
            "other.h": "#define BOOT_CALL_ON_SCRATCHPAD_STACK(top, call) call\n"}),
            "redefined outside")
        text = "#define BOOT_CALL_ON_SCRATCHPAD_STACK(top, call) call\n" + USER
        self.assertError(self.errors({"main/boot/Boot_MainLoop.c": text}),
                         "redefined outside")

    def test_repository_window_and_users_are_consistent(self):
        self.assertEqual(check_source_policy.stack_switch_window(HEADER),
                         check_source_policy.STACK_SWITCH_WINDOW)
        self.assertEqual(check_source_policy.stack_switch_errors(), [])


if __name__ == "__main__":
    unittest.main()
