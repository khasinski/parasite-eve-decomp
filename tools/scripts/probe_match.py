#!/usr/bin/env python3
"""Compile a main C unit and compare it with the retail function bytes.

The default comparison masks relocation values. Pass --linked to resolve
symbols against the existing retail-matching build/USA/main.elf and compare
every byte, including call targets and global addresses.
"""
import re, subprocess, struct, sys, pathlib
ROOT = pathlib.Path('.')
REF = open('assets/USA/main.exe', 'rb').read()
CFG = open('configs/USA/main.yaml').read()
BOUND = sorted(int(m.group(1), 16) for m in re.finditer(r'-\s*\[\s*(0x[0-9A-Fa-f]+)', CFG))
if len(sys.argv) not in (2, 3) or (len(sys.argv) == 3 and sys.argv[2] != '--linked'):
    sys.exit('usage: probe_match.py <name> [--linked]')
name = sys.argv[1]
linked = len(sys.argv) == 3
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
compare_obj = obj
if linked:
    symbols_elf = ROOT / 'build/USA/main.elf'
    built_exe = ROOT / 'build/USA/main.exe'
    if not symbols_elf.exists() or not built_exe.exists():
        sys.exit('build/USA/main.elf and main.exe are required for --linked')
    if built_exe.read_bytes() != REF:
        sys.exit('build/USA/main.exe does not match retail; run make check first')
    symbols = {}
    nm_output = subprocess.check_output(['mipsel-none-elf-nm', str(symbols_elf)], text=True)
    for line in nm_output.splitlines():
        entry = re.match(r'^([0-9a-fA-F]+)\s+\w\s+(\S+)$', line)
        if entry:
            symbols[entry.group(2)] = int(entry.group(1), 16)
    undefined = [line.split()[-1] for line in subprocess.check_output(
        ['mipsel-none-elf-nm', '-u', str(obj)], text=True).splitlines()]
    missing = [symbol for symbol in undefined if symbol not in symbols]
    if missing:
        sys.exit('unresolved symbols for --linked: ' + ', '.join(missing))
    if '_gp' not in symbols:
        sys.exit('build/USA/main.elf has no _gp symbol')
    # The PS-X EXE header is 0x800 bytes. Its load address is stored at 0x18.
    address = struct.unpack_from('<I', REF, 0x18)[0] + off - 0x800
    compare_obj = obj.with_suffix('.linked.elf')
    link_script = obj.with_suffix('.linked.ld')
    link_script.write_text(f'_gp = 0x{symbols["_gp"]:08x};\n'
                           f'SECTIONS {{ . = 0x{address:08x}; .text : {{ *(.text) }} }}\n')
    link_cmd = ['mipsel-none-elf-ld', '-EL', '-T', str(link_script),
                '-e', name.rsplit('/', 1)[-1], '-o', str(compare_obj), str(obj)]
    link_cmd += [f'--defsym={symbol}=0x{symbols[symbol]:08x}' for symbol in undefined]
    result = subprocess.run(link_cmd, capture_output=True, text=True)
    if result.returncode:
        link_script.unlink()
        sys.exit('LINK FAIL\n' + result.stderr)
    link_script.unlink()
subprocess.run(['mipsel-none-elf-objcopy', '-O', 'binary', '--only-section', '.text', str(compare_obj), str(binp)], capture_output=True, check=True)
data = binp.read_bytes(); binp.unlink()
relocs = {}
if not linked:
    for l in subprocess.run(['mipsel-none-elf-objdump', '-r', '--section=.text', str(obj)], capture_output=True, text=True).stdout.splitlines():
        mm = re.match(r'^([0-9a-f]{8})\s+(\S+)', l)
        if mm: relocs[int(mm.group(1), 16)] = mm.group(2)
dis = {f'{int(m2.group(1), 16) - (address if linked else 0):x}': m2.group(2).strip() for m2 in re.finditer(r'^\s+([0-9a-f]+):\s+[0-9a-f]{8}\s+(.+)$', subprocess.run(['mipsel-none-elf-objdump', '-d', str(compare_obj)], capture_output=True, text=True).stdout, re.M)}
obj.unlink()
if linked:
    compare_obj.unlink()
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
        shift = 26 if t == 'R_MIPS_26' else 16 if t in ('R_MIPS_HI16', 'R_MIPS_LO16', 'R_MIPS_GPREL16') else None
        if shift is not None and (r >> shift) != (b >> shift):
            print(f"  +{i:03x} retail {r:08x}  built {b:08x}  [reloc {t}: {dis.get(f'{i:x}', '?')}]"); diffs += 1
        continue
    if r != b:
        print(f"  +{i:03x} retail {r:08x}  built {b:08x}  [built: {dis.get(f'{i:x}', '?')}]"); diffs += 1
print("MATCH" if diffs == 0 and len(data) == size else f"NOMATCH ({diffs} word diffs)")
sys.exit(0 if diffs == 0 and len(data) == size else 1)
