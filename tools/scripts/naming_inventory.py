#!/usr/bin/env python3
"""Inventory organization naming debt: opaque filenames and address-named globals.

Read-only. Source scans work on a fresh clone; the per-TU symbol usage needs
the built objects (`make build` and `make overlay-build-all`).

Filenames are classified as
  address      func_<ADDR>                 (identity unknown)
  suffixed     <Name>_<ADDR>               (named, address kept for uniqueness)
  placeholder  misc12, menu5, task7, ...   (module boundary unknown)
  named        everything else

Globals: every file-scope `extern` declaration in a .c file, and every
D_<ADDR> / func_<ADDR> symbol referenced by a compiled object. A D_ symbol
referenced by exactly one translation unit is a static candidate when its
address also lies in that unit's module; symbols used by several TUs need a
header owner.

Output: text report on stdout, JSON in build/USA/analysis/naming_inventory.json.
"""
from __future__ import annotations

import argparse
import collections
import json
import pathlib
import re
import sys

from elftools.elf.elffile import ELFFile

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import pe1_linked as L  # noqa: E402

ADDRESS = re.compile(r"func_[0-9A-Fa-f]{6,}")
SUFFIXED = re.compile(r".+_[0-9A-Fa-f]{8}")
PLACEHOLDER = re.compile(
    r"(?:misc|main|task|entity|seq|spu|akao|menu|engine|gpu|psyq)\d*", re.IGNORECASE)
EXTERN = re.compile(r"^extern\b[^;(]*?(\b[A-Za-z_]\w*)\s*(?:\[[^\]]*\]\s*)*;|"
                    r"^extern\b.*?\b([A-Za-z_]\w*)\s*\(", re.M)
ADDR_SYM = re.compile(r"\b(D|func)_([0-9A-Fa-f]{8})\b")


def classify_file(stem: str) -> str:
    if ADDRESS.fullmatch(stem):
        return "address"
    if PLACEHOLDER.fullmatch(stem):
        return "placeholder"
    if SUFFIXED.fullmatch(stem):
        return "suffixed"
    return "named"


def scope_of(rel: pathlib.Path) -> tuple[str, str]:
    """(scope, group) for a path relative to src/."""
    parts = rel.parts
    if parts[0] == "overlays":
        ov = parts[1]
        fam = re.sub(r"_[^_]*$", "", ov) if ov != "room_lib" else "room_lib"
        return "overlays", fam
    return "main", parts[1] if len(parts) > 2 else "(root)"


