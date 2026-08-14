#!/usr/bin/env python3
"""Normalize splat-generated data asm. Byte-identical output, fewer lines.

Two passes over *.data.s / *.rodata.s, run after splat and before assembly:

- Collapse runs of `.word/.short/.byte 0` into a single `.space N`. GNU as
  emits the same zero bytes, but the huge zero-blob data files (dtail_0D0860
  is 15 MB, 99.5% zeros) stop costing ~279k assembled lines.
  Only collapses lines that are pure zero data with nothing but an address
  comment; stops at labels, directives, and non-zero data so symbol
  boundaries are preserved.

- Drop the `nonmatching` alias lines. The macro declares a second symbol at
  the same address for asm-differ's benefit, but asm-differ only ever diffs
  functions, so on data the alias does nothing - except confuse objdiff,
  which cannot pair a section whose symbols come in address-sharing pairs and
  silently declines to score it.

Usage: collapse_zero_data.py <file.s>..."""
import re, sys

ZERO = re.compile(r'^\s*(?:/\*[^*]*\*/\s*)?\.(word|short|byte)\s+0x?0+\s*$')
SIZE = {'word': 4, 'short': 2, 'byte': 1}
DIFFER_ALIAS = re.compile(r'^nonmatching\s.*$\n?', re.MULTILINE)


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
        new = DIFFER_ALIAS.sub("", collapse(text))
        if new != text:
            open(path, "w").write(new)


if __name__ == "__main__":
    main(sys.argv[1:])
