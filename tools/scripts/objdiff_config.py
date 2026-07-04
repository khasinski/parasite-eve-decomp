#!/usr/bin/env python3
"""Fill objdiff.json units from the current build tree.

One unit per C translation unit: base = the freshly built object,
target = the byte-verified snapshot under expected/build (make expected).
`metadata.complete` mirrors the progress metric: true only when the
source is plain C (no assembly of any kind).
Categories: main executable + each configured overlay.
"""
from __future__ import annotations

import json
import pathlib
import sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
BUILD = ROOT / "build/USA"
EXPECTED = ROOT / "expected/build/USA"

sys.path.insert(0, str(ROOT / "tools/scripts"))
from progress_report import is_clean  # noqa: E402


def unit_for(o: pathlib.Path, src: pathlib.Path, category: str) -> dict:
    rel = o.relative_to(BUILD)
    return {
        "name": str(rel)[: -len(".c.o")],
        "target_path": str((EXPECTED / rel).relative_to(ROOT)),
        "base_path": str(o.relative_to(ROOT)),
        "metadata": {
            "source_path": str(src.relative_to(ROOT)),
            "progress_categories": [category],
            "complete": src.exists() and is_clean(src.read_text(errors="ignore")),
        },
    }


def main() -> None:
    cfg_path = ROOT / "objdiff.json"
    cfg = json.loads(cfg_path.read_text())

    categories = [{"id": "main", "name": "SLUS_006.62 (main)"}]
    units = []

    for o in sorted((BUILD / "src/main").rglob("*.c.o")):
        src = ROOT / str(o.relative_to(BUILD))[: -len(".o")]
        units.append(unit_for(o, src, "main"))

    for ovl_yaml in sorted((ROOT / "configs/USA/overlays").glob("ovl_*.yaml")):
        ovl = ovl_yaml.stem
        categories.append({"id": ovl, "name": ovl})
        obj_root = BUILD / "overlays" / ovl / "src"
        if not obj_root.exists():
            continue
        for o in sorted(obj_root.rglob("*.c.o")):
            src = ROOT / "src/overlays" / ovl / str(o.relative_to(obj_root))[: -len(".o")]
            units.append(unit_for(o, src, ovl))

    cfg["progress_categories"] = categories
    cfg["units"] = units
    cfg_path.write_text(json.dumps(cfg, indent=2) + "\n")
    print(f"objdiff.json: {len(units)} units, {len(categories)} categories")


if __name__ == "__main__":
    main()
