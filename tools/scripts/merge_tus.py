#!/usr/bin/env python3
"""Merge adjacent single-function C files into larger translation units.

Original .obj boundaries are visible as pad/data subsegments in the splat
config; within one boundary-delimited group, consecutive C files sharing a
directory prefix and identical CC1/MASPSX flags are concatenated into one
TU. A group is cut early when two files declare the same identifier with
different text (typedef/struct/extern conflicts).

Usage: merge_tus.py [--apply] [--min 2]
Without --apply it only prints the plan.
"""
from __future__ import annotations

import argparse
import pathlib
import re
import subprocess
import sys
import tempfile

ROOT = pathlib.Path(__file__).resolve().parents[2]
YAML = ROOT / "configs/USA/main.yaml"
SRC = ROOT / "src/main"

FLAG_RE = re.compile(r"/\*\s*(CC1_FLAGS|MASPSX_FLAGS):([^*]*)\*/")
FUNC_START_RE = re.compile(
    r"^[A-Za-z_][\w \t\*]*?\b[A-Za-z_]\w*\s*\([^;{}]*\)\s*\{", re.M)


def parse_rows():
    t = YAML.read_text()
    return sorted(
        (int(m.group(1), 16), m.group(2), (m.group(3) or "").strip())
        for m in re.finditer(r"- \[(0x[0-9A-Fa-f]+), ([\w.]+)(?:, ([^\]]+))?\]", t)
    )


def groups_between_boundaries(rows):
    groups, cur = [], []
    for o, ty, n in rows:
        if ty in ("c", "asm"):
            cur.append((o, ty, n))
        elif ty != ".rodata":  # rodata windows do not break code adjacency
            if cur:
                groups.append(cur)
            cur = []
    if cur:
        groups.append(cur)
    return groups


def file_flags(text: str):
    cc1, mas = "", ""
    for kind, val in FLAG_RE.findall(text):
        if kind == "CC1_FLAGS":
            cc1 = " ".join(sorted(val.split()))
        else:
            mas = " ".join(sorted(val.split()))
    return cc1, mas


def split_header_body(text: str):
    """Header = everything before the first function definition."""
    stripped = FLAG_RE.sub("", text)
    m = FUNC_START_RE.search(stripped)
    if not m:
        return None, None
    return stripped[: m.start()], stripped[m.start():]


DECL_NAME_RES = [
    re.compile(r"^[A-Za-z_][\w \t\*]*?\b(\w+)\s*\([^;{]*\)\s*;", re.M),  # prototype

    re.compile(r"^typedef[^;{]*?\b(\w+)\s*;", re.M),          # one-line typedef
    re.compile(r"^\}\s*(\w+)\s*;", re.M),                     # struct typedef end
    re.compile(r"^typedef\s+(?:struct|union|enum)\s+(\w+)", re.M),
    re.compile(r"^extern\s[^;(]*?\b(\w+)(?:\[[^\]]*\])?\s*(?:__asm__\([^)]*\))?\s*;", re.M),
]


def header_chunks(header: str):
    """Split a header into blank-line chunks keyed by declared names."""
    chunks = []
    for raw in re.split(r"\n\s*\n", header):
        c = raw.strip()
        if not c:
            continue
        names = set()
        for r in DECL_NAME_RES:
            names.update(r.findall(c))
        chunks.append((c, frozenset(names)))
    return chunks


def norm(s: str) -> str:
    return re.sub(r"\s+", " ", s).strip()


class Unit:
    def __init__(self):
        self.files = []       # (offset, kind, tu_name)
        self.cc1 = None
        self.mas = set()
        self.chunks = []      # ordered (text, names)
        self.seen_norm = set()
        self.name_owner = {}  # identifier -> normalized chunk
        self.bodies = []
        self.windows = []     # rodata island positions owned by members

    def pop_last(self):
        self.files.pop()
        self.bodies.pop()
        n, seen, owner, mas, cc1 = self._snap
        del self.chunks[n:]
        self.seen_norm = seen
        self.name_owner = owner
        self.mas = mas
        self.cc1 = cc1 if any(k == "c" for _, k, _ in self.files) else None

    def add_asm(self, off, tu):
        self.files.append((off, "asm", tu))
        self.bodies.append(("ASM", tu))

    def try_add(self, off, tu, path):
        self._snap = (len(self.chunks), set(self.seen_norm),
                      dict(self.name_owner), set(self.mas), self.cc1)
        text = path.read_text()
        cc1, mas = file_flags(text)
        if self.cc1 is not None and (cc1 != self.cc1 or set(mas.split()) != self.mas):
            return False
        header, body = split_header_body(text)
        if header is None:
            return False
        new_chunks = []
        for c, names in header_chunks(header):
            nc = norm(c)
            for ident in names:
                owner = self.name_owner.get(ident)
                if owner is not None and owner != nc:
                    return False  # conflicting redefinition
            if nc in self.seen_norm:
                continue
            new_chunks.append((c, names, nc))
        # commit
        if self.cc1 is None:
            self.cc1 = cc1
        self.mas = set(mas.split())
        for c, names, nc in new_chunks:
            self.chunks.append(c)
            self.seen_norm.add(nc)
            for ident in names:
                self.name_owner.setdefault(ident, nc)
        self.bodies.append(("C", body.strip()))
        self.files.append((off, "c", tu))
        return True

    def render(self, new_tu: str) -> str:
        out = []
        if self.cc1:
            out.append(f"/* CC1_FLAGS: {self.cc1} */")
        if self.mas:
            out.append(f"/* MASPSX_FLAGS: {' '.join(sorted(self.mas))} */")
        if any(k == "ASM" for k, _ in self.bodies):
            out.append('#include "include_asm.h"')
        if out:
            out.append("")
        out.append("\n\n".join(self.chunks))
        out.append("")
        rendered = []
        for kind, val in self.bodies:
            if kind == "ASM":
                fn = val.split("/")[-1]
                rendered.append(
                    f'INCLUDE_ASM("asm/USA/main/nonmatchings/{new_tu}", {fn});')
            else:
                rendered.append(val)
        out.append("\n\n".join(rendered))
        return "\n".join(out) + "\n"


