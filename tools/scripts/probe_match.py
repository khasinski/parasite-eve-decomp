#!/usr/bin/env python3
"""probe_match.py <name>: compile src/main/<name>.c (honoring the GCC_VERSION
marker) and compare its .text to the retail bytes at the function's slot,
reloc-masked (jal / %hi / %lo). Works whether the config subsegment is `c` or
`asm`, so it can drive matching a function that is currently flipped to asm.
Prints reloc-masked word diffs and MATCH / NOMATCH (exit 0 on MATCH)."""
import re, subprocess, struct, sys, pathlib
ROOT = pathlib.Path('.')
REF = open('assets/USA/main.exe', 'rb').read()
CFG = open('configs/USA/main.yaml').read()
BOUND = sorted(int(m.group(1), 16) for m in re.finditer(r'-\s*\[\s*(0x[0-9A-Fa-f]+)', CFG))
name = sys.argv[1]
m = re.search(r'0x([0-9A-Fa-f]+), (?:c|asm), ' + re.escape(name) + r'\]', CFG)
if not m:
    print("no subsegment for " + name)
    sys.exit(2)
off = int(m.group(1), 16); nxt = min(a for a in BOUND if a > off); size = nxt - off
src = ROOT / f'src/main/{name}.c'
if not src.exists():
    print("no source " + str(src)); sys.exit(2)
obj = ROOT / f'build/USA/probe_{name.replace("/", "_")}.o'
obj.parent.mkdir(parents=True, exist_ok=True)
if obj.exists(): obj.unlink()
subprocess.run(['bash', 'tools/scripts/cc.sh', str(src), str(obj)], capture_output=True)
if not obj.exists():
    print("COMPILE FAIL"); sys.exit(1)
binp = obj.with_suffix('.pmb')
subprocess.run(['mipsel-none-elf-objcopy', '-O', 'binary', '--only-section', '.text', str(obj), str(binp)], capture_output=True)
data = binp.read_bytes(); binp.unlink()
relocs = {}
for l in subprocess.run(['mipsel-none-elf-objdump', '-r', '--section=.text', str(obj)], capture_output=True, text=True).stdout.splitlines():
    mm = re.match(r'^([0-9a-f]{8})\s+(\S+)', l)
    if mm: relocs[int(mm.group(1), 16)] = mm.group(2)
dis = {m2.group(1): m2.group(2).strip() for m2 in re.finditer(r'^\s+([0-9a-f]+):\s+[0-9a-f]{8}\s+(.+)$', subprocess.run(['mipsel-none-elf-objdump', '-d', str(obj)], capture_output=True, text=True).stdout, re.M)}
obj.unlink()
if len(data) != size:
    print(f"SIZE MISMATCH built 0x{len(data):x} != retail 0x{size:x}")
diffs = 0
for i in range(0, min(len(data), size), 4):
    r = struct.unpack('<I', REF[off + i:off + i + 4])[0]; b = struct.unpack('<I', data[i:i + 4])[0]
    if i in relocs:
        # Relocated word: the linker patches only the immediate/target field, so
        # compare just the untouched high bits. For %hi/%lo (I-type) that is the
        # top halfword (opcode + rs/rt) -- which still catches a register-alloc
        # difference the old whole-word mask missed. For R_MIPS_26 (jal) the
        # whole target is in the low 26 bits, so compare only the opcode.
        t = relocs[i]
        shift = 26 if t == 'R_MIPS_26' else 16 if t in ('R_MIPS_HI16', 'R_MIPS_LO16') else None
        if shift is not None and (r >> shift) != (b >> shift):
            print(f"  +{i:03x} retail {r:08x}  built {b:08x}  [reloc {t}: {dis.get(f'{i:x}', '?')}]"); diffs += 1
        continue
    if r != b:
        print(f"  +{i:03x} retail {r:08x}  built {b:08x}  [built: {dis.get(f'{i:x}', '?')}]"); diffs += 1
print("MATCH" if diffs == 0 and len(data) == size else f"NOMATCH ({diffs} word diffs)")
sys.exit(0 if diffs == 0 and len(data) == size else 1)
