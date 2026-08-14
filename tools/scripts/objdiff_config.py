#!/usr/bin/env python3
"""Write objdiff.json: one unit per object a linker actually consumes.

Every object goes in - decomp.dev requires a report to account for every
function in the game so that projects on the site are measured the same way.
Sony's library code and the game's own code are separated with progress
categories rather than by dropping anything, and the overlays are reported
alongside the main executable because two thirds of the game's code lives in
them.

The unit list comes from the linker maps rather than from a glob over src/,
because the two disagree in both directions: the maps also carry the
assembled data blobs, and a stray .c no manifest references would be counted
as work that does not exist.

A unit that exists only as splat-generated assembly has no source of its own,
so it gets no base object: its functions count toward the total and match
nothing, which is the truth. Giving it the tree's own assembly as a base
would score original bytes against original bytes and report decompilation
that never happened.

Paths under expected/build/USA mirror build/USA exactly, so each unit's path
appears once with two prefixes.
"""

import argparse
import hashlib
import json
import pathlib
import re

import yaml

ROOT = pathlib.Path(__file__).resolve().parents[2]
VERSION = "USA"

SCHEMA = "https://raw.githubusercontent.com/encounter/objdiff/main/config.schema.json"

MAP_PLACEMENT = re.compile(
    r"^\s(\.\w+)\s+0x[0-9a-f]+\s+0x([0-9a-f]+)\s+(build/\S+\.o)$", re.MULTILINE
)

CATEGORIES = [
    {"id": "main-game", "name": "Main executable: game code"},
    {"id": "main-psyq", "name": "Main executable: PsyQ libraries"},
    {"id": "rooms", "name": "Room overlays"},
    {"id": "scenes", "name": "Battle scene overlays"},
    {"id": "system", "name": "System overlays"},
]


def linked_objects(map_text):
    """Objects the map places, in link order, with whether they carry code."""
    seen = {}
    for section, size, obj in MAP_PLACEMENT.findall(map_text):
        entry = seen.setdefault(obj, {"code": False})
        if section == ".text" and int(size, 16) > 0:
            entry["code"] = True
    return seen


def overlay_category(name):
    if name.startswith("room_"):
        return "rooms"
    if name.startswith("scene_"):
        return "scenes"
    return "system"


def main_category(relative):
    """Which half of the main executable an object belongs to.

    Sony's libraries live under psyq/ and everything this project wrote lives
    elsewhere. Both are reported; the category only decides which subtotal
    the unit lands in.
    """
    return "main-psyq" if "/psyq/" in relative else "main-game"


def sha1(path):
    return hashlib.sha1(path.read_bytes()).hexdigest()


def module_verified(config):
    """Whether this module's build currently reproduces the retail binary.

    `complete` is a claim, so it is only made when the bytes back it up: the
    module's built binary hashes to the value the manifest records for the
    retail original.
    """
    want = config.get("sha1")
    options = config["options"]
    if "overlays/" in options["build_path"]:
        built = ROOT / options["build_path"] / ("%s.bin" % options["basename"])
    else:
        built = ROOT / options["build_path"] / ("%s.exe" % options["basename"])
    return bool(want) and built.exists() and sha1(built) == want


def unit(relative, build_prefix, name, category, source, complete):
    entry = {
        "name": name,
        "target_path": "expected/%s%s" % (build_prefix, relative),
        "metadata": {"progress_categories": [category]},
    }
    if source is not None:
        # Only decompiled units get a base: the base is what `make build`
        # compiled from that source.
        entry["base_path"] = "%s%s" % (build_prefix, relative)
        entry["metadata"]["source_path"] = source
        if complete is not None:
            entry["metadata"]["complete"] = complete
    return entry


