#!/usr/bin/env python3
"""Remove #include lines a translation unit does not need.

Tries each include in turn and keeps the removal only when the file still
compiles to the same code and data.

CAUTION: "still compiles" is not "still correct to read". Most headers here
include each other, so this will happily delete game/render.h from a file that
calls DrawSprite, on the grounds that game/menu.h pulls render.h in anyway.
That builds and matches, and leaves the file naming things it no longer
includes. Decide which includes are candidates by checking whether the file
uses any name the header declares, and use this only to confirm that removing
those particular ones does not move the code.

Usage: try_drop_includes.py [path...]   (default: src)
"""

import pathlib
import re
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
from objverify import ROOT, compile_one, snapshot, try_edit

INCLUDE = re.compile(r'^#include\s+["<][^">]+[">]\s*\n', re.M)


def main(argv):
    roots = [ROOT / a for a in argv] if argv else [ROOT / 'src']
    sources = []
    for root in roots:
        sources.extend(sorted(root.rglob('*.c')) if root.is_dir() else [root])

    dropped = kept = 0
    for src in sources:
        if not INCLUDE.search(src.read_text()):
            continue
        baseline = snapshot(src)
        if baseline is None:
            print('%s: baseline build failed, skipping' % src)
            continue
        index = 0
        while True:
            text = src.read_text()
            lines = INCLUDE.findall(text)
            if index >= len(lines):
                break
            line = lines[index]
            candidate = text.replace(line, '', 1)
            if try_edit(src, text, candidate, baseline):
                dropped += 1
                print('  %s: %s' % (src.name, line.strip()))
            else:
                kept += 1
                index += 1
        compile_one(src)
        baseline.unlink()
    print('%d includes dropped, %d needed' % (dropped, kept))


if __name__ == '__main__':
    main(sys.argv[1:])