def overlay_range(name: str):
    cfg = L.ROOT / "configs" / "USA" / "overlays" / f"{name}.yaml"
    if not cfg.exists():
        return None
    m = re.search(r"vram:\s*0x([0-9A-Fa-f]+)", cfg.read_text())
    target = L.ROOT / "original" / "USA" / "overlays" / f"{name}.bin"
    if not m:
        return None
    lo = int(m.group(1), 16)
    size = target.stat().st_size if target.exists() else 0x30000
    return lo, lo + size


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--out", default=str(L.BUILD / "analysis" / "naming_inventory.json"))
    args = parser.parse_args()

    src = L.ROOT / "src"
    files = collections.defaultdict(lambda: collections.Counter())
    file_lists = collections.defaultdict(list)
    extern_decls = collections.defaultdict(lambda: collections.Counter())
    extern_sym_files = collections.defaultdict(set)
    totals = collections.Counter()
    for path in sorted(src.rglob("*.c")):
        rel = path.relative_to(src)
        scope, group = scope_of(rel)
        kind = classify_file(path.stem)
        files[(scope, group)][kind] += 1
        totals[(scope, kind)] += 1
        if kind in ("address", "placeholder"):
            file_lists[(scope, kind)].append(str(rel))
        text = path.read_text(errors="replace")
        text = re.sub(r'\b(?:__asm__|asm)\s*\("[^"]*"\)', "", text)
        for m in EXTERN.finditer(text):
            name = m.group(1) or m.group(2)
            if not name:
                continue
            is_func = m.group(2) is not None
            addr = bool(ADDR_SYM.fullmatch(name))
            key = ("addr_" if addr else "named_") + ("func" if is_func else "data")
            extern_decls[(scope, group)][key] += 1
            totals[(scope, "extern_" + key)] += 1
            extern_sym_files[name].add(str(rel))

    # Symbol usage per translation unit from built objects.
    tu_refs = collections.defaultdict(set)   # symbol -> set(tu)
    tu_defs = {}
    tu_scope = {}
    tu_module = {}
    objs = list((L.BUILD / "src").rglob("*.c.o")) + list((L.BUILD / "overlays").glob("*/src/**/*.c.o"))
    # Data blobs still emitted from splat asm reference symbols too (callback
    # tables, pointers); they count as users so a symbol they need is never
    # reported as private to one C file.
    objs += list((L.BUILD / "asm").rglob("*.s.o")) + list((L.BUILD / "overlays").glob("*/asm/**/*.s.o"))
    for obj in objs:
        rel = str(obj.relative_to(L.ROOT))
        source = L._source_for(rel)
        if source:
            srel = pathlib.Path(source).relative_to("src")
            scope, group = scope_of(srel)
            module = srel.parts[1] if scope == "overlays" else "main"
        else:
            m = re.match(r"build/USA/overlays/([^/]+)/", rel)
            module = m.group(1) if m else "main"
            scope = "overlays" if m else "main"
            group = scope_of(pathlib.Path("overlays", module, "x"))[1] if m else "(asm data)"
            source = "asm:" + rel
        tu_scope[source] = (scope, group)
        tu_module[source] = module
        with obj.open("rb") as handle:
            elf = ELFFile(handle)
            symtab = elf.get_section_by_name(".symtab")
            if symtab is None:
                continue
            for sym in symtab.iter_symbols():
                if not ADDR_SYM.fullmatch(sym.name or ""):
                    continue
                if sym["st_shndx"] == "SHN_UNDEF":
                    tu_refs[sym.name].add(source)
                elif sym["st_info"]["bind"] == "STB_GLOBAL":
                    # A C definition makes its TU a user; an asm data blob
                    # that merely holds the definition does not.
                    if not source.startswith("asm:"):
                        tu_refs[sym.name].add(source)
                    tu_defs[(sym.name, module)] = source
    built = bool(objs)
    ranges = {}
    usage = collections.Counter()
    by_group = collections.defaultdict(collections.Counter)
    static_candidates = []
    def home(addr: int, module: str) -> str:
        """Owning module of an address as seen from a TU in `module`."""
        if module != "main":
            if module not in ranges:
                ranges[module] = overlay_range(module)
            rng = ranges[module]
            if rng and rng[0] <= addr < rng[1]:
                return module
        return "main"

    # Overlays reuse the same load addresses, so D_8019xxxx in two overlays
    # are different objects: key every symbol by (name, home module).
    units = collections.defaultdict(set)
    for name, tus in tu_refs.items():
        addr = int(ADDR_SYM.fullmatch(name).group(2), 16)
        for tu in tus:
            units[(name, home(addr, tu_module[tu]))].add(tu)
    for (name, owner), tus in units.items():
        kind = ADDR_SYM.fullmatch(name).group(1)
        where = "main" if owner == "main" else "overlay"
        scopes = {tu_scope[t][0] for t in tus}
        if len(tus) == 1:
            (tu,) = tus
            definer = tu_defs.get((name, owner), "")
            local = definer == tu or owner == tu_module[tu]
            cls = f"{kind}_{where}_one_tu" + ("" if local else "_foreign")
            if local and definer.startswith("asm:"):
                cls += "_defined_in_asm_data"
            if tu.startswith("asm:"):
                cls = f"{kind}_{where}_asm_data_only"
            elif local and kind == "D":
                static_candidates.append({"symbol": name, "tu": tu})
        elif where == "overlay":
            cls = f"{kind}_overlay_shared_within_overlay"
        elif scopes == {"main"}:
            cls = f"{kind}_main_shared_main_only"
        elif scopes == {"overlays"}:
            cls = f"{kind}_main_shared_overlays_only"
        else:
            cls = f"{kind}_main_shared_main_and_overlays"
        for g in {tu_scope[t] for t in tus}:
            by_group[g][cls] += 1
        usage[cls] += 1

    dup_externs = sorted(((len(v), k) for k, v in extern_sym_files.items() if len(v) > 1), reverse=True)

    out = pathlib.Path(args.out)
    out.parent.mkdir(parents=True, exist_ok=True)
    out.write_text(json.dumps({
        "file_classes": {f"{s}/{g}": dict(c) for (s, g), c in sorted(files.items())},
        "opaque_files": {f"{s}:{k}": v for (s, k), v in file_lists.items()},
        "extern_declarations": {f"{s}/{g}": dict(c) for (s, g), c in sorted(extern_decls.items())},
        "symbol_usage": dict(usage),
        "symbol_usage_by_group": {f"{s}/{g}": dict(c) for (s, g), c in sorted(by_group.items())},
        "static_candidates": static_candidates,
        "most_redeclared_externs": [{"symbol": k, "files": n} for n, k in dup_externs[:200]],
    }, indent=1) + "\n")

    print("filenames:")
    for scope in ("main", "overlays"):
        row = "  ".join(f"{k} {totals[(scope, k)]}" for k in ("address", "suffixed", "placeholder", "named"))
        print(f"  {scope:8s} {row}")
    print("file-scope extern declarations in .c files:")
    for scope in ("main", "overlays"):
        row = "  ".join(f"{k} {totals[(scope, 'extern_' + k)]}" for k in
                        ("addr_data", "addr_func", "named_data", "named_func"))
        print(f"  {scope:8s} {row}")
    print(f"distinct extern names declared in more than one .c file: {len(dup_externs)}")
    print("  most repeated: " + ", ".join(f"{k}({n})" for n, k in dup_externs[:12]))
    print()
    print("per group (files address/suffixed/placeholder/named | D_ externs | func_ externs):")
    groups = sorted(files, key=lambda k: (k[0], -sum(files[k].values())))
    for key in groups:
        c, e = files[key], extern_decls.get(key, {})
        if key[0] == "overlays" and sum(c.values()) < 30:
            continue
        print(f"  {key[0]:8s} {key[1]:12s} {c['address']:5d} {c['suffixed']:5d} {c['placeholder']:4d} "
              f"{c['named']:5d} | {e.get('addr_data', 0):5d} | {e.get('addr_func', 0):5d}")
    if built:
        print()
        print("address-named symbols referenced by compiled TUs (undefined in the TU):")
        for k, v in sorted(usage.items()):
            print(f"  {k:45s} {v}")
        print(f"D_ static candidates (one C TU, symbol owned by that TU's module): {len(static_candidates)}")
    else:
        print("\n(no built objects: symbol usage skipped)")
    print(f"\nmanifest: {out.relative_to(L.ROOT)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
