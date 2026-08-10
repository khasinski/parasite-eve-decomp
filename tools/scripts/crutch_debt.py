#!/usr/bin/env python3
"""Crutch-debt tracker: count the matching crutches that still have to be
removed to reach clean C, so the number can be watched shrinking.

GOAL.md wants plain C with no `asm`/`__asm__` of any kind. A function can be
byte-matched yet still carry crutches: register pins, empty asm barriers,
`asm("sym")` aliases, real asm bodies, `goto` chains, INCLUDE_ASM. This script
counts each per subsystem (read-only; no build side effects) and writes
docs/CRUTCH_DEBT.md + docs/badges/debt.json.

Run: `make debt` (or `python tools/scripts/crutch_debt.py`).
"""
from __future__ import annotations

import argparse
import json
import pathlib
import re

ROOT = pathlib.Path(__file__).resolve().parents[2]
SRC = ROOT / "src"
BASELINE = ROOT / "tools" / "crutch_debt_baseline.json"


def strip_comments(text: str) -> str:
    return re.sub(r"//.*", "", re.sub(r"/\*.*?\*/", "", text, flags=re.S))


# GTE / hardware coprocessor ops that are legitimately expressed as asm and are
# not "dirty" crutches to remove (kept out of the ASM_BODY count).
_HW_OPS = "cfc2|ctc2|lwc2|swc2|mfc2|mtc2|nop"

PATTERNS = {
    "byte_pointer_arithmetic": re.compile(
        r"\(\s*(?:const\s+|volatile\s+)?(?:u8|s8|char)\s*\*\s*\)"
        r"(?!\s*\()[^;=\n]*\+"
    ),
    "raw_offset_dereferences": re.compile(
        r"\*\s*\([^)]*\*\s*\)\s*\("
        r"(?:[^();\n]|\([^();\n]*\))*?[+-]\s*(?:0x[0-9A-Fa-f]+|[1-9][0-9]*)"
    ),
    "pointer_integer_casts": re.compile(
        r"\(\s*(?:s32|u32|long|unsigned\s+long)\s*\)\s*(?:[A-Za-z_&]|\()"
    ),
    "field_macros": re.compile(r"\b(?:M2C_)?(?:RAW_)?FIELD\w*\s*\("),
    # register pins: `register T x asm("$8");` / `asm("$v0")`
    "pins": re.compile(r'register\s+[^;]*\basm\s*\(\s*"\$?\w+"\s*\)'),
    # empty asm barriers: asm(""), excluding the sanctioned REGALLOC_BARRIER macro
    "barriers": re.compile(r'(?<!REGALLOC_BARRIER)(?:__asm__|asm)\s*(?:volatile|__volatile__)?\s*\(\s*""'),
    # symbol aliases: `extern T x asm("D_800...");`
    "aliases": re.compile(r'extern\s+[^;]*\basm\s*\(\s*"[A-Za-z_]'),
    # real asm instruction bodies (exclude empty barriers, directives, HW ops)
    "asm_bodies": re.compile(rf'\basm\s*(?:volatile|__volatile__)?\s*\(\s*"(?!\.|")(?!(?:{_HW_OPS})\b)[a-z]'),
    # assembler directives inside asm(): asm(".word ...")
    "directives": re.compile(r'\basm\s*\(\s*"\.'),
    "gotos": re.compile(r'\bgoto\b'),
    "include_asm": re.compile(r'\bINCLUDE_ASM\b'),
    "postpass": re.compile(r'\bCC_POSTPASS\b'),
    "statement_expressions": re.compile(r"\(\s*\{"),
    "unknown_fields": re.compile(r"\b(?:field_[0-9A-Fa-f]+|unk[0-9A-Fa-f]+)\b"),
    "declaration_overrides": re.compile(
        r"^\s*#define\s+GAME_[A-Z0-9_]+_(?:TYPE|QUALIFIER|DECL)\b",
        re.MULTILINE,
    ),
    # Declarations belong in subsystem headers so ABI information has one
    # source of truth. Existing declarations are migration debt.
    "externs_in_c": re.compile(r'^\s*extern\b', re.MULTILINE),
}

ORDER = [
    "byte_pointer_arithmetic", "raw_offset_dereferences",
    "pointer_integer_casts", "field_macros", "pins", "barriers", "aliases",
    "asm_bodies", "directives", "gotos", "include_asm", "postpass",
    "statement_expressions", "unknown_fields", "declaration_overrides",
    "externs_in_c",
]
HEAVY = [key for key in ORDER if key != "gotos"]


def subsystem_of(rel: pathlib.PurePath) -> str:
    parts = rel.parts
    if parts[:2] == ("src", "overlays"):
        return "overlays"
    if parts[:2] == ("src", "main") and len(parts) >= 3:
        return f"main/{parts[2]}"
    return parts[1] if len(parts) > 1 else "src"


