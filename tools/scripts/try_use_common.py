#!/usr/bin/env python3
"""Replace a file's local standard typedefs with #include "common.h", keeping
the edit only if the object stays byte-identical (rage-pc lesson: one typedef
hub, zero local typedefs). Non-standard typedefs (u8_1, size_t, ...) are left
alone; anything that fails to compile or changes bytes auto-reverts."""
import pathlib, re, sys
sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
from objverify import ROOT, compile_one, snapshot, try_edit

STD = {
    's8': r'signed\s+char', 'u8': r'unsigned\s+char',
    's16': r'(?:signed\s+)?short', 'u16': r'unsigned\s+short',
    's32': r'(?:signed\s+)?int', 'u32': r'unsigned\s+int',
    's64': r'(?:signed\s+)?long\s+long', 'u64': r'unsigned\s+long\s+long',
    'f32': r'float',
}
TYPEDEF = re.compile(r'typedef\s+(?P<base>[a-z ]+?)\s+(?P<name>\w+)\s*;')

def strip_std_typedefs(text):
    removed = 0
    def repl(m):
        nonlocal removed
        name, base = m.group('name'), m.group('base').strip()
        if name in STD and re.fullmatch(STD[name], base):
            removed += 1
            return ''
        return m.group(0)
    out = TYPEDEF.sub(repl, text)
    return out, removed

def main(argv):
    done = kept = 0
    for name in argv:
        src = (ROOT / name).resolve()
        original = src.read_text()
        stripped, n = strip_std_typedefs(original)
        if n == 0:
            continue
        # collapse blank lines left behind, prepend include
        stripped = re.sub(r'\n{3,}', '\n\n', stripped)
        candidate = '#include "common.h"\n' + stripped.lstrip('\n')
        baseline = snapshot(src)
        if baseline is None:
            print('%s: baseline failed' % name); continue
        if try_edit(src, original, candidate, baseline):
            done += 1; print('%s: consolidated (%d typedefs)' % (name, n))
        else:
            kept += 1
        baseline.unlink()
    print('%d consolidated, %d left (not byte-identical / redefinition)' % (done, kept))

if __name__ == '__main__':
    main(sys.argv[1:])
