#!/usr/bin/env python3
"""Report C symbols that have multiple declaration shapes.

Matching PSY-Q code often depends on declaration spelling: array vs pointer,
extern vs tentative definition, and volatile can change gp-relative selection
or register allocation. This report makes those variants visible before a
cleanup accidentally "fixes" them.
"""

from __future__ import annotations

import argparse
import re
from collections import defaultdict
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
SRC_DIRS = [ROOT / "src/main/main", ROOT / "src/main/akao", ROOT / "include/pe1"]

DECL_RE = re.compile(
    r"^(?P<prefix>(?:extern\s+)?(?:volatile\s+)?(?:const\s+)?[A-Za-z_][A-Za-z0-9_ \t*]*?)"
    r"\s+(?P<name>[A-Za-z_][A-Za-z0-9_]*)"
    r"(?P<suffix>(?:\[[^\]]*\])*)\s*;$"
)
ASM_NAME_RE = re.compile(r"\s+asm\s*\(")

INTENTIONAL_VARIANTS = {
    "g_PlayerEntity": "field actor current pointer; absolute/gp-relative selection is codegen-sensitive",
    "g_ActiveActor": "field actor secondary pointer; absolute/gp-relative selection is codegen-sensitive",
    "g_CurrentEntity": "field actor list/current sentinel; declaration shape affects compare code",
    "g_GameState": "menu/window global used with mixed scalar/array shapes",
    "g_GameStateFlags": "small-data symbol with known gp-relative pressure",
    "g_ActiveDrawSlot": "small-data symbol with known gp-relative pressure",
    "g_AyaEquippedWeaponSlot": "halfword mirror symbol; store width/order is codegen-sensitive",
    "g_AyaEquippedArmorSlot": "halfword mirror symbol; store width/order is codegen-sensitive",
}


def iter_lines(paths: list[Path]):
    for base in paths:
        suffix = "*.c" if "src/main" in str(base) else "*.h"
        for path in sorted(base.rglob(suffix)):
            rel = path.relative_to(ROOT)
            for lineno, raw in enumerate(path.read_text(errors="replace").splitlines(), 1):
                stripped = raw.strip()
                if not stripped or stripped.startswith(("#", "//", "/*", "*")):
                    continue
                yield rel, lineno, stripped


def declaration_name(line: str) -> str | None:
    if line.startswith(("return ", "if ", "for ", "while ", "switch ", "case ")):
        return None
    if "(" in line and "(*" not in line:
        return None
    if ASM_NAME_RE.search(line):
        line = ASM_NAME_RE.split(line)[0] + ";"
    match = DECL_RE.match(line)
    if not match:
        return None
    name = match.group("name")
    if name in {"if", "for", "while", "return", "switch", "case"}:
        return None
    return name


def collect(paths: list[Path]) -> dict[str, dict[str, list[str]]]:
    symbols: dict[str, dict[str, list[str]]] = defaultdict(lambda: defaultdict(list))
    for rel, lineno, line in iter_lines(paths):
        name = declaration_name(line)
        if name is None:
            continue
        symbols[name][line].append(f"{rel}:{lineno}")
    return symbols


def emit(symbols: dict[str, dict[str, list[str]]], show_all: bool) -> None:
    print("symbol\tvariant_count\tintentional\tdeclaration\tlocations")
    for name in sorted(symbols):
        variants = symbols[name]
        if not show_all and len(variants) <= 1:
            continue
        note = INTENTIONAL_VARIANTS.get(name, "")
        for decl in sorted(variants):
            locations = ",".join(variants[decl])
            print(f"{name}\t{len(variants)}\t{note}\t{decl}\t{locations}")


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--all", action="store_true", help="include single-declaration symbols")
    parser.add_argument("paths", nargs="*", type=Path, help="override scan paths")
    args = parser.parse_args()
    paths = [p if p.is_absolute() else ROOT / p for p in args.paths] if args.paths else SRC_DIRS
    emit(collect(paths), args.all)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
