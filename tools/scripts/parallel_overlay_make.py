#!/usr/bin/env python3
"""Run an overlay Make target concurrently with isolated diagnostic logs."""
from __future__ import annotations

import argparse
import concurrent.futures
import hashlib
import pathlib
import subprocess
import sys


ROOT = pathlib.Path(__file__).resolve().parents[2]
LOG_ROOT = ROOT / "build" / "USA"
SHARED_INPUTS = (
    ROOT / "Makefile",
    ROOT / "requirements.txt",
    ROOT / "configs" / "USA" / "sym.main.txt",
    ROOT / "include",
    ROOT / "tools" / "m2c",
    ROOT / "tools" / "scripts" / "cc.sh",
    ROOT / "tools" / "scripts" / "overlay_extra_undefineds.py",
    ROOT / "tools" / "scripts" / "parallel_overlay_make.py",
)


def overlay_fingerprint(name: str) -> str:
    """Hash the inputs that can affect one overlay's linked bytes."""
    inputs = SHARED_INPUTS + (
        ROOT / "configs" / "USA" / "overlays" / (name + ".yaml"),
        ROOT / "configs" / "USA" / "overlays" / ("sym." + name + ".txt"),
        ROOT / "src" / "overlays" / name,
    )
    files: list[pathlib.Path] = []
    for path in inputs:
        if path.is_dir():
            files.extend(item for item in path.rglob("*") if item.is_file())
        elif path.is_file():
            files.append(path)

    digest = hashlib.sha256()
    for path in sorted(set(files)):
        digest.update(str(path.relative_to(ROOT)).encode())
        digest.update(b"\0")
        digest.update(path.read_bytes())
        digest.update(b"\0")
    return digest.hexdigest()


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--jobs", type=int, default=4)
    parser.add_argument("--target", default="overlay-build")
    parser.add_argument("overlays", nargs="+")
    args = parser.parse_args()
    LOG_ROOT.mkdir(parents=True, exist_ok=True)

    def run(name):
        log = LOG_ROOT / ("%s.%s.log" % (args.target, name))
        overlay_dir = LOG_ROOT / "overlays" / name
        stamp = overlay_dir / ".input-sha256"
        fingerprint = overlay_fingerprint(name) if args.target == "overlay-build" else None
        if (fingerprint is not None
                and stamp.exists()
                and stamp.read_text().strip() == fingerprint
                and (overlay_dir / (name + ".bin")).exists()
                and (overlay_dir / (name + ".map")).exists()):
            log.write_text("cached: inputs unchanged\n")
            return name, 0, log, True
        with log.open("w") as output:
            done = subprocess.run(
                ["make", "--no-print-directory", args.target, "OVERLAY=" + name],
                cwd=ROOT, stdout=output, stderr=subprocess.STDOUT,
            )
        if done.returncode == 0 and fingerprint is not None:
            stamp.write_text(fingerprint + "\n")
        return name, done.returncode, log, False

    failed = []
    with concurrent.futures.ThreadPoolExecutor(max_workers=args.jobs) as pool:
        for name, code, log, cached in pool.map(run, args.overlays):
            if code:
                failed.append((name, log))
                print("FAIL " + name, flush=True)
            elif cached:
                print("CACHED " + name, flush=True)
            else:
                print("OK   " + name, flush=True)

    for name, log in failed:
        print("\n== %s: %s ==" % (name, log.relative_to(ROOT)))
        print("".join(log.read_text(errors="replace").splitlines(True)[-40:]), end="")
    return int(bool(failed))


if __name__ == "__main__":
    sys.exit(main())