def module_units(name, config_path, skip):
    config = yaml.safe_load(config_path.read_text())
    if name == "main":
        map_path = ROOT / "build" / VERSION / "main.map"
        build_prefix = "build/%s/" % VERSION
        src_lead = "src/main/"
        asm_lead = "asm/%s/main/" % VERSION
        site_lead = "main/"
        category = main_category
    else:
        map_path = ROOT / "build" / VERSION / "overlays" / name / ("%s.map" % name)
        build_prefix = "build/%s/overlays/%s/" % (VERSION, name)
        src_lead = "src/overlays/%s/" % name
        asm_lead = "asm/%s/overlays/%s/" % (VERSION, name)
        site_lead = "overlays/%s/" % name
        fixed = overlay_category(name)
        category = lambda _relative: fixed  # noqa: E731
    if not map_path.exists():
        return None

    complete = True if module_verified(config) else None
    units = []
    skipped = []
    for obj, info in linked_objects(map_path.read_text()).items():
        if not obj.startswith(build_prefix):
            continue
        relative = obj[len(build_prefix):]
        if relative.startswith("src/"):
            site = site_lead + relative[len(src_lead):].removesuffix(".c.o")
            source = relative.removesuffix(".o")
            entry = unit(relative, build_prefix, site, category(relative), source,
                         complete)
        elif info["code"]:
            # Original code still spelled as assembly: report it, match nothing.
            site = site_lead + relative[len(asm_lead):].removesuffix(".s.o")
            entry = unit(relative, build_prefix, site, category(relative), None,
                         None)
        else:
            # A data blob: both sides assemble the same bytes from the same
            # file, and the data measures should say whether they really do.
            site = site_lead + relative[len(asm_lead):].removesuffix(".s.o")
            entry = unit(relative, build_prefix, site, category(relative), None,
                         None)
            entry["base_path"] = "%s%s" % (build_prefix, relative)
        if entry["name"] in skip:
            skipped.append(entry["name"])
            continue
        units.append(entry)
    return units, skipped


def config(units):
    return {
        "$schema": SCHEMA,
        "min_version": "2.0.0",
        "custom_make": "make",
        "custom_args": ["-j"],
        "build_target": False,
        "build_base": True,
        "watch_patterns": [
            "src/**/*.c",
            "src/**/*.h",
            "include/**/*.h",
            "asm/**/*.s",
            "configs/**/*.yaml",
            "configs/**/*.txt",
            "Makefile",
        ],
        "progress_categories": CATEGORIES,
        "units": units,
    }


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output", default="objdiff.json")
    # objdiff refuses to score a whole unit when it cannot pair one symbol,
    # and the failure names no unit, so having somewhere to put one beats
    # rediscovering that under pressure.
    parser.add_argument("--skip", action="append", default=[],
                        help="unit objdiff cannot score; repeatable")
    args = parser.parse_args(argv)

    skip = set(args.skip)
    units = []
    all_skipped = []
    missing = []
    names = ["main"] + sorted(
        cfg.stem for cfg in (ROOT / "configs" / VERSION / "overlays").glob("*.yaml")
    )
    for name in names:
        config_path = (
            ROOT / "configs" / VERSION / "main.yaml"
            if name == "main"
            else ROOT / "configs" / VERSION / "overlays" / ("%s.yaml" % name)
        )
        result = module_units(name, config_path, skip)
        if result is None:
            missing.append(name)
            continue
        module, skipped = result
        units.extend(module)
        all_skipped.extend(skipped)

    (ROOT / args.output).write_text(json.dumps(config(units), indent=2) + "\n")

    counts = {}
    for entry in units:
        for cat in entry["metadata"]["progress_categories"]:
            counts[cat] = counts.get(cat, 0) + 1
    print(
        "%s: %d units (%s)"
        % (
            args.output,
            len(units),
            ", ".join("%s %d" % kv for kv in sorted(counts.items())),
        )
    )
    # A report that quietly drops modules or units reads as though it covered
    # everything, so every omission is said out loud.
    if missing:
        print("  not built, absent from the report: %s" % ", ".join(missing))
    if all_skipped:
        print("  skipped, objdiff cannot pair their symbols: %s"
              % ", ".join(all_skipped))
    return 1 if missing else 0


if __name__ == "__main__":
    import sys

    sys.exit(main())
