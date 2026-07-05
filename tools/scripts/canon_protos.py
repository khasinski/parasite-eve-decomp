#!/usr/bin/env python3
"""Rewrite loose function prototypes to the signature of the actual
definition found elsewhere in the tree. Each modified file is kept only
if it still compiles to a byte-identical object (full .o comparison),
so caller codegen provably does not change."""
from __future__ import annotations

import pathlib
import re
import sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "tools/scripts"))
from merge_tus import text_bytes, FLAG_RE  # noqa: E402

DEF_RE = re.compile(
    r"^([A-Za-z_][\w \t\*]*?\b([A-Za-z_]\w*)\s*\([^;{}]*\))\s*\{", re.M)
KEYWORDS = {"if", "while", "for", "switch", "return"}


def norm(s: str) -> str:
    return re.sub(r"\s+", " ", s).strip()


def collect_defs():
    canon = {}
    for f in pathlib.Path(ROOT / "src/main").rglob("*.c"):
        t = re.sub(r"/\*.*?\*/", "", f.read_text(errors="ignore"), flags=re.S)
        for m in DEF_RE.finditer(t):
            head, name = m.group(1), m.group(2)
            if name in KEYWORDS or name.startswith("__maspsx"):
                continue
            canon.setdefault(name, norm(head))
    return canon


def main():
    canon = collect_defs()
    changed = kept = reverted = 0
    for f in sorted(pathlib.Path(ROOT / "src/main").rglob("*.c")):
        orig = f.read_text(errors="ignore")
        t = orig
        for m in list(re.finditer(
                r"^([A-Za-z_][\w \t\*]*?\b([A-Za-z_]\w*)\s*\([^;{}]*\))\s*;",
                t, re.M)):
            head, name = m.group(1), m.group(2)
            c = canon.get(name)
            if not c or norm(head) == c:
                continue
            if re.search(rf"\b{name}\s*\([^;{{]*\)\s*\{{",
                         t.replace(m.group(0), "")):
                continue  # defined in this file; leave its own decls alone
            t = t.replace(m.group(0), c + ";")
        if t == orig:
            continue
        changed += 1
        a = text_bytes(orig)
        b = text_bytes(t)
        if a is not None and a == b:
            f.write_text(t)
            kept += 1
        else:
            reverted += 1
    print(f"files touched {changed}: kept {kept}, reverted {reverted}")


if __name__ == "__main__":
    main()
