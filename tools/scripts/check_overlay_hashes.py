#!/usr/bin/env python3
"""Verify every already-built overlay against its manifest retail SHA-1."""
from __future__ import annotations

import hashlib
import pathlib
import sys

import yaml


ROOT = pathlib.Path(__file__).resolve().parents[2]
CONFIGS = ROOT / "configs" / "USA" / "overlays"


def sha1(path):
    return hashlib.sha1(path.read_bytes()).hexdigest()


def main() -> int:
    failed = []
    checked = 0
    for path in sorted(CONFIGS.glob("*.yaml")):
        config = yaml.safe_load(path.read_text())
        options = config["options"]
        built = ROOT / options["build_path"] / (options["basename"] + ".bin")
        expected = config.get("sha1")
        actual = sha1(built) if built.exists() else None
        checked += 1
        if not expected or actual != expected:
            failed.append((path.stem, expected, actual))
    if failed:
        for name, expected, actual in failed:
            print("%s: expected %s, actual %s" %
                  (name, expected or "<no manifest SHA>", actual or "<missing>"))
        return 1
    print("OK: all %d overlay binaries match their retail SHA-1." % checked)
    return 0


if __name__ == "__main__":
    sys.exit(main())
