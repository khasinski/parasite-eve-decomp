#!/usr/bin/env python3
"""Build a C context file for m2c by preprocessing a target source file.

Usage: m2ctx.py path/to/file.c [-o ctx.c]

m2c needs a single preprocessed C "context" file so it can resolve types,
globals, and function signatures while decompiling MIPS asm. We preprocess
with the PSYQ GCC 2.7.2 cpp to keep behavior consistent with the actual
toolchain.
"""
from __future__ import annotations
import argparse
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CPP = ROOT / "tools" / "psyq-gcc-2.7.2" / "cpp"
PSYQ_INC = ROOT / "sdk" / "psyq-4.0" / "PSX" / "INCLUDE"
PROJ_INC = ROOT / "include"


def build_ctx(src: Path) -> str:
    cmd = [
        str(CPP),
        "-undef",
        "-D__GNUC__=2",
        "-D__OPTIMIZE__",
        "-Dmips",
        "-D__mips__",
        "-D__LITTLE_ENDIAN__",
        f"-I{PSYQ_INC}",
        f"-I{PROJ_INC}",
        str(src),
    ]
    return subprocess.check_output(cmd, text=True)


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("src", type=Path)
    ap.add_argument("-o", "--out", type=Path, default=Path("ctx.c"))
    args = ap.parse_args()

    if not args.src.exists():
        sys.exit(f"Source file not found: {args.src}")

    args.out.write_text(build_ctx(args.src))
    print(f"Wrote context: {args.out} ({args.out.stat().st_size} bytes)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
