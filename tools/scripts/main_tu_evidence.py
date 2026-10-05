#!/usr/bin/env python3
"""Propose translation-unit groups for the resident executable from binary evidence.

Read-only. Needs `make build` (build/USA/main.elf and main.map). For every
function the linked relocations give its calls, address-taken functions and
data references; data references are bucketed into items at the nearest
preceding symbol. Each gap between two adjacent functions then collects
weighted evidence:

join (same TU)
  literal  - an anonymous .rodata item (string, float pool, jump table) is
             shared by two functions. GCC emits literals per TU, so sharing
             is only possible inside one object (named const tables can
             mimic this, so wide spans are capped).
  order    - a literal private to an earlier function sits after a literal
             private to a later one. Objects are laid out in text order in
             every section, so an inversion can only happen inside one TU.
  private  - a .data/.bss item used only by functions within a short span
             (static-like state; weak, globals can look the same).
  helper   - a callee whose callers/address-takers all sit within a short span
             (static-helper-like; weak).
  call     - one neighbour calls the other directly (weak).

sdk (either way)
  sdk      - both functions inside one matched Psy-Q object (join), or the
             gap is the edge of such an object (split).

split (different TU)
  compiler - adjacent sources need different compilers (GCC 2.7.2 vs 2.8.1).
  gprofile - the same item is reached through $gp in one neighbour and
             through an absolute address in the other.
  flags    - other per-source cc1/maspsx -G or -O differences (weak; the flags
             were chosen to match, a merged TU must share them).
  pad      - unclaimed bytes between the two functions in .text (weak).

Groups are maximal runs of gaps whose net score reaches --join-threshold.
The report compares them with the current source files and checks whether a
merge of adjacent files is mechanically possible (every non-text section of
the two objects adjacent in link order).

Output: text report on stdout, JSON in build/USA/analysis/main_tu_evidence.json.
"""
from __future__ import annotations

import argparse
import bisect
import collections
import json
import pathlib
import re
import subprocess
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import pe1_linked as L  # noqa: E402

SPAN_PRIVATE = 12
SPAN_LITERAL = 40


def nm_symbols(elf: pathlib.Path) -> dict[str, int]:
    out = subprocess.run(["mipsel-none-elf-nm", str(elf)], capture_output=True, text=True,
                         check=True).stdout
    syms = {}
    for line in out.splitlines():
        parts = line.split()
        if len(parts) == 3:
            syms[parts[2]] = int(parts[0], 16)
    return syms


class Regions:
    def __init__(self, syms):
        self.ranges = []
        for seg in ("main", "field_engine"):
            for kind, a, b in (("rodata", "RODATA_START", "RODATA_END"),
                               ("text", "TEXT_START", "TEXT_END"),
                               ("data", "DATA_START", "DATA_END")):
                s, e = syms.get(f"{seg}_{a}"), syms.get(f"{seg}_{b}")
                if s is not None and e is not None and e > s:
                    self.ranges.append((s, e, kind))

    def kind(self, addr):
        for s, e, k in self.ranges:
            if s <= addr < e:
                return k
        return "ext"


