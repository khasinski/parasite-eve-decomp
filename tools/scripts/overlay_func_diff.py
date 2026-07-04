#!/usr/bin/env python3
"""Compile and normalized-diff an overlay permuter scratch candidate."""

from __future__ import annotations

import argparse
import difflib
import subprocess
from pathlib import Path

from decomp_pipeline import normalize_objdump, normalize_target
from make_overlay_permuter_scratch import (
    ROOT,
    SCRATCH_ROOT,
    DEFAULT_C_TARGET_QUEUE,
    DEFAULT_STATUS,
    read_rows,
    select_row,
)


OBJDUMP = "mipsel-none-elf-objdump"


def ensure_scratch(row: dict[str, str], scratch: Path) -> None:
    required = ["base.c", "compile.sh", "settings.toml", "target.o", "target.s", "target.meta.json"]
    if all((scratch / name).exists() for name in required):
        return
    cmd = [
        str(ROOT / "tools" / "scripts" / "make_overlay_permuter_scratch.py"),
        "--func",
        row["symbol"],
        "--out-dir",
        str(scratch),
    ]
    proc = subprocess.run(cmd, cwd=ROOT, text=True, check=False)
    if proc.returncode != 0:
        raise SystemExit(proc.returncode)


def run(cmd: list[str], *, stdout: Path | None = None) -> subprocess.CompletedProcess[str]:
    if stdout is None:
        return subprocess.run(cmd, cwd=ROOT, text=True, check=False)
    with stdout.open("w") as f:
        return subprocess.run(cmd, cwd=ROOT, text=True, stdout=f, stderr=subprocess.STDOUT, check=False)


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--status", type=Path, default=DEFAULT_STATUS)
    parser.add_argument("--c-target-queue", type=Path, default=DEFAULT_C_TARGET_QUEUE)
    parser.add_argument("--func")
    parser.add_argument("--rank")
    parser.add_argument("--c-rank")
    parser.add_argument("--src", type=Path)
    parser.add_argument("--scratch", type=Path)
    parser.add_argument("--expect-mismatch", action="store_true")
    parser.add_argument("--quiet", action="store_true")
    args = parser.parse_args()

    status_path = args.status if args.status.is_absolute() else ROOT / args.status
    c_target_path = (
        args.c_target_queue
        if args.c_target_queue.is_absolute()
        else ROOT / args.c_target_queue
    )
    c_target_rows = read_rows(c_target_path) if c_target_path.exists() else None
    row = select_row(read_rows(status_path), args.func, args.rank, args.c_rank, c_target_rows)
    scratch = args.scratch or SCRATCH_ROOT / f"scratch_{row['symbol']}"
    scratch = scratch if scratch.is_absolute() else ROOT / scratch
    ensure_scratch(row, scratch)

    src = args.src or scratch / "base.c"
    src = src if src.is_absolute() else ROOT / src
    if not src.exists():
        raise SystemExit(f"missing candidate source: {src}")

    build = scratch / "build"
    build.mkdir(parents=True, exist_ok=True)
    obj = build / "candidate.o"
    objdump = build / "candidate.objdump"
    diff_path = build / "candidate.diff"
    compile_log = build / "candidate.log"

    proc = run([str(scratch / "compile.sh"), str(src), "-o", str(obj)], stdout=compile_log)
    if proc.returncode != 0:
        raise SystemExit(f"compile failed; see {compile_log.relative_to(ROOT)}")
    proc = run([OBJDUMP, "-dr", str(obj)], stdout=objdump)
    if proc.returncode != 0:
        raise SystemExit(f"objdump failed; see {objdump.relative_to(ROOT)}")

    target_lines = normalize_target(scratch / "target.s")
    candidate_lines = normalize_objdump(objdump)
    diff = list(
        difflib.unified_diff(
            target_lines,
            candidate_lines,
            fromfile=f"{row['symbol']}.target",
            tofile=f"{row['symbol']}.candidate",
            lineterm="",
        )
    )
    diff_path.write_text("\n".join(diff) + ("\n" if diff else ""))
    if diff:
        print(f"DIFF: {diff_path.relative_to(ROOT)}")
        if not args.quiet:
            print("\n".join(diff[:120]))
        if args.expect_mismatch:
            print("EXPECTED_MISMATCH")
            return 0
        return 1

    if args.expect_mismatch:
        print(f"UNEXPECTED_MATCH: {row['symbol']} ({objdump.relative_to(ROOT)})")
        return 1
    print(f"MATCH: {row['symbol']} ({objdump.relative_to(ROOT)})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
