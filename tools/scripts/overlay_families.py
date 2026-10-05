#!/usr/bin/env python3
"""Group every overlay function into families of identical linked code.

Read-only analysis over `make overlay-build-all` output. Each function's
linked words are normalized by masking every relocated operand field; the
resolved targets are then classified:

* `exact`  - targets inside the function's own overlay become LOCAL, targets
  in the resident executable keep their absolute address (same main API,
  same main data offsets). Two functions in one family are therefore the
  same compiled code linked at different overlay addresses.
* `loose`  - every relocation target is masked (same code, possibly calling
  different main entry points or touching different main data).
* `shape`  - additionally masks all 16-bit immediates of I-type instructions
  (same control flow and register use; constants may differ).

Output: a text summary on stdout and a JSON manifest (default
build/USA/analysis/overlay_families.json, not tracked).
"""
from __future__ import annotations

import argparse
import collections
import hashlib
import json
import pathlib
import re
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import pe1_linked as L  # noqa: E402

# Shared-source mechanisms: .inc templates, function-generating *_family.h
# macro headers, and C files included from another overlay or candidates/.
INCLUDE_INC = re.compile(r'#include\s+"([^"]+(?:\.inc|_family\.h|\.c))"')
# I-type opcodes whose low 16 bits are data immediates or displacements
# (branches excluded: their offsets are structural).
IMM_OPS = {0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
           0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x28, 0x29, 0x2A,
           0x2B, 0x2E, 0x32, 0x3A}


def overlay_class(name: str) -> str:
    for prefix in ("room_", "scene_", "fx_", "boot_", "menu_", "sys_", "render_"):
        if name.startswith(prefix):
            return prefix.rstrip("_")
    return "other"


def keys(module: L.Module, function: L.Function, external=None):
    external = external or (lambda a: not module.contains(a))
    words = list(function.words)
    exact_targets = []
    for rel in function.relocs:
        index = rel.offset // 4
        if 0 <= index < len(words):
            words[index] = L.mask_word(words[index], rel.rtype)
        exact_targets.append(
            f"{rel.offset:x}:{rel.rtype}:"
            + (f"{rel.target:08x}" if external(rel.target) else "LOCAL"))
    body = ",".join(f"{w:08x}" for w in words)
    exact = hashlib.sha1((body + "|" + ";".join(exact_targets)).encode()).hexdigest()[:16]
    loose_targets = ";".join(f"{r.offset:x}:{r.rtype}" for r in function.relocs)
    loose = hashlib.sha1((body + "|" + loose_targets).encode()).hexdigest()[:16]
    shaped = []
    for w in words:
        if (w >> 26) in IMM_OPS:
            w &= 0xFFFF0000
        shaped.append(f"{w:08x}")
    shape = hashlib.sha1(",".join(shaped).encode()).hexdigest()[:16]
    return exact, loose, shape


_inc_cache: dict[str, list[str]] = {}


def includes(source: str) -> list[str]:
    if source not in _inc_cache:
        path = L.ROOT / source
        found = []
        if source and path.exists():
            for inc in INCLUDE_INC.findall(path.read_text(errors="replace")):
                found.append(str((path.parent / inc).resolve().relative_to(L.ROOT)))
        _inc_cache[source] = found
    return _inc_cache[source]


def bucket(n: int) -> str:
    for limit, label in ((1, "1"), (2, "2"), (5, "3-5"), (10, "6-10"), (25, "11-25"),
                         (50, "26-50"), (100, "51-100"), (200, "101-200")):
        if n <= limit:
            return label
    return ">200"


