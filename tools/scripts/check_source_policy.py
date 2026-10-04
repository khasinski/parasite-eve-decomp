#!/usr/bin/env python3
"""Reject source forms that can hide copied game assembly in src/."""
from __future__ import annotations

import json
import pathlib
import re
import sys
from collections import Counter

import yaml

try:
    from source_quality import PSYQ_ASM_USE, classify, has_instruction_asm
except ImportError:
    from tools.scripts.source_quality import PSYQ_ASM_USE, classify, has_instruction_asm


ROOT = pathlib.Path(__file__).resolve().parents[2]
SRC = ROOT / "src"
ASM_FILE = re.compile(r"^\s*\.(?:text|section|set|globl|ent)\b", re.MULTILINE)
ASM_INCLUDE = re.compile(r"^\s*\.include\b", re.MULTILINE)
PSYQ_ASM_OBJECT = re.compile(r"\bPSYQ_ASM_OBJECT\s*\(\s*(\w+)\s*,\s*(\w+)\s*\)")
MAIN_VRAM_DELTA = 0x8000F800  # main.exe file offset -> retail address


def main_c_ranges(config_path):
    """Retail address range of every `c` subsegment in main.yaml, by path."""
    config = yaml.safe_load(config_path.read_text())
    ranges = {}
    for segment in config["segments"]:
        if not isinstance(segment, dict):
            continue
        rows = segment.get("subsegments", [])
        for row, following in zip(rows, rows[1:]):
            if not (isinstance(row, list) and len(row) >= 3 and row[1] == "c"):
                continue
            end = following["start"] if isinstance(following, dict) else following[0]
            ranges[row[2]] = (row[0] + MAIN_VRAM_DELTA, end + MAIN_VRAM_DELTA)
    return ranges


def psyq_asm_errors(src=SRC, config_path=None, provenance_path=None):
    """PSY-Q assembler-object sources must stay narrow and evidenced.

    Each user of include/pe1/psyq_asm.h lives under src/main/psyq/, takes the
    GNU assembler path, names one SDK object, and links entirely inside that
    object's retail range from configs/USA/psyq_provenance.json. Its
    classification must be original_asm, i.e. no C function definitions.
    """
    root = src.parent
    config_path = config_path or root / "configs/USA/main.yaml"
    provenance_path = provenance_path or root / "configs/USA/psyq_provenance.json"
    ranges = None
    evidence = None
    errors = []
    for path in sorted(src.rglob("*.c")):
        text = path.read_text(errors="ignore")
        if not PSYQ_ASM_USE.search(text):
            continue
        rel = path.relative_to(src)
        name = str(rel)
        if rel.parts[:2] != ("main", "psyq"):
            errors.append("PSY-Q assembler macro outside src/main/psyq/: %s" % name)
            continue
        if "ASSEMBLER: GNU" not in text:
            errors.append("PSY-Q assembler source without ASSEMBLER: GNU: %s" % name)
        if classify(path) != "original_asm":
            errors.append("PSY-Q assembler source mixed with C functions: %s" % name)
        objects = PSYQ_ASM_OBJECT.findall(text)
        if len(objects) != 1:
            errors.append("PSY-Q assembler source must name one PSYQ_ASM_OBJECT: %s" % name)
            continue
        if ranges is None:
            ranges = main_c_ranges(config_path)
            evidence = json.loads(provenance_path.read_text())["evidence"]
        unit = str(rel.relative_to("main").with_suffix(""))
        if unit not in ranges:
            errors.append("PSY-Q assembler source is not a main.yaml c unit: %s" % name)
            continue
        start, end = ranges[unit]
        library, obj = objects[0]
        if not any(record["library"] == library and record["object"] == obj
                   and int(record["address"], 16) <= start
                   and end <= int(record["address"], 16) + record["size"]
                   for record in evidence):
            errors.append("%s: range 0x%08X..0x%08X is not inside SDK object %s %s"
                          % (name, start, end, library, obj))
    return errors


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
                           ROOT / "include" / "pe1" / "psyq_asm.h",
                           ROOT / "include" / "pe1" / "psyq_bios.h"}
    for path in (ROOT / "include").rglob("*.h"):
        if has_instruction_asm(path.read_text(errors="ignore")) and path not in allowed_asm_headers:
            errors.append("inline instruction asm in non-GTE header: %s" %
                          path.relative_to(ROOT))

    errors.extend(psyq_asm_errors())

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
