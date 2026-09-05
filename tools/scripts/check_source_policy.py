#!/usr/bin/env python3
"""Reject source forms that can hide copied game assembly in src/."""
from __future__ import annotations

import pathlib
import re
import sys
from collections import Counter

try:
    from source_quality import classify, has_instruction_asm
except ImportError:
    from tools.scripts.source_quality import classify, has_instruction_asm


ROOT = pathlib.Path(__file__).resolve().parents[2]
SRC = ROOT / "src"
ASM_FILE = re.compile(r"^\s*\.(?:text|section|set|globl|ent)\b", re.MULTILINE)
ASM_INCLUDE = re.compile(r"^\s*\.include\b", re.MULTILINE)


def main() -> int:
    errors = []
    for path in SRC.rglob("*.s"):
        errors.append("assembly source under src/: %s" % path.relative_to(ROOT))
    for path in SRC.rglob("*.inc"):
        text = path.read_text(errors="ignore")
        if ASM_FILE.search(text):
            errors.append("assembly-form .inc under src/: %s" % path.relative_to(ROOT))
        if ASM_INCLUDE.search(text):
            errors.append("assembler include under src/: %s" % path.relative_to(ROOT))
    allowed_asm_headers = {ROOT / "include" / "include_asm.h",
                           ROOT / "include" / "pe1" / "gte.h",
                           ROOT / "include" / "pe1" / "psyq_bios.h"}
    for path in (ROOT / "include").rglob("*.h"):
        if has_instruction_asm(path.read_text(errors="ignore")) and path not in allowed_asm_headers:
            errors.append("inline instruction asm in non-GTE header: %s" %
                          path.relative_to(ROOT))

    counts = Counter(classify(path) for path in SRC.rglob("*.c"))
    if errors:
        print("Source policy violations:")
        for error in errors:
            print("  " + error)
        return 1
    print("OK: no assembly source or assembly-form include is stored under src/.")
    print("  " + ", ".join("%s=%d" % item for item in sorted(counts.items())))
    return 0


if __name__ == "__main__":
    sys.exit(main())
