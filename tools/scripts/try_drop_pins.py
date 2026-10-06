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
# One line can pin several locals. The line pattern above binds only the asm
# immediately before the semicolon, so the earlier clauses have to be listed
# on their own or they are never compared.
ASM_CLAUSE = re.compile(r'\s*\basm\s*\("' + REGISTER + r'"\)')


def clauses(text):
    """Byte spans of every register pin on a line the pin matcher accepts."""
    spans = []
    for pin in PIN.finditer(text):
        for clause in ASM_CLAUSE.finditer(text, pin.start(), pin.end()):
            spans.append((clause.start(), clause.end()))
    return spans


def main(argv):
    dropped = kept = 0
    for name in argv:
        src = (ROOT / name).resolve()
        baseline = snapshot(src)
        if baseline is None:
            print('%s: baseline build failed, skipping' % name)
            continue
        original = src.read_text()

        print('%s: %d pin(s)' % (name, len(clauses(original))))
        text = original
        while True:
            spans = clauses(text)
            removed = False
            for start, end in spans:
                snippet = text[start:end].strip()
                line = text[:start].count('\n') + 1
                candidate = text[:start] + text[end:]
                if try_edit(src, text, candidate, baseline):
                    text = candidate
                    dropped += 1
                    removed = True
                    print('  dropped: %s:%d %s' % (name, line, snippet))
                    break
                kept += 1
                print('  load-bearing: %s:%d %s' % (name, line, snippet))
            if not removed:
                break
        src.write_text(text)
        compile_one(src)
        baseline.unlink()
    print('%d dropped, %d load-bearing' % (dropped, kept))


if __name__ == '__main__':
    main(sys.argv[1:])
