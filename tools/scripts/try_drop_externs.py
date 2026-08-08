#!/usr/bin/env python3
"""Remove C-file extern declarations already supplied by included headers.

Each declaration is removed in isolation and the rebuilt object's loadable
sections are compared with a baseline. A removal is retained only when the
translation unit still compiles and remains byte-identical.

Usage: try_drop_externs.py <file.c>...
"""

import pathlib
import re
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
from objverify import ROOT, compile_one, snapshot, try_edit


EXTERN = re.compile(r"^extern\s+[^;\n]+;[ \t]*\n", re.MULTILINE)


def main(argv):
    dropped = kept = 0
    for name in argv:
        src = (ROOT / name).resolve()
        baseline = snapshot(src)
        if baseline is None:
            print(f"{name}: baseline build failed, skipping")
            continue

        text = src.read_text()
        print(f"{name}: {len(EXTERN.findall(text))} extern(s)")
        index = 0
        while True:
            matches = list(EXTERN.finditer(text))
            if index >= len(matches):
                break
            match = matches[index]
            candidate = text[:match.start()] + text[match.end():]
            if try_edit(src, text, candidate, baseline):
                text = candidate
                dropped += 1
                print(f"  dropped: {match.group(0).strip()}")
            else:
                kept += 1
                index += 1

        src.write_text(text)
        compile_one(src)
        baseline.unlink()

    print(f"{dropped} dropped, {kept} still require a declaration")


if __name__ == "__main__":
    main(sys.argv[1:])