def compiles(unit) -> bool:
    src = unit.render("test/test")
    with tempfile.NamedTemporaryFile("w", suffix=".c", dir="/tmp", delete=False) as f:
        f.write(src)
        tmp = f.name
    r = subprocess.run(
        [str(ROOT / "tools/scripts/cc.sh"), tmp, tmp + ".o", "-DPERMUTER"],
        capture_output=True, cwd=ROOT)
    pathlib.Path(tmp).unlink(missing_ok=True)
    pathlib.Path(tmp + ".o").unlink(missing_ok=True)
    return r.returncode == 0


def rodata_index(rows):
    """Island position index for every TU owning .rodata windows."""
    idx = {}
    pos = 0
    for _, ty, n in rows:
        if ty in (".rodata", "rodata"):
            idx.setdefault(n, []).append(pos)
            pos += 1
    return idx


def gap_cut_tus():
    """TU names whose object is followed by linker fill in the good map -
    an invisible original file boundary; a unit must not extend past them."""
    mp = ROOT / "expected/build/USA/main.map"
    if not mp.exists():
        return set()
    objs = []
    for m in re.finditer(r"^ \.text\s+0x(8[0-9a-f]{7})\s+0x([0-9a-f]+)\s+(\S+)$",
                         mp.read_text(), re.M):
        objs.append((int(m.group(1), 16), int(m.group(2), 16), m.group(3)))
    objs.sort()
    cuts = set()
    for (a, sz, o), (b, _, _) in zip(objs, objs[1:]):
        if a + sz != b and "/src/main/" in o:
            name = o.split("/src/main/")[-1]
            cuts.add(name[:-4] if name.endswith(".c.o") else name)
    return cuts


def plan_units(min_files: int):
    rows = parse_rows()
    ridx = rodata_index(rows)
    cuts = gap_cut_tus()
    units = []
    for g in groups_between_boundaries(rows):
        i = 0
        while i < len(g):
            o, ty, n = g[i]
            if ty != "c":
                i += 1
                continue
            unit = Unit()
            prefix = n.split("/")[0]
            j = i
            while j < len(g):
                oj, tyj, nj = g[j]
                if nj.split("/")[0] != prefix:
                    break
                new_w = ridx.get(nj, [])
                if new_w and unit.windows:
                    break  # only one member may own rodata windows
                if tyj == "asm":
                    unit.add_asm(oj, nj)
                    unit.windows += new_w
                    j += 1
                    continue  # asm .s reproduces its own trailing pad
                path = (SRC / nj).with_suffix(".c")
                if not path.exists() or not unit.try_add(oj, nj, path):
                    break
                if len(unit.files) > 1 and not compiles(unit):
                    unit.pop_last()
                    break
                unit.windows += new_w
                j += 1
                if nj in cuts:
                    break  # linker fill follows: original file boundary
            # drop trailing asm members (do not extend a unit past its last C file)
            while unit.files and unit.files[-1][1] == "asm":
                unit.files.pop()
                unit.bodies.pop()
            n_c = sum(1 for f in unit.files if f[1] == "c")
            if n_c >= 1 and len(unit.files) >= min_files and unit.files[0][1] == "c":
                units.append(unit)
                i = i + len(unit.files)
            else:
                i = max(j, i + 1)
    return units


def apply_units(units):
    yaml_text = YAML.read_text()
    for u in units:
        first_off = u.files[0][0]
        prefix = u.files[0][2].split("/")[0]
        new_tu = f"{prefix}/tu_{first_off:06X}"
        new_path = (SRC / new_tu).with_suffix(".c")
        new_path.write_text(u.render(new_tu))
        for k, (off, kind, tu) in enumerate(u.files):
            row = f"- [0x{off:X}, {kind}, {tu}]"
            assert row in yaml_text, row
            yaml_text = yaml_text.replace(
                row, f"- [0x{off:X}, c, {new_tu}]" if k == 0 else "\x00", 1)
            for sec in (".rodata", ".data", ".sdata", ".sbss", ".bss"):
                yaml_text = yaml_text.replace(f", {sec}, {tu}]", f", {sec}, {new_tu}]")
            if kind == "c":
                path = (SRC / tu).with_suffix(".c")
                park = ROOT / "local/merged-away" / tu
                park.parent.mkdir(parents=True, exist_ok=True)
                path.rename(park.with_suffix(".c"))
        yaml_text = "\n".join(l for l in yaml_text.split("\n") if "\x00" not in l)
    YAML.write_text(yaml_text)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--apply", action="store_true")
    ap.add_argument("--min", type=int, default=2)
    args = ap.parse_args()
    units = plan_units(args.min)
    total = sum(len(u.files) for u in units)
    for u in units:
        names = " ".join(("*" if k == "asm" else "") + t.split("/")[-1]
                         for _, k, t in u.files)
        print(f"0x{u.files[0][0]:06X} {len(u.files):3} files "
              f"[cc1='{u.cc1 or ''}' mas='{' '.join(sorted(u.mas))}'] {names[:110]}")
    print(f"-- {len(units)} units from {total} files "
          f"(net -{total - len(units)} files)", file=sys.stderr)
    if args.apply:
        apply_units(units)
        print("applied", file=sys.stderr)


if __name__ == "__main__":
    main()
