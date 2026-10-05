#!/usr/bin/env python3
"""Reject source forms that can hide copied game assembly in src/."""
from __future__ import annotations

import ast
import json
import pathlib
import re
import sys
from collections import Counter

import yaml

try:
    from source_quality import (C_STRING, FUNCTION_DEF, GAME_ASM_USE, PSYQ_ASM_USE,
                                STACK_SWITCH_USE, classify, has_instruction_asm,
                                strip_comments)
except ImportError:
    from tools.scripts.source_quality import (C_STRING, FUNCTION_DEF, GAME_ASM_USE,
                                              PSYQ_ASM_USE, STACK_SWITCH_USE, classify,
                                              has_instruction_asm, strip_comments)


ROOT = pathlib.Path(__file__).resolve().parents[2]
SRC = ROOT / "src"
ASM_FILE = re.compile(r"^\s*\.(?:text|section|set|globl|ent)\b", re.MULTILINE)
ASM_INCLUDE = re.compile(r"^\s*\.include\b", re.MULTILINE)
PSYQ_ASM_OBJECT = re.compile(r"\bPSYQ_ASM_OBJECT\s*\(\s*(\w+)\s*,\s*(\w+)\s*\)")
ASM_FUNCTION_NAME = re.compile(r"\b(GAME|PSYQ)_ASM_FUNCTION\s*\(\s*(\w+)")
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
        if rel.parts[0] == "overlays":
            # Overlays have no PSY-Q provenance manifest; their SDK assembler
            # objects are checked against the original-assembler manifest.
            continue
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


def c_unit_ranges(config_path):
    """Retail address range of every `c` subsegment in a splat yaml, by path.

    Works for the main executable and for overlays: each row's file offset is
    mapped through its segment's vram, and a unit ends at the next subsegment
    or segment boundary.
    """
    config = yaml.safe_load(config_path.read_text())
    boundaries = set()
    rows = []
    for segment in config["segments"]:
        if isinstance(segment, list):
            boundaries.add(segment[0])
            continue
        if "start" in segment:
            boundaries.add(segment["start"])
        delta = segment.get("vram", segment.get("start", 0)) - segment.get("start", 0)
        for row in segment.get("subsegments", []):
            if isinstance(row, dict):
                start, kind, path = row["start"], row.get("type"), row.get("name")
            else:
                start = row[0]
                kind = row[1] if len(row) > 1 else None
                path = row[2] if len(row) > 2 else None
            boundaries.add(start)
            if kind == "c" and path:
                rows.append((path, start, delta))
    ordered = sorted(boundaries)
    ranges = {}
    for path, start, delta in rows:
        following = [offset for offset in ordered if offset > start]
        if following:
            ranges[path] = (start + delta, following[0] + delta)
    return ranges


