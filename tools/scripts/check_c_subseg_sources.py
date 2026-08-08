#!/usr/bin/env python3
"""Fresh-clone guard: every `c` subsegment in a committed overlay/main config
must have a committed source file. Catches the class of bug where a config is
promoted asm->c but the .c is never `git add`ed, breaking fresh-clone builds
(as scene_e08_header.c did, 2026-07). Exit 1 on any missing source."""
import re, subprocess, sys, glob
tracked = set(subprocess.check_output(['git', 'ls-files']).decode().splitlines())
missing = []
for cfg in glob.glob('configs/USA/overlays/*.yaml') + glob.glob('configs/USA/*.yaml'):
    try:
        txt = subprocess.check_output(['git', 'show', f'HEAD:{cfg}'],
                                      stderr=subprocess.DEVNULL).decode()
    except subprocess.CalledProcessError:
        continue
    m = re.search(r'src_path:\s*(\S+)', txt)
    if not m:
        continue
    src = m.group(1)
    for sm in re.finditer(r'-\s*\[\s*0x[0-9A-Fa-f]+,\s*c,\s*([A-Za-z0-9_+]+)\s*\]', txt):
        p = f'{src}/{sm.group(1)}.c'
        if p not in tracked:
            missing.append((cfg, p))
if missing:
    print(f"ERROR: {len(missing)} committed 'c' subsegment(s) reference untracked source:")
    for cfg, p in missing:
        print(f"  {cfg} -> {p}")
    print("Commit the source (git add), or change the subsegment back to asm.")
    sys.exit(1)
print(f"OK: all committed 'c' subsegments have tracked sources.")
