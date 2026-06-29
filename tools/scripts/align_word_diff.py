import re, difflib, sys
fn = sys.argv[1]
tgt, cand = [], []
for ln in open(f'proposals/{fn}/target.s'):
    m = re.match(r'\s+/\* \w+ (\w+) (\w{8}) \*/\s+(\S+)\s*(.*)', ln)
    if m:
        w = m.group(2); word = ''.join([w[6:8],w[4:6],w[2:4],w[0:2]])
        tgt.append((m.group(1), word.lower(), m.group(3)+' '+m.group(4).strip()))
for ln in open(f'proposals/{fn}/build/candidate.objdump'):
    m = re.match(r'\s*([0-9a-f]+):\s+([0-9a-f]{8})\s+(.*)', ln)
    if m: cand.append((m.group(1), m.group(2), m.group(3).strip()))
a=[t[1] for t in tgt]; b=[c[1] for c in cand]
print(f"target {len(a)}, cand {len(b)}")
sm = difflib.SequenceMatcher(None,a,b,autojunk=False)
for tag,i1,i2,j1,j2 in sm.get_opcodes():
    if tag=='equal': continue
    print(f"--- {tag} T[{i1}:{i2}] C[{j1}:{j2}]")
    for k in range(max(0,i1-2), i1): print(f"  ctx {tgt[k][0]} {tgt[k][2]}")
    for k in range(i1,i2): print(f"  T {tgt[k][0]} {tgt[k][1]} {tgt[k][2]}")
    for k in range(j1,j2): print(f"  C {cand[k][0]} {cand[k][1]} {cand[k][2]}")
