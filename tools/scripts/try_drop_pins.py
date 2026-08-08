#!/usr/bin/env python3
"""Drop explicit MIPS register hints that the compiler no longer needs.

A pin forces one local into one hardware register. Most were added while a
function was being matched and stopped carrying their weight once the
surrounding code settled, but nothing tells you which - so this tries each one
in turn and keeps only the removals that leave the object file bit-identical.

Verification is per object, not per link: it rebuilds the single .o and
compares its code and data sections with a baseline taken before the edit,
which is faster than `make check` and immune to the stale-object trap.

Usage: try_drop_pins.py <file.c>...
"""

import pathlib
import re
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
from objverify import ROOT, compile_one, snapshot, try_edit

REGISTER = r'(?:\$\d+|zero|at|v[01]|a[0-3]|t[0-9]|s[0-8]|k[01]|gp|sp|fp|ra)'
PIN = re.compile(r'^(?P<indent>\s*)register\s+(?P<decl>.+?)\s+asm\("' + REGISTER + r'"\)'
                 r'(?P<tail>\s*(?:=[^;]*)?;)\s*$', re.MULTILINE)


def main(argv):
    dropped = kept = 0
    for name in argv:
        src = (ROOT / name).resolve()
        baseline = snapshot(src)
        if baseline is None:
            print('%s: baseline build failed, skipping' % name)
            continue
        original = src.read_text()

        print('%s: %d pin(s)' % (name, len(PIN.findall(original))))
        text = original
        index = 0
        while True:
            attempt = list(PIN.finditer(text))
            if index >= len(attempt):
                break
            m = attempt[index]
            replacement = '%s%s%s' % (m.group('indent'), m.group('decl'), m.group('tail'))
            candidate = text[:m.start()] + replacement + text[m.end():]
            if try_edit(src, text, candidate, baseline):
                text = candidate
                dropped += 1
                print('  dropped: %s' % m.group(0).strip())
            else:
                kept += 1
                index += 1
                print('  load-bearing: %s' % m.group(0).strip())
        src.write_text(text)
        compile_one(src)
        baseline.unlink()
    print('%d dropped, %d load-bearing' % (dropped, kept))


if __name__ == '__main__':
    main(sys.argv[1:])
