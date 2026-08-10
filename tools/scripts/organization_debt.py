#!/usr/bin/env python3
"""Reject growth in opaque source filenames.

Address and placeholder filenames are valid migration state, but new work must
not make that state larger. Counts are scoped so mass-generated overlays cannot
hide regressions in the resident executable.
"""
from __future__ import annotations

import argparse
import json
import pathlib
import re


ROOT = pathlib.Path(__file__).resolve().parents[2]
SRC = ROOT / "src"
BASELINE = ROOT / "tools" / "organization_debt_baseline.json"

ADDRESS_NAME = re.compile(r"func_[0-9A-Fa-f]{6,}$")
PLACEHOLDER_NAME = re.compile(
    r"(?:misc|main|task|entity|seq|spu|akao|menu|engine|gpu|psyq)\d*$",
    re.IGNORECASE,
)


def collect(source_root: pathlib.Path = SRC) -> dict[str, dict[str, int]]:
    counts = {
        "main": {"address_files": 0, "placeholder_files": 0},
        "overlays": {"address_files": 0, "placeholder_files": 0},
    }
    for path in source_root.rglob("*.c"):
        rel = path.relative_to(source_root)
        scope = "overlays" if rel.parts and rel.parts[0] == "overlays" else "main"
        if ADDRESS_NAME.fullmatch(path.stem):
            counts[scope]["address_files"] += 1
        elif PLACEHOLDER_NAME.fullmatch(path.stem):
            counts[scope]["placeholder_files"] += 1
    return counts


def check(counts, baseline_path: pathlib.Path = BASELINE) -> int:
    baseline = json.loads(baseline_path.read_text())
    increases = []
    decreases = []
    for scope, metrics in counts.items():
        for metric, value in metrics.items():
            before = baseline.get(scope, {}).get(metric, 0)
            if value > before:
                increases.append((scope, metric, before, value))
            elif value < before:
                decreases.append((scope, metric, before, value))
    if increases:
        print("Organization debt increased:")
        for scope, metric, before, after in increases:
            print(f"  {scope}.{metric}: {before} -> {after}")
        return 1
    if decreases:
        print("Organization debt improved, but the ratchet baseline is stale:")
        for scope, metric, before, after in decreases:
            print(f"  {scope}.{metric}: {before} -> {after}")
        print("Run `make organization-baseline` and commit the lowered baseline.")
        return 1
    print("OK: opaque source filename counts match the ratchet baseline.")
    return 0


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--check", action="store_true")
    parser.add_argument("--json", action="store_true")
    parser.add_argument("--update-baseline", action="store_true")
    args = parser.parse_args()
    counts = collect()
    if args.update_baseline:
        BASELINE.write_text(json.dumps(counts, indent=2, sort_keys=True) + "\n")
        print(f"wrote {BASELINE}")
        return 0
    if args.json or not args.check:
        print(json.dumps(counts, indent=2, sort_keys=True))
    return check(counts) if args.check else 0


if __name__ == "__main__":
    raise SystemExit(main())
