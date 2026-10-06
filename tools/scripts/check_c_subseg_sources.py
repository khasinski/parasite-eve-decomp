#!/usr/bin/env python3
"""Verify the two-way contract between splat manifests and ``src/``.

Every configured C subsegment must have a tracked source, and every C file
under ``src/main`` or ``src/overlays`` must be configured. Experiments stay
outside the tracked tree, so nothing unconfigured can be compiled, linked, or
counted as matched accidentally.

A source normally owns exactly one subsegment. The exception is a linked
overlay library (``src/overlays/room_lib/``): many overlay manifests name it
(as ``../room_lib/<unit>``) and each overlay compiles its own copy, but one
manifest may still list it only once.
"""
from __future__ import annotations

import argparse
import collections
import os
import pathlib
import subprocess

import yaml


ROOT = pathlib.Path(__file__).resolve().parents[2]
CONFIGS = [ROOT / "configs/USA/main.yaml"] + sorted(
    (ROOT / "configs/USA/overlays").glob("*.yaml")
)
SOURCE_ROOTS = (ROOT / "src/main", ROOT / "src/overlays")
# Sources that several overlay manifests may share, once per manifest.
SHARED_ROOTS = (ROOT / "src/overlays/room_lib",)


def is_shared(path: pathlib.Path) -> bool:
    return any(root in path.parents for root in SHARED_ROOTS)


def iter_subsegments(config: dict):
    for segment in config.get("segments", []):
        if isinstance(segment, dict):
            yield from segment.get("subsegments", [])


def configured_source_entries(config_paths=CONFIGS):
    """(manifest, source) for every C subsegment, with `..` resolved."""
    result = []
    for path in config_paths:
        config = yaml.safe_load(path.read_text())
        source_root = ROOT / config["options"]["src_path"]
        for subsegment in iter_subsegments(config):
            if (isinstance(subsegment, list) and len(subsegment) >= 3
                    and subsegment[1] == "c"):
                source = pathlib.Path(os.path.normpath(
                    source_root / f"{subsegment[2]}.c"))
                result.append((path, source))
    return result


def configured_sources(config_paths=CONFIGS) -> set[pathlib.Path]:
    return {source for _manifest, source in configured_source_entries(config_paths)}


def duplicate_configured_sources(config_paths=CONFIGS) -> set[pathlib.Path]:
    entries = configured_source_entries(config_paths)
    per_manifest = collections.Counter(entries)
    overall = collections.Counter(source for _manifest, source in entries)
    duplicates = {source for (_manifest, source), count in per_manifest.items()
                  if count > 1}
    duplicates |= {source for source, count in overall.items()
                   if count > 1 and not is_shared(source)}
    return duplicates


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
            print("Configure the source in a manifest, or keep the experiment out of src/.")
        if untracked:
            print(f"ERROR: {len(untracked)} source files are not tracked by git:")
            for path in sorted(untracked):
                print(f"  {path.relative_to(ROOT)}")
            print("Track active source, or keep local experiments out of src/.")
        if duplicates:
            print(f"ERROR: {len(duplicates)} C sources occur more than once in manifests:")
            for path in sorted(duplicates):
                print(f"  {path.relative_to(ROOT)}")
            print("Each active source must own exactly one configured subsegment.")
        return 1
    entries = configured_source_entries()
    shared = {source for _manifest, source in entries if is_shared(source)}
    shared_uses = sum(1 for _manifest, source in entries if is_shared(source))
    print(f"OK: manifests and src/ have a one-to-one C source mapping "
          f"({len(configured_sources())} files; {len(shared)} shared library "
          f"sources linked {shared_uses} times).")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