def original_asm_manifest_errors(src=SRC, configs=None, manifest_path=None):
    """Game-side assembler (and overlay SDK assembler) needs listed evidence.

    GAME_ASM_FUNCTION anywhere, and PSYQ_ASM_FUNCTION in an overlay, may only
    define functions listed in configs/USA/original_asm_evidence.json. The
    file takes the GNU assembler path, contains no C function, defines
    exactly the functions listed for it, and its yaml `c` range is exactly
    those functions, contiguous, from the first address to the last end.
    Every manifest entry must be reproduced by its source and carry evidence.
    """
    root = src.parent
    configs = configs or root / "configs/USA"
    manifest_path = manifest_path or configs / "original_asm_evidence.json"
    errors = []
    entries = json.loads(manifest_path.read_text())["functions"] if manifest_path.exists() else []
    by_source = {}
    for entry in entries:
        label = entry.get("name", "?")
        missing = [key for key in ("name", "module", "source", "address", "size",
                                   "origin", "evidence", "verified_by") if not entry.get(key)]
        if missing:
            errors.append("original-asm manifest entry %s lacks %s" % (label, ", ".join(missing)))
            continue
        if entry["origin"] not in ("game", "psyq"):
            errors.append("original-asm manifest entry %s has unknown origin %r"
                          % (label, entry["origin"]))
            continue
        if entry["origin"] == "psyq" and (entry["module"] == "main"
                                          or not entry.get("library") or not entry.get("object")):
            errors.append("original-asm manifest entry %s: PSY-Q origin needs an overlay "
                          "module, library and object (main uses psyq_provenance.json)" % label)
            continue
        lead = "main/" if entry["module"] == "main" else "overlays/%s/" % entry["module"]
        if not entry["source"].startswith(lead) or not entry["source"].endswith(".c"):
            errors.append("original-asm manifest entry %s: source %s is not a %s unit"
                          % (label, entry["source"], entry["module"]))
            continue
        by_source.setdefault(entry["source"], []).append(entry)

    used = set()
    yaml_ranges = {}
    for path in sorted(src.rglob("*.c")):
        rel = path.relative_to(src)
        name = str(rel)
        code = strip_comments(path.read_text(errors="ignore"))
        game = bool(GAME_ASM_USE.search(code))
        overlay_psyq = rel.parts[0] == "overlays" and bool(PSYQ_ASM_USE.search(code))
        if not game and not overlay_psyq:
            continue
        listed = by_source.get(name)
        if not listed:
            errors.append("original assembler without manifest evidence: %s" % name)
            continue
        used.add(name)
        if "ASSEMBLER: GNU" not in path.read_text(errors="ignore"):
            errors.append("original assembler source without ASSEMBLER: GNU: %s" % name)
        if classify(path) != "original_asm":
            errors.append("original assembler source mixed with C functions: %s" % name)
        if game and PSYQ_ASM_USE.search(code):
            errors.append("game and PSY-Q assembler mixed in one unit: %s" % name)
            continue
        origin = "game" if game else "psyq"
        if any(entry["origin"] != origin for entry in listed):
            errors.append("%s: manifest origin does not match its %s macro"
                          % (name, "GAME_ASM_FUNCTION" if game else "PSYQ_ASM_FUNCTION"))
        if origin == "psyq":
            objects = PSYQ_ASM_OBJECT.findall(code)
            if len(objects) != 1 or any((entry["library"], entry["object"]) != objects[0]
                                        for entry in listed):
                errors.append("%s: PSYQ_ASM_OBJECT does not name the manifest object" % name)
        defined = [function for _, function in ASM_FUNCTION_NAME.findall(code)]
        expected = sorted(entry["name"] for entry in listed)
        if sorted(defined) != expected:
            errors.append("%s: defines %s, manifest lists %s"
                          % (name, ", ".join(sorted(defined)) or "nothing", ", ".join(expected)))
        module = listed[0]["module"]
        if module not in yaml_ranges:
            config = (configs / "main.yaml" if module == "main"
                      else configs / "overlays" / ("%s.yaml" % module))
            yaml_ranges[module] = c_unit_ranges(config) if config.exists() else {}
        lead = "main/" if module == "main" else "overlays/%s/" % module
        unit = name[len(lead):].removesuffix(".c")
        if unit not in yaml_ranges[module]:
            errors.append("original assembler source is not a %s yaml c unit: %s" % (module, name))
            continue
        start, end = yaml_ranges[module][unit]
        cursor = start
        for entry in sorted(listed, key=lambda item: int(item["address"], 16)):
            address = int(entry["address"], 16)
            if address != cursor:
                errors.append("%s: %s starts at 0x%08X, expected 0x%08X (unit range "
                              "0x%08X..0x%08X)" % (name, entry["name"], address, cursor, start, end))
                break
            cursor = address + entry["size"]
        else:
            if cursor != end:
                errors.append("%s: manifest functions end at 0x%08X but the yaml unit ends at 0x%08X"
                              % (name, cursor, end))
    for source in sorted(set(by_source) - used):
        errors.append("original-asm manifest lists %s, which does not reproduce it" % source)
    return errors


# The complete instruction text of include/pe1/boot_stack.h: park $sp in the
# top scratchpad word, point $sp below it, and after the call step back up and
# reload it. The macro may not grow beyond this window.
STACK_SWITCH_HEADER = "include/pe1/boot_stack.h"
STACK_SWITCH_WINDOW = [
    ["move $8,%0", "sw $29,0($8)", "addiu $8,$8,-4", "move $29,$8"],
    ["addiu $29,$29,4", "lw $29,0($29)"],
]
ASM_STATEMENT = re.compile(r'__asm__\s+volatile\s*\(\s*((?:%s\s*)+)' % C_STRING, re.S)


def stack_switch_window(text):
    """Instruction lists of each asm statement in the stack-switch header."""
    text = re.sub(r"\\\r?\n", "", text)
    windows = []
    for match in ASM_STATEMENT.finditer(text):
        body = "".join(ast.literal_eval(part) for part in re.findall(C_STRING, match.group(1)))
        windows.append([line.strip() for line in body.split("\n") if line.strip()])
    return windows


def enclosing_function(code, offset):
    """Name of the C function whose definition precedes `offset`, if any."""
    name = None
    for match in FUNCTION_DEF.finditer(code, 0, offset):
        name = re.search(r"([A-Za-z_]\w*)\s*\([^;{}]*\)\s*\{$", match.group()).group(1)
    return name


