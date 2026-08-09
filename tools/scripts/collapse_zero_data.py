#!/usr/bin/env python3
"""Collapse runs of `.word/.short/.byte 0` in generated data asm into a single
`.space N`. Byte-identical (GNU as emits the same zero bytes), but shrinks the
huge zero-blob data files (dtail_0D0860 is 15 MB, 99.5% zeros) so the build
stops assembling ~279k `.word 0` lines. Runs after splat, before assembly.

Only collapses lines that are pure zero data with nothing but an address
comment; stops at labels, directives, and non-zero data so symbol boundaries
are preserved. Usage: collapse_zero_data.py <file.s>..."""
import re, sys

ZERO = re.compile(r'^\s*(?:/\*[^*]*\*/\s*)?\.(word|short|byte)\s+0x?0+\s*$')
SIZE = {'word': 4, 'short': 2, 'byte': 1}


def collapse(text: str) -> str:
    out, run = [], 0
    def flush():
        nonlocal run
        if run:
            out.append(f"    .space 0x{run:X}")
            run = 0
    for line in text.splitlines():
        m = ZERO.match(line)
        if m:
            run += SIZE[m.group(1)]
        else:
            flush()
            out.append(line)
    flush()
    return "\n".join(out) + "\n"


def main(argv):
    for path in argv:
        try:
            text = open(path).read()
        except OSError:
            continue
        new = collapse(text)
        if new != text:
            open(path, "w").write(new)


if __name__ == "__main__":
    main(sys.argv[1:])
