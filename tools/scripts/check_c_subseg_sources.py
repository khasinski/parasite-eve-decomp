#!/usr/bin/env python3
"""Verify the two-way contract between splat manifests and ``src/``.

Every configured C subsegment must have a tracked source, and every C file
under ``src/main`` or ``src/overlays`` must be configured. Non-matching or
experimental C belongs under ``candidates/`` so it cannot be compiled, linked,
or counted as matched accidentally.
"""
from __future__ import annotations

import argparse
import collections
import pathlib
import subprocess

import yaml


ROOT = pathlib.Path(__file__).resolve().parents[2]
CONFIGS = [ROOT / "configs/USA/main.yaml"] + sorted(
    (ROOT / "configs/USA/overlays").glob("*.yaml")
)
SOURCE_ROOTS = (ROOT / "src/main", ROOT / "src/overlays")


def iter_subsegments(config: dict):
    for segment in config.get("segments", []):
        if isinstance(segment, dict):
            yield from segment.get("subsegments", [])


def configured_source_entries(config_paths=CONFIGS) -> list[pathlib.Path]:
    result = []
    for path in config_paths:
        config = yaml.safe_load(path.read_text())
        source_root = ROOT / config["options"]["src_path"]
        for subsegment in iter_subsegments(config):
            if (isinstance(subsegment, list) and len(subsegment) >= 3
                    and subsegment[1] == "c"):
                result.append(source_root / f"{subsegment[2]}.c")
    return result


def configured_sources(config_paths=CONFIGS) -> set[pathlib.Path]:
    return set(configured_source_entries(config_paths))


def duplicate_configured_sources(config_paths=CONFIGS) -> set[pathlib.Path]:
    counts = collections.Counter(configured_source_entries(config_paths))
    return {path for path, count in counts.items() if count > 1}


def tracked_sources() -> set[pathlib.Path]:
    output = subprocess.check_output(
        ["git", "ls-files", "src/main/*.c", "src/main/**/*.c",
         "src/overlays/*.c", "src/overlays/**/*.c"],
        cwd=ROOT,
        text=True,
    )
    return {ROOT / line for line in output.splitlines()}


def filesystem_sources(source_roots=SOURCE_ROOTS) -> set[pathlib.Path]:
    return {
        path
        for source_root in source_roots
        for path in source_root.rglob("*.c")
    }


def source_contract(config_paths=CONFIGS, source_roots=SOURCE_ROOTS,
                    tracked=None):
    configured = configured_sources(config_paths)
    present = filesystem_sources(source_roots)
    tracked = tracked_sources() if tracked is None else tracked
    return configured - present, present - configured, present - tracked


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--list-extra", action="store_true",
                        help="print unconfigured tracked C paths, one per line")
    args = parser.parse_args()
    missing, extra, untracked = source_contract()
    duplicates = duplicate_configured_sources()
    if args.list_extra:
        for path in sorted(extra):
            print(path.relative_to(ROOT))
        return 0
    if missing or extra or untracked or duplicates:
        if missing:
            print(f"ERROR: {len(missing)} configured C subsegments lack tracked source:")
            for path in sorted(missing):
                print(f"  {path.relative_to(ROOT)}")
        if extra:
            print(f"ERROR: {len(extra)} tracked C files are not configured:")
            for path in sorted(extra):
                print(f"  {path.relative_to(ROOT)}")
            print("Move matching source into a manifest, or park WIP under candidates/.")
        if untracked:
            print(f"ERROR: {len(untracked)} source files are not tracked by git:")
            for path in sorted(untracked):
                print(f"  {path.relative_to(ROOT)}")
            print("Track active source, or move local experiments under candidates/.")
        if duplicates:
            print(f"ERROR: {len(duplicates)} C sources occur more than once in manifests:")
            for path in sorted(duplicates):
                print(f"  {path.relative_to(ROOT)}")
            print("Each active source must own exactly one configured subsegment.")
        return 1
    print(f"OK: manifests and src/ have a one-to-one C source mapping ({len(configured_sources())} files).")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