def collect_debt(source_root: pathlib.Path = SRC):
    from collections import defaultdict

    per_sub = defaultdict(lambda: {k: 0 for k in ORDER} | {"dirty_files": 0})
    totals = {k: 0 for k in ORDER}
    dirty_files = 0

    for path in sorted(source_root.rglob("*.c")):
        # Prefix with ``src`` so tests can pass an isolated source tree while
        # subsystem_of keeps using repository-relative paths.
        rel = pathlib.PurePath("src") / path.relative_to(source_root)
        text = strip_comments(path.read_text(errors="ignore"))
        sub = subsystem_of(pathlib.PurePath(rel))
        counts = {k: len(PATTERNS[k].findall(text)) for k in ORDER}
        for k, v in counts.items():
            per_sub[sub][k] += v
            totals[k] += v
        if any(counts[k] for k in HEAVY):
            per_sub[sub]["dirty_files"] += 1
            dirty_files += 1

    scopes = {
        "main": {k: 0 for k in ORDER},
        "overlays": {k: 0 for k in ORDER},
    }
    for name, counts in per_sub.items():
        scope = "overlays" if name == "overlays" else "main"
        for key in ORDER:
            scopes[scope][key] += counts[key]

    return per_sub, totals, dirty_files, scopes


def render_report(per_sub, totals, dirty_files) -> str:
    def row(name, c):
        cells = " | ".join(str(c[k]) for k in ORDER)
        return f"| {name} | {cells} | {c['dirty_files']} |"

    header = "| subsystem | " + " | ".join(ORDER) + " | dirty files |"
    sep = "|" + "---|" * (len(ORDER) + 2)
    subs_sorted = sorted(
        per_sub.items(),
        key=lambda kv: sum(kv[1][k] for k in HEAVY),
        reverse=True,
    )
    lines = [
        "# Crutch debt",
        "",
        "_Matching crutches still to remove to reach clean C (GOAL.md). "
        "Read-only; regenerate with `make debt`. Watch these shrink._",
        "",
        "**pins** = `register T x asm(\"$r\")` · **barriers** = empty `asm(\"\")` · "
        "**aliases** = `extern T x asm(\"sym\")` · **asm_bodies** = real instructions · "
        "**directives** = `asm(\".word ...\")` · **gotos** · **include_asm** · **postpass** · "
        "**externs_in_c** = declarations awaiting a subsystem header. Raw offset, pointer, "
        "field-macro, statement-expression, unknown-field, and declaration-override columns "
        "track semantic/typing scaffolding.",
        "",
        header,
        sep,
    ]
    for name, c in subs_sorted:
        if any(c[k] for k in ORDER):
            lines.append(row(name, c))
    total_counts = dict(totals)
    total_counts["dirty_files"] = dirty_files
    lines.append(row("**TOTAL**", total_counts))
    return "\n".join(lines) + "\n"


def write_reports(per_sub, totals, dirty_files) -> None:
    (ROOT / "docs" / "CRUTCH_DEBT.md").write_text(
        render_report(per_sub, totals, dirty_files)
    )

    # ----- badge (date-free, code.json shape) -----
    msg = (f"{totals['pins']} pins / {totals['barriers']} barriers / "
           f"{totals['gotos']} gotos / {totals['aliases']} aliases")
    heavy_total = sum(totals[k] for k in HEAVY)
    color = "brightgreen" if heavy_total == 0 else ("yellow" if heavy_total < 500 else "orange" if heavy_total < 1500 else "red")
    (ROOT / "docs" / "badges" / "debt.json").write_text(json.dumps({
        "schemaVersion": 1, "label": "crutch debt", "message": msg, "color": color,
    }))

    print(f"wrote {ROOT/'docs'/'CRUTCH_DEBT.md'}")
    print("  " + msg + f" | asm_bodies {totals['asm_bodies']} | directives {totals['directives']} | "
          f"include_asm {totals['include_asm']} | postpass {totals['postpass']} | dirty files {dirty_files}")


def check_baseline(scopes, baseline_path: pathlib.Path = BASELINE) -> int:
    baseline = json.loads(baseline_path.read_text())
    increases = []
    decreases = []
    for scope, counts in scopes.items():
        expected = baseline.get(scope, {})
        for key, count in counts.items():
            before = expected.get(key, 0)
            if count > before:
                increases.append((scope, key, before, count))
            elif count < before:
                decreases.append((scope, key, before, count))

    if increases:
        print("Crutch debt increased:")
        for scope, key, before, after in increases:
            print(f"  {scope}.{key}: {before} -> {after}")
        return 1

    if decreases:
        print("Crutch debt improved, but the ratchet baseline is stale:")
        for scope, key, before, after in decreases:
            print(f"  {scope}.{key}: {before} -> {after}")
        print("Run `make debt-baseline` and commit the lowered baseline.")
        return 1

    print("OK: crutch debt matches the ratchet baseline in main and overlays.")
    return 0


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--check", action="store_true",
                        help="fail if a scoped count exceeds the committed baseline")
    parser.add_argument("--json", action="store_true",
                        help="print current scoped counts as JSON")
    parser.add_argument("--write", action="store_true",
                        help="regenerate the Markdown report and badge")
    parser.add_argument("--update-baseline", action="store_true",
                        help="write current scoped counts as the new ratchet")
    args = parser.parse_args()

    per_sub, totals, dirty_files, scopes = collect_debt()
    if args.json:
        print(json.dumps(scopes, indent=2, sort_keys=True))
    if args.write:
        write_reports(per_sub, totals, dirty_files)
    if args.update_baseline:
        BASELINE.write_text(json.dumps(scopes, indent=2, sort_keys=True) + "\n")
        print(f"wrote {BASELINE}")
    if args.check:
        return check_baseline(scopes)
    if not args.json and not args.write and not args.update_baseline:
        for scope, counts in scopes.items():
            print(scope + ": " + ", ".join(f"{k}={v}" for k, v in counts.items()))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
