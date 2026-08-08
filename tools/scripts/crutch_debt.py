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

import json
import pathlib
import re

ROOT = pathlib.Path(__file__).resolve().parents[2]
SRC = ROOT / "src"


def strip_comments(text: str) -> str:
    return re.sub(r"//.*", "", re.sub(r"/\*.*?\*/", "", text, flags=re.S))


# GTE / hardware coprocessor ops that are legitimately expressed as asm and are
# not "dirty" crutches to remove (kept out of the ASM_BODY count).
_HW_OPS = "cfc2|ctc2|lwc2|swc2|mfc2|mtc2|nop"

PATTERNS = {
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
}

ORDER = ["pins", "barriers", "aliases", "asm_bodies", "directives", "gotos", "include_asm", "postpass"]
HEAVY = ["pins", "barriers", "aliases", "asm_bodies", "directives"]  # count toward the headline "dirty"


def subsystem_of(rel: pathlib.PurePath) -> str:
    parts = rel.parts
    if parts[:2] == ("src", "overlays"):
        return "overlays"
    if parts[:2] == ("src", "main") and len(parts) >= 3:
        return f"main/{parts[2]}"
    return parts[1] if len(parts) > 1 else "src"


def main() -> None:
    from collections import defaultdict

    per_sub = defaultdict(lambda: {k: 0 for k in ORDER} | {"dirty_files": 0})
    totals = {k: 0 for k in ORDER}
    dirty_files = 0

    for path in sorted(SRC.rglob("*.c")):
        rel = path.relative_to(ROOT)
        text = strip_comments(path.read_text(errors="ignore"))
        sub = subsystem_of(pathlib.PurePath(rel))
        counts = {k: len(PATTERNS[k].findall(text)) for k in ORDER}
        for k, v in counts.items():
            per_sub[sub][k] += v
            totals[k] += v
        if any(counts[k] for k in HEAVY):
            per_sub[sub]["dirty_files"] += 1
            dirty_files += 1

    # ----- CRUTCH_DEBT.md -----
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
        "**directives** = `asm(\".word ...\")` · **gotos** · **include_asm** · **postpass**",
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
    (ROOT / "docs" / "CRUTCH_DEBT.md").write_text("\n".join(lines) + "\n")

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


if __name__ == "__main__":
    main()