def source_profile(source: str) -> dict:
    text = (L.ROOT / source).read_text(errors="replace") if source else ""
    gcc = "2.8.1" if re.search(r"GCC_VERSION:.*2\.8\.1", text) else "2.7.2"
    g = re.search(r"CC1_FLAGS:.*-G(\d+)", text)
    ag = re.search(r"MASPSX_FLAGS:.*-G(\d+)", text)
    opt = re.search(r"CC1_FLAGS:.*-(O[0-3])", text)
    return {"gcc": gcc, "G": g.group(1) if g else "0", "asG": ag.group(1) if ag else "",
            "O": opt.group(1) if opt else "O2",
            "lines": text.count("\n")}


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--out", default=str(L.BUILD / "analysis" / "main_tu_evidence.json"))
    parser.add_argument("--join-threshold", type=float, default=1.0)
    parser.add_argument("--top", type=int, default=40)
    parser.add_argument("--tiny-lines", type=int, default=20)
    args = parser.parse_args()

    module = L.load_module("main")
    syms = nm_symbols(module.elf)
    regions = Regions(syms)
    fns = [f for f in module.functions if regions.kind(f.address) == "text"]
    index_of = {f.address: i for i, f in enumerate(fns)}
    n = len(fns)

    # Item starts: every symbol plus every object section start in data regions.
    starts = set()
    for name, addr in syms.items():
        if regions.kind(addr) in ("rodata", "data"):
            starts.add(addr)
    for obj, secs in module.placement.items():
        for sec, (addr, size) in secs.items():
            if regions.kind(addr) in ("rodata", "data"):
                starts.add(addr)
    starts = sorted(starts)

    def item_of(addr):
        i = bisect.bisect_right(starts, addr) - 1
        return starts[i] if i >= 0 else addr

    # Owner object of each data address (for the rodata-blob ownership view).
    sec_ranges = sorted((a, a + s, obj) for obj, secs in module.placement.items()
                        for sec, (a, s) in secs.items() if sec != ".text")
    sec_starts = [r[0] for r in sec_ranges]

    def owner(addr):
        i = bisect.bisect_right(sec_starts, addr) - 1
        if i >= 0 and sec_ranges[i][0] <= addr < sec_ranges[i][1]:
            return sec_ranges[i][2]
        return None

    calls = [set() for _ in range(n)]
    refs = [dict() for _ in range(n)]  # item -> set(modes)
    referencers = collections.defaultdict(set)
    item_kind = {}
    callers = collections.defaultdict(set)
    for i, f in enumerate(fns):
        for r in f.relocs:
            kind = regions.kind(r.target)
            if kind == "text":
                j = index_of.get(r.target)
                if j is not None and j != i:
                    calls[i].add(j)
                    callers[j].add(i)
                continue
            if kind not in ("rodata", "data"):
                continue
            # Literals are referenced at their exact start; data through
            # base+offset, so data is bucketed at the preceding symbol.
            item = r.target if kind == "rodata" else item_of(r.target)
            mode = "gp" if r.rtype == L.R_MIPS_GPREL16 else "abs"
            refs[i].setdefault(item, set()).add(mode)
            referencers[item].add(i)
            item_kind[item] = kind

    join = [collections.Counter() for _ in range(max(n - 1, 0))]
    split = [collections.Counter() for _ in range(max(n - 1, 0))]

    def add_join(lo, hi, label, w):
        for g in range(lo, hi):
            join[g][label] += w

    # literal / private data spans
    for item, users in referencers.items():
        if len(users) < 2:
            continue
        lo, hi = min(users), max(users)
        span = hi - lo
        if item_kind[item] == "rodata":
            if span <= SPAN_LITERAL:
                add_join(lo, hi, "literal", 3.0 if span <= SPAN_PRIVATE else 1.0)
        elif span <= SPAN_PRIVATE:
            add_join(lo, hi, "private", 0.5 if span <= 4 else 0.25)
    # helper spans
    for j, cs in callers.items():
        users = cs | {j}
        lo, hi = min(users), max(users)
        if hi - lo <= SPAN_PRIVATE:
            add_join(lo, hi, "helper", 0.5 if hi - lo <= 4 else 0.25)
    # direct neighbour call
    for g in range(n - 1):
        if (g + 1) in calls[g] or g in calls[g + 1]:
            join[g]["call"] += 0.25
    # Order inversions of function-private literals. A global used by one
    # function can be defined in any TU, so only .rodata literals count.
    private_items = [[] for _ in range(n)]
    for item, users in referencers.items():
        if len(users) == 1 and item_kind[item] == "rodata":
            (u,) = users
            private_items[u].append(item)
    for a in range(n):
        for b in range(a + 1, min(n, a + SPAN_PRIVATE + 1)):
            pa, pb = private_items[a], private_items[b]
            if pa and pb and max(pa) > min(pb):
                add_join(a, b, "order", 2.0 if b == a + 1 else 1.0)
    # split evidence
    profiles = {}
    for f in fns:
        if f.source not in profiles:
            profiles[f.source] = source_profile(f.source)
    for g in range(n - 1):
        a, b = fns[g], fns[g + 1]
        if a.source != b.source:
            pa, pb = profiles[a.source], profiles[b.source]
            if pa["gcc"] != pb["gcc"]:
                split[g]["compiler"] += 3.0
            elif (pa["G"], pa["asG"], pa["O"]) != (pb["G"], pb["asG"], pb["O"]):
                split[g]["flags"] += 1.0
        for item in set(refs[g]) & set(refs[g + 1]):
            if refs[g][item] != refs[g + 1][item] and len(refs[g][item] | refs[g + 1][item]) == 2 \
                    and len(refs[g][item]) == 1 and len(refs[g + 1][item]) == 1:
                split[g]["gprofile"] += 2.0
                break
        if a.address + a.size < b.address:
            split[g]["pad"] += 0.5

    # Known Psy-Q object ranges (configs/USA/psyq_provenance.json).
    prov = L.ROOT / "configs" / "USA" / "psyq_provenance.json"
    sdk_ranges = []
    if prov.exists():
        for e in json.loads(prov.read_text())["evidence"]:
            if e.get("scope") == "object":
                a = int(e["address"], 16)
                sdk_ranges.append((a, a + e["size"], f'{e["library"]}/{e["object"]}'))
    for g in range(n - 1):
        a, b = fns[g], fns[g + 1]
        for lo, hi, _ in sdk_ranges:
            if lo <= a.address < hi and lo <= b.address < hi:
                join[g]["sdk"] += 5.0
                break
            if (lo <= a.address < hi) != (lo <= b.address < hi):
                split[g]["sdk"] += 5.0
                break

    net = [sum(join[g].values()) - sum(split[g].values()) for g in range(n - 1)]

    # Groups
    groups = []
    start = 0
    for g in range(n - 1):
        if net[g] < args.join_threshold:
            groups.append((start, g))
            start = g + 1
    groups.append((start, n - 1))

    def directory(source):
        parts = pathlib.Path(source).parts
        return parts[2] if len(parts) > 3 else "(root)"

    files = []
    file_index = {}
    for i, f in enumerate(fns):
        if f.source not in file_index:
            file_index[f.source] = len(files)
            files.append({"source": f.source, "first": i, "last": i})
        files[file_index[f.source]]["last"] = i
    # Mechanical merge feasibility between consecutive files.
    order_in_section = collections.defaultdict(list)
    for obj, secs in module.placement.items():
        for sec, (addr, size) in secs.items():
            order_in_section[sec].append((addr, size, obj))
    for sec in order_in_section:
        order_in_section[sec].sort()
    obj_of_source = {}
    for f in fns:
        obj_of_source[f.source] = f.obj

    def adjacent_everywhere(src_a, src_b):
        oa, ob = obj_of_source[src_a], obj_of_source[src_b]
        sa, sb = module.placement.get(oa, {}), module.placement.get(ob, {})
        problems = []
        for sec in set(sa) | set(sb):
            if sec == ".text" or sec not in sa or sec not in sb:
                continue
            seq = order_in_section[sec]
            ia = next(i for i, x in enumerate(seq) if x[2] == oa)
            ib = next(i for i, x in enumerate(seq) if x[2] == ob)
            if ib != ia + 1:
                problems.append(f"{sec}:{ib - ia - 1} objects between")
        return problems

    group_records = []
    for gi, (lo, hi) in enumerate(groups):
        srcs = []
        for i in range(lo, hi + 1):
            if not srcs or srcs[-1] != fns[i].source:
                srcs.append(fns[i].source)
        labels = collections.Counter()
        for g in range(lo, hi):
            labels.update(join[g])
        strength = "single" if lo == hi else (
            "strong" if labels["literal"] >= 3 or labels["order"] >= 2 or labels["sdk"] else
            "medium" if labels["private"] + labels["helper"] >= 1 else "weak")
        problems = []
        for a, b in zip(srcs, srcs[1:]):
            problems += [f"{pathlib.Path(a).stem}->{pathlib.Path(b).stem} {p}"
                         for p in adjacent_everywhere(a, b)]
        group_records.append({
            "id": f"G{gi:04d}", "first": fns[lo].name, "last": fns[hi].name,
            "start": f"0x{fns[lo].address:08X}",
            "end": f"0x{fns[hi].address + fns[hi].size:08X}",
            "functions": hi - lo + 1, "files": srcs, "strength": strength,
            "score": round(sum(net[g] for g in range(lo, hi)), 1),
            "evidence": dict(labels),
            "directories": sorted({directory(s) for s in srcs}),
            "mergeable": not problems, "merge_problems": problems,
            "tiny_files": sum(1 for s in srcs if profiles[s]["lines"] < args.tiny_lines),
        })

    # Current files spanning split evidence.
    questionable = []
    for rec in files:
        bad = [(fns[g].name, fns[g + 1].name, dict(split[g]), round(net[g], 1))
               for g in range(rec["first"], rec["last"]) if net[g] < 0]
        if bad:
            questionable.append({"source": rec["source"], "gaps": bad})

    # Directory view.
    dir_stats = collections.defaultdict(lambda: {"files": 0, "functions": 0, "runs": 0,
                                                 "groups": set(), "prefixes": collections.Counter()})
    prev_dir = None
    for i, f in enumerate(fns):
        d = directory(f.source)
        dir_stats[d]["functions"] += 1
        if d != prev_dir:
            dir_stats[d]["runs"] += 1
        prev_dir = d
    for rec in files:
        d = directory(rec["source"])
        dir_stats[d]["files"] += 1
        stem = pathlib.Path(rec["source"]).stem
        dir_stats[d]["prefixes"][re.split(r"[_]", stem)[0]] += 1
    for grec in group_records:
        for d in grec["directories"]:
            dir_stats[d]["groups"].add(grec["id"])
    multi_dir_groups = [g for g in group_records if len(g["directories"]) > 1]

    # Rodata/data blobs not owned by a C object, with their referencers.
    blob_refs = collections.defaultdict(set)
    for item, users in referencers.items():
        obj = owner(item)
        if obj and "/asm/" in obj:
            for u in users:
                blob_refs[obj].add(fns[u].source)

    merge_candidates = [g for g in group_records if len(g["files"]) > 1]
    merge_candidates.sort(key=lambda g: (-{"strong": 3, "medium": 2, "weak": 1}[g["strength"]],
                                         not g["mergeable"], -g["score"]))

    out = pathlib.Path(args.out)
    out.parent.mkdir(parents=True, exist_ok=True)
    out.write_text(json.dumps({
        "functions": n, "files": len(files), "groups": group_records,
        "questionable_files": questionable,
        "gaps": [{"after": fns[g].name, "before": fns[g + 1].name,
                  "join": dict(join[g]), "split": dict(split[g]), "net": round(net[g], 2)}
                 for g in range(n - 1)],
        "directories": {d: {"files": s["files"], "functions": s["functions"], "runs": s["runs"],
                            "groups": len(s["groups"]), "prefixes": dict(s["prefixes"])}
                        for d, s in dir_stats.items()},
        "unowned_blob_referencers": {k: sorted(v) for k, v in blob_refs.items()},
    }, indent=1) + "\n")

    multi = [g for g in group_records if g["functions"] > 1]
    print(f"functions {n}  current files {len(files)}  evidence groups {len(group_records)} "
          f"(multi-function {len(multi)}, singletons {len(group_records) - len(multi)})")
    by_strength = collections.Counter(g["strength"] for g in group_records)
    print("group strength: " + "  ".join(f"{k}:{v}" for k, v in sorted(by_strength.items())))
    print(f"groups spanning several current files (merge candidates): {len(merge_candidates)} "
          f"covering {sum(len(g['files']) for g in merge_candidates)} files; mechanically "
          f"mergeable now: {sum(1 for g in merge_candidates if g['mergeable'])}")
    print(f"groups spanning several directories: {len(multi_dir_groups)}")
    print(f"current files containing a gap with net split evidence: {len(questionable)}")
    print()
    print(f"{'dir':10s} {'files':>5s} {'funcs':>5s} {'runs':>4s} {'groups':>6s}  prefixes")
    for d, s in sorted(dir_stats.items(), key=lambda kv: -kv[1]["files"]):
        top = ",".join(f"{p}:{c}" for p, c in s["prefixes"].most_common(5))
        print(f"{d:10s} {s['files']:5d} {s['functions']:5d} {s['runs']:4d} {len(s['groups']):6d}  {top}")
    print()
    print(f"top {args.top} merge candidates:")
    for g in merge_candidates[:args.top]:
        names = " + ".join(pathlib.Path(s).stem for s in g["files"])
        flag = "ok " if g["mergeable"] else "BLK"
        print(f"  {g['id']} {flag} {g['strength']:6s} {g['score']:6.1f} {g['start']}..{g['end']} "
              f"{','.join(g['directories'])}: {names[:150]}")
    print(f"\nmanifest: {out.relative_to(L.ROOT)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