BUCKETS = ["1", "2", "3-5", "6-10", "11-25", "26-50", "51-100", "101-200", ">200"]


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--out", default=str(L.BUILD / "analysis" / "overlay_families.json"))
    parser.add_argument("--top", type=int, default=40)
    parser.add_argument("--library-threshold", type=int, default=3,
                        help="overlays a family needs to count as shared library code")
    args = parser.parse_args()

    members = []  # one record per overlay function
    missing = []
    for name in L.overlay_names():
        try:
            module = L.load_module(name)
        except FileNotFoundError:
            missing.append(name)
            continue
        for index, fn in enumerate(module.functions):
            exact, loose, shape = keys(module, fn)
            members.append({
                "overlay": name, "class": overlay_class(name), "index": index,
                "name": fn.name, "address": fn.address, "size": fn.size,
                "source": fn.source, "inc": includes(fn.source),
                "exact": exact, "loose": loose, "shape": shape,
            })
    if missing:
        print(f"warning: {len(missing)} overlays not built: {' '.join(missing[:10])}", file=sys.stderr)

    # Resident executable, keyed loosely, to flag overlay copies of main code.
    main_loose = {}
    try:
        main = L.load_module("main")
        for fn in main.functions:
            _, loose, _ = keys(main, fn, external=lambda a: True)
            main_loose.setdefault(loose, fn.name)
    except FileNotFoundError:
        print("warning: main not built", file=sys.stderr)

    families = collections.defaultdict(list)
    for m in members:
        families[m["exact"]].append(m)
    loose_groups = collections.defaultdict(set)
    shape_groups = collections.defaultdict(set)
    for m in members:
        loose_groups[m["loose"]].add(m["exact"])
        shape_groups[m["shape"]].add(m["exact"])

    fam_records = []
    for key, items in families.items():
        overlays = sorted({m["overlay"] for m in items})
        sources = sorted({m["source"] for m in items})
        inc_counts = collections.Counter(i for m in items for i in set(m["inc"]))
        main_inc, main_cov = (inc_counts.most_common(1)[0] if inc_counts else (None, 0))
        names = collections.Counter(re.sub(r"_[0-9A-Fa-f]{8}$", "", m["name"]) for m in items)
        fam_records.append({
            "key": key,
            "size": items[0]["size"],
            "instances": len(items),
            "overlays": len(overlays),
            "classes": sorted({m["class"] for m in items}),
            "name": names.most_common(1)[0][0],
            "names": dict(names.most_common(5)),
            "template": main_inc,
            "template_coverage": main_cov,
            "loose_key": items[0]["loose"],
            "shape_key": items[0]["shape"],
            "in_main": main_loose.get(items[0]["loose"]),
            "members": [{"overlay": m["overlay"], "address": f"0x{m['address']:08X}",
                         "name": m["name"], "source": m["source"]} for m in items],
            "_sources": sources,
        })
    fam_records.sort(key=lambda r: (-r["instances"] * r["size"], r["name"]))
    # Stable family ids by importance.
    for i, rec in enumerate(fam_records):
        rec["id"] = f"F{i:04d}"
    by_key = {r["key"]: r for r in fam_records}

    # Bundles: families sharing exactly the same overlay set (>= threshold).
    bundles = collections.defaultdict(list)
    for rec in fam_records:
        if rec["overlays"] >= args.library_threshold:
            ovs = tuple(sorted({m["overlay"] for m in rec["members"]}))
            bundles[ovs].append(rec["id"])

    # Order evidence for the largest bundles: are bundle members adjacent and in
    # the same order in every overlay (one linked library object)?
    per_overlay = collections.defaultdict(list)
    for m in members:
        per_overlay[m["overlay"]].append(m)
    bundle_records = []
    for ovs, ids in sorted(bundles.items(), key=lambda kv: (-len(kv[0]) * len(kv[1]))):
        idset = set(ids)
        orders = collections.Counter()
        contiguous = 0
        runs_hist = collections.Counter()
        interleaved = collections.Counter()
        for ov in ovs:
            seq = [by_key[m["exact"]]["id"] for m in per_overlay[ov]]
            pos = [i for i, fid in enumerate(seq) if fid in idset]
            orders[tuple(seq[i] for i in pos)] += 1
            if pos and pos[-1] - pos[0] + 1 == len(pos):
                contiguous += 1
            runs = 1 + sum(1 for a, b in zip(pos, pos[1:]) if b != a + 1)
            runs_hist[runs] += 1
            for i in range(pos[0], pos[-1] + 1):
                if seq[i] not in idset:
                    interleaved[seq[i]] += 1
        bundle_records.append({
            "overlays": len(ovs), "families": len(ids), "ids": ids,
            "overlay_list": list(ovs),
            "distinct_orders": len(orders),
            "contiguous_in": contiguous,
            "runs_histogram": dict(runs_hist),
            "interleaved_families": dict(interleaved.most_common(20)),
            "bytes_per_copy": sum(by_key_id["size"] for by_key_id in fam_records if by_key_id["id"] in idset),
        })

    total = len(members)
    multi = [r for r in fam_records if r["instances"] > 1]
    lib = [r for r in fam_records if r["overlays"] >= args.library_threshold]
    sources_total = {m["source"] for m in members}
    # A source file is a "copy" when every function in it belongs to a family
    # with more than one instance.
    fam_of = {(m["overlay"], m["address"]): by_key[m["exact"]] for m in members}
    src_funcs = collections.defaultdict(list)
    for m in members:
        src_funcs[m["source"]].append(fam_of[(m["overlay"], m["address"])])
    copy_sources = [s for s, recs in src_funcs.items() if all(r["instances"] > 1 for r in recs)]
    lib_sources = [s for s, recs in src_funcs.items()
                   if all(r["overlays"] >= args.library_threshold for r in recs)]
    templated_sources = [s for s in sources_total if includes(s)]
    hist_fam = collections.Counter(bucket(r["instances"]) for r in fam_records)
    hist_inst = collections.Counter()
    for r in fam_records:
        hist_inst[bucket(r["instances"])] += r["instances"]
    template_fams = [r for r in multi if r["template"] and r["template_coverage"] == r["instances"]]
    partial_fams = [r for r in multi if r["template"] and 0 < r["template_coverage"] < r["instances"]]
    untemplated_multi = [r for r in multi if not r["template"]]
    redundant = sum(r["instances"] - 1 for r in multi)

    summary = {
        "overlays": len(per_overlay), "functions": total, "sources": len(sources_total),
        "families_exact": len(fam_records),
        "families_loose": len(loose_groups), "families_shape": len(shape_groups),
        "singleton_families": len(fam_records) - len(multi),
        "multi_instance_families": len(multi),
        "redundant_instances": redundant,
        "library_families": len(lib),
        "library_instances": sum(r["instances"] for r in lib),
        "copy_sources": len(copy_sources),
        "library_only_sources": len(lib_sources),
        "templated_sources": len(templated_sources),
        "multi_families_with_template": len(template_fams),
        "multi_families_partly_templated": len(partial_fams),
        "multi_instances_templated": sum(r["template_coverage"] for r in multi if r["template"]),
        "multi_families_without_template": len(untemplated_multi),
        "multi_instances_without_template": sum(r["instances"] for r in untemplated_multi),
        "families_also_in_main": sum(1 for r in fam_records if r["in_main"]),
        "histogram_families": {b: hist_fam.get(b, 0) for b in BUCKETS},
        "histogram_instances": {b: hist_inst.get(b, 0) for b in BUCKETS},
    }
    out = pathlib.Path(args.out)
    out.parent.mkdir(parents=True, exist_ok=True)
    for rec in fam_records:
        rec.pop("_sources", None)
    out.write_text(json.dumps({"summary": summary, "bundles": bundle_records,
                               "families": fam_records}, indent=1) + "\n")

    print(f"overlays {summary['overlays']}  functions {total}  sources {len(sources_total)}")
    print(f"families exact {summary['families_exact']}  loose {summary['families_loose']}  "
          f"shape {summary['families_shape']}")
    print(f"singletons {summary['singleton_families']}  multi-instance families {len(multi)} "
          f"covering {sum(r['instances'] for r in multi)} functions ({redundant} redundant copies)")
    print(f"library families (>= {args.library_threshold} overlays) {len(lib)} with "
          f"{summary['library_instances']} instances")
    print(f"sources whose every function is a multi-instance copy: {len(copy_sources)}; "
          f"library-only sources: {len(lib_sources)}; sources including a .inc: {len(templated_sources)}")
    print(f"multi-instance families fully templated: {len(template_fams)}; partly: {len(partial_fams)} "
          f"({summary['multi_instances_templated']} instances use the family template); "
          f"not templated: {len(untemplated_multi)} ({summary['multi_instances_without_template']} instances)")
    print(f"families with a loose twin in main: {summary['families_also_in_main']}")
    print("instances per family: " + "  ".join(
        f"{b}:{hist_fam.get(b, 0)}f/{hist_inst.get(b, 0)}i" for b in BUCKETS))
    print()
    print(f"top {args.top} families by duplicated bytes (instances x size):")
    for rec in fam_records[:args.top]:
        tmpl = (f"{pathlib.Path(rec['template']).name}({rec['template_coverage']})"
                if rec["template"] else "-")
        print(f"  {rec['id']} {rec['instances']:4d}x {rec['size']:5d}B ovl={rec['overlays']:3d} "
              f"{','.join(rec['classes']):12s} {rec['name'][:44]:44s} tmpl={tmpl}")
    print()
    print("largest bundles (families with identical overlay sets):")
    for b in bundle_records[:15]:
        print(f"  {b['overlays']:3d} overlays x {b['families']:3d} families, {b['bytes_per_copy']}B/copy, "
              f"orders={b['distinct_orders']} contiguous_in={b['contiguous_in']} "
              f"runs={dict(sorted(b['runs_histogram'].items()))} e.g. {b['ids'][:4]}")
    print(f"\nmanifest: {out.relative_to(L.ROOT)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
