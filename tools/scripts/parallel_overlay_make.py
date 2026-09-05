#!/usr/bin/env python3
"""Run an overlay Make target concurrently with isolated diagnostic logs."""
from __future__ import annotations

import argparse
import concurrent.futures
import pathlib
import subprocess
import sys


ROOT = pathlib.Path(__file__).resolve().parents[2]
LOG_ROOT = ROOT / "build" / "USA"


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--jobs", type=int, default=4)
    parser.add_argument("--target", default="overlay-build")
    parser.add_argument("overlays", nargs="+")
    args = parser.parse_args()
    LOG_ROOT.mkdir(parents=True, exist_ok=True)

    def run(name):
        log = LOG_ROOT / ("%s.%s.log" % (args.target, name))
        with log.open("w") as output:
            done = subprocess.run(
                ["make", "--no-print-directory", args.target, "OVERLAY=" + name],
                cwd=ROOT, stdout=output, stderr=subprocess.STDOUT,
            )
        return name, done.returncode, log

    failed = []
    with concurrent.futures.ThreadPoolExecutor(max_workers=args.jobs) as pool:
        for name, code, log in pool.map(run, args.overlays):
            if code:
                failed.append((name, log))
                print("FAIL " + name, flush=True)
            else:
                print("OK   " + name, flush=True)

    for name, log in failed:
        print("\n== %s: %s ==" % (name, log.relative_to(ROOT)))
        print("".join(log.read_text(errors="replace").splitlines(True)[-40:]), end="")
    return int(bool(failed))


if __name__ == "__main__":
    sys.exit(main())