def stack_switch_errors(src=SRC, configs=None, manifest_path=None, header=None):
    """BOOT_CALL_ON_SCRATCHPAD_STACK is only for listed, evidenced functions.

    Each listed entry names a function, its unit and its retail range; the
    unit uses the macro exactly once per listed function, inside that
    function, has no other instruction asm (it classifies as semantic_c), and
    the function lies inside the unit's yaml range. The header keeps exactly
    the six-instruction window, and nothing else defines the macro.
    """
    root = src.parent
    configs = configs or root / "configs/USA"
    manifest_path = manifest_path or configs / "original_asm_evidence.json"
    header = header or root / STACK_SWITCH_HEADER
    errors = []
    entries = (json.loads(manifest_path.read_text()).get("stack_switch_macros", [])
               if manifest_path.exists() else [])
    if header.exists() and stack_switch_window(header.read_text()) != STACK_SWITCH_WINDOW:
        errors.append("%s no longer holds exactly the scratchpad stack-switch window"
                      % STACK_SWITCH_HEADER)
    by_source = {}
    for entry in entries:
        label = entry.get("name", "?")
        missing = [key for key in ("name", "module", "source", "address", "size",
                                   "evidence", "verified_by") if not entry.get(key)]
        if missing:
            errors.append("stack-switch entry %s lacks %s" % (label, ", ".join(missing)))
            continue
        by_source.setdefault(entry["source"], []).append(entry)

    for path in sorted((root / "include").rglob("*.h")):
        if path != header and re.search(
                r"#\s*define\s+BOOT_CALL_ON_SCRATCHPAD_STACK\b",
                strip_comments(path.read_text(errors="ignore"))):
            errors.append("stack-switch macro redefined outside %s: %s"
                          % (STACK_SWITCH_HEADER, path.relative_to(root)))

    used = set()
    yaml_ranges = {}
    allowed = frozenset(by_source)
    for path in sorted(list(src.rglob("*.c")) + list(src.rglob("*.inc")) + list(src.rglob("*.h"))):
        rel = path.relative_to(src)
        name = str(rel)
        code = re.sub(C_STRING, '""', strip_comments(path.read_text(errors="ignore")))
        if re.search(r"#\s*define\s+BOOT_CALL_ON_SCRATCHPAD_STACK\b", code):
            errors.append("stack-switch macro redefined outside %s: %s"
                          % (STACK_SWITCH_HEADER, name))
        uses = [match.start() for match in STACK_SWITCH_USE.finditer(code)]
        if not uses:
            continue
        listed = by_source.get(name)
        if not listed or path.suffix != ".c":
            errors.append("scratchpad stack switch without evidence: %s" % name)
            continue
        used.add(name)
        if classify(path, allowed) != "semantic_c":
            errors.append("stack-switch unit has other instruction asm: %s" % name)
        functions = sorted(enclosing_function(code, offset) or "?" for offset in uses)
        expected = sorted(entry["name"] for entry in listed)
        if functions != expected:
            errors.append("%s: stack switch used in %s, evidence lists %s"
                          % (name, ", ".join(functions), ", ".join(expected)))
        module = listed[0]["module"]
        if module not in yaml_ranges:
            config = (configs / "main.yaml" if module == "main"
                      else configs / "overlays" / ("%s.yaml" % module))
            yaml_ranges[module] = c_unit_ranges(config) if config.exists() else {}
        lead = "main/" if module == "main" else "overlays/%s/" % module
        unit = name[len(lead):].removesuffix(".c") if name.startswith(lead) else None
        if unit not in yaml_ranges[module]:
            errors.append("stack-switch source is not a %s yaml c unit: %s" % (module, name))
            continue
        start, end = yaml_ranges[module][unit]
        for entry in listed:
            address = int(entry["address"], 16)
            if not start <= address < address + entry["size"] <= end:
                errors.append("%s: %s 0x%08X+0x%X is outside the unit range 0x%08X..0x%08X"
                              % (name, entry["name"], address, entry["size"], start, end))
    for source in sorted(set(by_source) - used):
        errors.append("stack-switch evidence lists %s, which does not use the macro" % source)
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
                           ROOT / "include" / "pe1" / "boot_stack.h",
                           ROOT / "include" / "pe1" / "game_asm.h",
                           ROOT / "include" / "pe1" / "gte.h",
                           ROOT / "include" / "pe1" / "psyq_asm.h",
                           ROOT / "include" / "pe1" / "psyq_bios.h"}
    for path in (ROOT / "include").rglob("*.h"):
        if has_instruction_asm(path.read_text(errors="ignore")) and path not in allowed_asm_headers:
            errors.append("inline instruction asm in non-GTE header: %s" %
                          path.relative_to(ROOT))

    errors.extend(psyq_asm_errors())
    errors.extend(original_asm_manifest_errors())
    errors.extend(stack_switch_errors())

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
