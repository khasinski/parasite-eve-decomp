#!/usr/bin/env python3
"""Inventory every source of fixed program addresses (shiftability audit).

Read-only. It inspects the linker symbol files, the sources, the built main
executable and the built overlays, and prints one table per area:

  (a) absolute symbol assignments under linkers/USA, classified by what the
      address is: hardware, kernel, an overlay load window, a function, data
      owned by a C unit, data inside an anonymous asm blob, or nothing built;
  (b) fixed addresses written in C: address-named alias declarations,
      integer-to-pointer casts of program addresses, literal program
      addresses and the pointer-integer casts counted by the crutch ratchet;
  (c) words of the built main executable that hold a program address but
      carry no relocation (found by relinking main with --emit-relocs into a
      scratch directory), and relocations that resolve to absolute symbols;
  (d) how the overlays learn main's addresses;
  (e) layout that is fixed in the linker script or the executable header.

`--json` prints the same inventory as JSON. `--check BASELINE` fails when a
counted category grows past the JSON baseline (for later CI use).
"""
from __future__ import annotations

import argparse
import json
import re
import shutil
import subprocess
import sys
import tempfile
from bisect import bisect_right
from collections import Counter, defaultdict
from dataclasses import dataclass, field
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
VERSION = "USA"
LINKERS = ROOT / "linkers" / VERSION
BUILD = ROOT / "build" / VERSION
OVERLAY_CONFIGS = ROOT / "configs" / VERSION / "overlays"


def tool(name: str) -> str:
    """A binutils program from PATH, else from the repository's toolchain."""
    found = shutil.which(name)
    if found:
        return found
    local = ROOT / "tools" / "binutils-2.45" / "bin" / name
    return str(local) if local.exists() else name


LD = tool("mipsel-none-elf-ld")
NM = tool("mipsel-none-elf-nm")

MAIN_START = 0x80010000
MAIN_END = 0x801FE000  # end of the PS-X EXE image (text size 0x1EE000)

ASSIGN_RE = re.compile(
    r"^\s*(PROVIDE(?:_HIDDEN)?\s*\(\s*)?([A-Za-z_.$][\w.$]*)\s*=\s*([^;]+?)\s*;",
    re.MULTILINE)
ADDRESS_NAME_RE = re.compile(r"^(?:D_|func_|jtbl_|jpt_|jlabel_|L)([0-9A-Fa-f]{8})\b")


# --------------------------------------------------------------------------
# address regions


def address_region(addr: int) -> str | None:
    """Hardware and kernel regions; None for anything else."""
    if 0x1F800000 <= addr < 0x1F800400:
        return "scratchpad"
    if 0x1F000000 <= addr < 0x1F800000 or 0x1F801000 <= addr < 0x1F810000:
        return "hardware"
    if addr >= 0xBFC00000 or 0xFFFE0000 <= addr:
        return "hardware"
    low = addr & 0x1FFFFFFF
    if low < 0x10000 and (addr >> 29) in (0, 4, 5):
        return "kernel"
    return None


def in_program(addr: int) -> bool:
    return MAIN_START <= addr < MAIN_END


# --------------------------------------------------------------------------
# linker symbol files


@dataclass
class Assignment:
    name: str
    value: int | None  # None when the right-hand side is symbolic
    rhs: str
    path: str
    provide: bool = False  # PROVIDE() yields to an object definition


def parse_assignments(text: str, path: str = "") -> list[Assignment]:
    text = re.sub(r"/\*.*?\*/", "", text, flags=re.S)
    out = []
    for match in ASSIGN_RE.finditer(text):
        provide, name, rhs = bool(match.group(1)), match.group(2), match.group(3)
        if provide:
            rhs = rhs.rstrip(") \t")
        try:
            value = int(rhs, 0)
        except ValueError:
            value = None
        out.append(Assignment(name, value, rhs, path, provide))
    return out


# --------------------------------------------------------------------------
# map / ELF layout


@dataclass(order=True)
class InputSection:
    start: int
    size: int
    section: str = field(compare=False)
    obj: str = field(compare=False)


MAP_SECTION_RE = re.compile(
    r"^ (\.[\w.]+)(?:\n)?\s+0x([0-9a-f]{8,16})\s+0x([0-9a-f]+)\s+(\S+\.o)\s*$",
    re.MULTILINE)


def parse_map_sections(text: str) -> list[InputSection]:
    """Input-section placements from a GNU ld map, including wrapped names."""
    sections = []
    for match in MAP_SECTION_RE.finditer(text):
        size = int(match.group(3), 16)
        if size == 0:
            continue
        start = int(match.group(2), 16) & 0xFFFFFFFF
        sections.append(InputSection(start, size, match.group(1), match.group(4)))
    sections.sort()
    return sections


class Layout:
    """Which built input section holds an address."""

    def __init__(self, sections: list[InputSection], image: bytes = b"",
                 image_base: int = MAIN_START):
        self.sections = sorted(sections)
        self.starts = [s.start for s in self.sections]
        self.image = image
        self.image_base = image_base
        self._zero_cache: dict[str, bool] = {}

    def find(self, addr: int) -> InputSection | None:
        i = bisect_right(self.starts, addr) - 1
        if i >= 0:
            sec = self.sections[i]
            if sec.start <= addr < sec.start + sec.size:
                return sec
        return None

    def is_zero_blob(self, sec: InputSection) -> bool:
        if sec.obj not in self._zero_cache:
            off = sec.start - self.image_base
            chunk = self.image[off:off + sec.size] if self.image else b""
            zero = bool(chunk) and chunk.count(0) >= 0.95 * len(chunk)
            self._zero_cache[sec.obj] = zero
        return self._zero_cache[sec.obj]


def location_kind(addr: int, layout: Layout, windows: list[tuple[int, int, str]]) -> str:
    """Classify a program address by what the build places there."""
    region = address_region(addr)
    if region:
        return region
    for lo, hi, _name in windows:
        if lo <= addr < hi:
            return "overlay_window"
    sec = layout.find(addr)
    if sec is None:
        return "outside_image" if not in_program(addr) else "unplaced"
    if sec.section.startswith(".text"):
        if "/asm/" in sec.obj and "/data/" in sec.obj:
            return "data_asm_blob"
        return "function"
    if "/src/" in sec.obj:
        return "data_c_unit"
    if layout.is_zero_blob(sec):
        return "zero_blob"
    return "data_asm_blob"


def overlay_windows() -> list[tuple[int, int, str]]:
    """[vram, vram+size) of every configured overlay segment."""
    windows = []
    for cfg in sorted(OVERLAY_CONFIGS.glob("*.yaml")):
        name = cfg.stem
        text = cfg.read_text()
        target = ROOT / "original" / VERSION / "overlays" / f"{name}.bin"
        total = target.stat().st_size if target.exists() else None
        segs = []
        for m in re.finditer(r"^  - name: \S+\n(?:    .*\n)*?    start: (0x[0-9A-Fa-f]+)\n(?:    .*\n)*?    vram: (0x[0-9A-Fa-f]+)", text, re.M):
            segs.append((int(m.group(1), 16), int(m.group(2), 16)))
        ends = [int(m.group(1), 16) for m in re.finditer(r"^  - \[(0x[0-9A-Fa-f]+)\]\s*$", text, re.M)]
        end = ends[-1] if ends else total
        for i, (start, vram) in enumerate(segs):
            stop = segs[i + 1][0] if i + 1 < len(segs) else end
            if stop is None:
                continue
            windows.append((vram, vram + (stop - start), name))
    return windows


def merge_windows(windows):
    merged = []
    for lo, hi, _ in sorted(windows):
        if merged and lo <= merged[-1][1]:
            merged[-1][1] = max(merged[-1][1], hi)
        else:
            merged.append([lo, hi])
    return [(lo, hi, "overlays") for lo, hi in merged]


def nm_tables(objects: list[Path]) -> tuple[set[str], set[str], set[str]]:
    """(defined, undefined, common) symbol names across objects."""
    defined, undefined, common = set(), set(), set()
    for i in range(0, len(objects), 400):
        out = subprocess.run([NM, *map(str, objects[i:i + 400])], check=True,
                             capture_output=True, text=True).stdout
        for line in out.splitlines():
            parts = line.split()
            if len(parts) == 2 and parts[0] == "U":
                undefined.add(parts[1])
            elif len(parts) == 3 and parts[1] in ("C", "c"):
                common.add(parts[2])
            elif len(parts) == 3:
                defined.add(parts[2])
    return defined, undefined, common


# --------------------------------------------------------------------------
# (a) main linker assignments


def classify_main_assignment(a: Assignment, defined: set[str], referenced: set[str],
                             common: set[str], elf_addrs: set[int],
                             layout: Layout, windows) -> str:
    if a.value is None:
        return "symbolic_alias"
    region = address_region(a.value)
    if region:
        return region
    if a.name in common:
        return "common_pin"
    if a.name in defined:
        return "shadows_definition"
    if a.name not in referenced:
        return "unreferenced"
    kind = location_kind(a.value, layout, windows)
    if kind in ("function", "data_c_unit", "data_asm_blob", "zero_blob") and \
            a.value in elf_addrs:
        # A linked object already names this address: references can be
        # renamed to that symbol (or the object can export this alias).
        kind += "/named_elsewhere"
    return kind


# --------------------------------------------------------------------------
# (b) sources


def strip_comments(text: str) -> str:
    return re.sub(r"//.*", "", re.sub(r"/\*.*?\*/", "", text, flags=re.S))


ALIAS_RE = re.compile(r'\b(?:__asm__|asm)\s*\(\s*"([A-Za-z_][\w.$]*)"\s*\)')
PROGRAM_LITERAL_RE = re.compile(r"\b0x(80[01][0-9A-Fa-f]{5}|A0[01][0-9A-Fa-f]{5})(?:[uUlL]*)\b", re.I)
INT_TO_PTR_RE = re.compile(
    r"\(\s*(?:const\s+|volatile\s+)*[A-Za-z_]\w*(?:\s+\w+)*\s*\*+\s*\)\s*\(?\s*0x(80[01][0-9A-Fa-f]{5})\b",
    re.I)
# Constructs that bind source to an address: aliases onto linker pins and
# literal program addresses. The rest are reported for context only.
FIXED_SOURCE_KEYS = ("alias_to_pinned", "int_to_pointer_literal", "program_literal")
SOURCE_KEYS = FIXED_SOURCE_KEYS + ("alias_address_named_owned", "alias_named_owned",
                                   "pointer_integer_cast")
PTR_INT_CAST_RE = re.compile(r"\(\s*(?:s32|u32|long|unsigned\s+long)\s*\)\s*(?:[A-Za-z_&]|\()")


def scan_source(text: str) -> dict[str, list[str]]:
    """Fixed-address constructs in one C/header text (comments stripped)."""
    text = strip_comments(text)
    found: dict[str, list[str]] = defaultdict(list)
    for m in ALIAS_RE.finditer(text):
        start = max(text.rfind(";", 0, m.start()), text.rfind("{", 0, m.start()),
                    text.rfind("}", 0, m.start()))
        decl = text[start + 1:m.start()]
        if re.search(r"\bregister\b", decl):
            continue  # register pin, not a symbol alias
        target = m.group(1)
        if target.startswith("$") or re.fullmatch(r"\$?(?:[astvk][0-9]|gp|sp|fp|ra|at)", target):
            continue
        found["alias"].append(target)
    cast_spans = []
    for m in INT_TO_PTR_RE.finditer(text):
        found["int_to_pointer_literal"].append("0x" + m.group(1).upper())
        cast_spans.append(m.span())
    for m in PROGRAM_LITERAL_RE.finditer(text):
        if any(lo <= m.start() < hi for lo, hi in cast_spans):
            continue
        value = int(m.group(1), 16)
        if (value & 0x1FFFFFFF) >= 0x10000:
            found["program_literal"].append("0x" + m.group(1).upper())
    found["pointer_integer_cast"].extend(PTR_INT_CAST_RE.findall(text))
    return found


def classify_aliases(found: dict[str, list[str]], pinned: set[str]) -> None:
    """Split alias targets into those the linker pins and those an object owns."""
    for target in found.pop("alias", []):
        if target in pinned:
            found.setdefault("alias_to_pinned", []).append(target)
        elif ADDRESS_NAME_RE.match(target):
            found.setdefault("alias_address_named_owned", []).append(target)
        else:
            found.setdefault("alias_named_owned", []).append(target)


def scan_tree(paths: list[Path], pinned: set[str] = frozenset()) -> tuple[Counter, dict[str, Counter]]:
    totals: Counter = Counter()
    per_file: dict[str, Counter] = {}
    for path in paths:
        found = scan_source(path.read_text(errors="ignore"))
        classify_aliases(found, pinned)
        counts = Counter({k: len(v) for k, v in found.items() if v})
        if counts:
            per_file[str(path.relative_to(ROOT))] = counts
            totals.update(counts)
    return totals, per_file


# --------------------------------------------------------------------------
# (c) unrelocated program addresses in the built executable


def lui_imm(word: int) -> int | None:
    if (word >> 26) == 0x0F:
        return word & 0xFFFF
    return None


def find_unrelocated(data: bytes, base: int, relocated: set[int], text: bool):
    """Yield (addr, word, kind) for address-shaped words lacking a relocation.

    Data: a 32-bit word in the program range. Text: a `lui` whose upper half
    lies in the program range (0x8001..0x8020) or a j/jal into the program.
    """
    for off in range(0, len(data) - 3, 4):
        addr = base + off
        if addr in relocated:
            continue
        word = int.from_bytes(data[off:off + 4], "little")
        if text:
            hi = lui_imm(word)
            if hi is not None and 0x8001 <= hi <= 0x8020:
                yield addr, word, "lui"
            elif (word >> 26) in (2, 3):
                target = (addr & 0xF0000000) | ((word & 0x03FFFFFF) << 2)
                if in_program(target) and word & 0x03FFFFFF:
                    yield addr, word, "jump"
        elif in_program(word):
            yield addr, word, "data_word"


def emit_relocs_link(scratch: Path) -> Path:
    """Relink main with --emit-relocs into scratch; the repository is untouched."""
    out = scratch / "main_relocs.elf"
    cmd = [LD, "-EL", "-q", "-T", str(LINKERS / "main.ld"),
           "-T", str(LINKERS / "undefined_syms_manual.txt"),
           "-T", str(LINKERS / "undefined_addr_aliases.main.txt"),
           "-o", str(out)]
    subprocess.run(cmd, cwd=ROOT, check=True, capture_output=True, text=True)
    return out


def relocation_audit(elf_path: Path, layout: Layout):
    from elftools.elf.elffile import ELFFile
    from elftools.elf.relocation import RelocationSection

    results = {"unrelocated": [], "abs_relocs": Counter(), "abs_reloc_syms": Counter(),
               "abs_reloc_files": Counter()}
    with open(elf_path, "rb") as fh:
        elf = ELFFile(fh)
        symtab = elf.get_section_by_name(".symtab")
        relocated: dict[str, set[int]] = defaultdict(set)
        for sec in elf.iter_sections():
            if not isinstance(sec, RelocationSection):
                continue
            target = elf.get_section(sec["sh_info"])
            for rel in sec.iter_relocations():
                relocated[target.name].add(rel["r_offset"])
                sym = symtab.get_symbol(rel["r_info_sym"])
                if sym["st_shndx"] == "SHN_ABS":
                    value = sym["st_value"]
                    region = address_region(value) or "program"
                    if region == "program":
                        results["abs_relocs"]["program"] += 1
                        results["abs_reloc_syms"][sym.name] += 1
                        src = layout.find(rel["r_offset"])
                        results["abs_reloc_files"][src.obj if src else "?"] += 1
                    else:
                        results["abs_relocs"][region] += 1
        for sec in elf.iter_sections():
            if sec["sh_type"] != "SHT_PROGBITS" or not (sec["sh_flags"] & 2) or sec["sh_addr"] < MAIN_START:
                continue
            data = sec.data()
            base = sec["sh_addr"]
            for addr, word, kind in find_unrelocated(data, base, relocated[sec.name], text=False):
                insec = layout.find(addr)
                is_text = insec is not None and insec.section.startswith(".text") and \
                    not ("/asm/" in insec.obj and "/data/" in insec.obj)
                if is_text:
                    continue
                results["unrelocated"].append((addr, word, kind, insec.obj if insec else "?"))
            for addr, word, kind in find_unrelocated(data, base, relocated[sec.name], text=True):
                insec = layout.find(addr)
                if insec is None or not insec.section.startswith(".text") or \
                        ("/asm/" in insec.obj and "/data/" in insec.obj):
                    continue
                results["unrelocated"].append((addr, word, kind, insec.obj))
    return results


# --------------------------------------------------------------------------
# (d) overlays


def overlay_symbols() -> dict[str, dict[str, int]]:
    """Defined symbols of every built overlay ELF: overlay -> name -> address."""
    from elftools.elf.elffile import ELFFile

    out = {}
    for elf_path in sorted((BUILD / "overlays").glob("*/*.elf")):
        with open(elf_path, "rb") as fh:
            symtab = ELFFile(fh).get_section_by_name(".symtab")
            out[elf_path.stem] = {s.name: s["st_value"] for s in symtab.iter_symbols()
                                  if s.name and s["st_shndx"] not in ("SHN_ABS", "SHN_UNDEF")}
    return out


def classify_overlay_assignment(a: Assignment, overlay: str, referenced: set[str],
                                defined: set[str],
                                own: list[tuple[int, int]], windows,
                                main_syms: dict[str, int], main_addrs: set[int],
                                main_abs: set[str], other: dict[str, int]) -> str:
    if a.value is None:
        return "symbolic"
    if a.name not in referenced:
        return "unreferenced"
    region = address_region(a.value)
    if region:
        return region
    if a.name in defined:
        return "provide_unused" if a.provide else "shadows_own_definition"
    if any(lo <= a.value < hi for lo, hi in own):
        return "own_overlay_pinned"
    if other.get(a.name) == a.value:
        return "other_overlay_symbol"
    if in_program(a.value) and not any(lo <= a.value < hi for lo, hi, _ in windows):
        if main_syms.get(a.name) == a.value:
            return "main_same_name"
        if a.value in main_addrs:
            return "main_other_name"
        if a.name in main_abs:
            return "main_pinned_in_main_too"
        return "main_no_symbol"
    if in_program(a.value):
        return "overlay_window_buffer"
    return "outside_image"


def overlay_audit(main_syms: dict[str, int], windows) -> dict:
    """How each overlay learns main's (and other overlays') addresses."""
    stats = Counter()
    files = Counter()
    per_overlay = {}
    main_addrs = set(main_syms.values())
    main_abs = absolute_symbols()
    ovl_syms = overlay_symbols()
    main_targets: set[int] = set()
    for cfg in sorted(OVERLAY_CONFIGS.glob("*.yaml")):
        name = cfg.stem
        assigns = []
        for kind in ("undefined_funcs_auto", "undefined_syms_auto", "undefined_extra"):
            path = LINKERS / "overlays" / f"{kind}.{name}.txt"
            if path.exists():
                got = parse_assignments(path.read_text(), path.name)
                files[kind] += len(got)
                assigns += got
        objdir = BUILD / "overlays" / name
        objects = sorted(objdir.rglob("*.o")) if objdir.exists() else []
        if not objects:
            stats["overlays_not_built"] += 1
            continue
        defined, undefined, _common = nm_tables(objects)
        own = [(lo, hi) for lo, hi, n in windows if n == name]
        other = {}
        for oname, syms in ovl_syms.items():
            if oname != name:
                for sname, addr in syms.items():
                    other.setdefault(sname, addr)
        counts = Counter()
        for a in assigns:
            kind = classify_overlay_assignment(a, name, undefined, defined, own, windows, main_syms,
                                               main_addrs, main_abs, other)
            counts[kind] += 1
            if kind.startswith("main_"):
                main_targets.add(a.value)
        per_overlay[name] = counts
        stats.update(counts)
        stats["overlays_built"] += 1
    stats["distinct_main_addresses"] = len(main_targets)
    return {"totals": stats, "per_overlay": per_overlay, "files": files}


# --------------------------------------------------------------------------
# (e) fixed layout


def fixed_layout(ld_text: str, header_text: str) -> list[str]:
    items = []
    for m in re.finditer(r"^\s*(\.[\w.]+)\s+(0x[0-9A-Fa-f]+)\s*(?:\(NOLOAD\))?\s*:", ld_text, re.M):
        items.append(f"section {m.group(1)} placed at fixed VMA {m.group(2)}")
    for m in re.finditer(r"^\s*(_gp|[A-Za-z_]\w*)\s*=\s*(0x[0-9A-Fa-f]+)\s*;", ld_text, re.M):
        items.append(f"symbol {m.group(1)} = {m.group(2)} in the linker script")
    pads = len(re.findall(r"^\s*\. \+= 0x", ld_text, re.M))
    if pads:
        items.append(f"{pads} manifest pad gaps (`. += N`), size-only, shift with their neighbours")
    for m in re.finditer(r"^\.word (0x[0-9A-Fa-f]+)\s*/\* (.*?) \*/", header_text, re.M):
        if int(m.group(1), 16):
            items.append(f"PS-X EXE header literal {m.group(1)} ({m.group(2).strip()})")
    return items


# --------------------------------------------------------------------------
# driver


def absolute_symbols() -> set[str]:
    from elftools.elf.elffile import ELFFile

    with open(BUILD / "main.elf", "rb") as fh:
        return {sym.name for sym in ELFFile(fh).get_section_by_name(".symtab").iter_symbols()
                if sym.name and sym["st_shndx"] == "SHN_ABS"
                and address_region(sym["st_value"]) is None}


def object_addresses(names: set[str], layout: Layout) -> dict[str, int]:
    """Where each object would place a symbol the linker script overrides."""
    from elftools.elf.elffile import ELFFile

    starts = {(sec.obj, sec.section): sec.start for sec in layout.sections}
    by_obj = defaultdict(dict)
    for (obj, secname), start in starts.items():
        by_obj[obj][secname] = start
    found = {}
    for obj in by_obj:
        with open(ROOT / obj, "rb") as fh:
            elf = ELFFile(fh)
            symtab = elf.get_section_by_name(".symtab")
            if symtab is None:
                continue
            for sym in symtab.iter_symbols():
                if sym.name in names and isinstance(sym["st_shndx"], int) and \
                        sym["st_info"]["bind"] == "STB_GLOBAL":
                    secname = elf.get_section(sym["st_shndx"]).name
                    if secname in by_obj[obj]:
                        found[sym.name] = by_obj[obj][secname] + sym["st_value"]
    return found


def build_layout() -> tuple[Layout, dict[str, int]]:
    from elftools.elf.elffile import ELFFile

    map_text = (BUILD / "main.map").read_text()
    exe = (BUILD / "main.exe").read_bytes()
    layout = Layout(parse_map_sections(map_text), exe[0x800:], MAIN_START)
    syms = {}
    with open(BUILD / "main.elf", "rb") as fh:
        for sym in ELFFile(fh).get_section_by_name(".symtab").iter_symbols():
            if sym.name and sym["st_shndx"] not in ("SHN_ABS", "SHN_UNDEF"):
                syms[sym.name] = sym["st_value"]
    return layout, syms


def main_objects() -> list[Path]:
    objs = []
    for line in (LINKERS / "main.ld").read_text().splitlines():
        m = re.match(r"\s*(build/\S+\.o)\(", line)
        if m:
            objs.append(ROOT / m.group(1))
    return sorted(set(objs))


def collect(with_relocs: bool = True) -> dict:
    for need in (BUILD / "main.elf", BUILD / "main.map", LINKERS / "main.ld"):
        if not need.exists():
            sys.exit(f"missing {need.relative_to(ROOT)}: run `make build` first")
    layout, main_syms = build_layout()
    windows = overlay_windows()
    merged = merge_windows(windows)
    defined, undefined, common = nm_tables(main_objects())
    main_addrs = set(main_syms.values())

    report: dict = {}

    # (a)
    files = {}
    for name in ("undefined_syms_manual.txt", "undefined_addr_aliases.main.txt",
                 "undefined_syms_auto.main.txt", "undefined_funcs_auto.main.txt"):
        path = LINKERS / name
        if not path.exists():
            continue
        linked = name in ("undefined_syms_manual.txt", "undefined_addr_aliases.main.txt")
        counts = Counter()
        examples = defaultdict(list)
        assigns = parse_assignments(path.read_text(), name)
        shadowed = {a.name for a in assigns if a.value is not None and a.name in defined}
        own = object_addresses(shadowed, layout) if shadowed else {}
        for a in assigns:
            kind = classify_main_assignment(a, defined, undefined, common, main_addrs, layout, merged)
            if kind == "shadows_definition":
                kind = "shadows_same_address" if own.get(a.name) == a.value else "shadows_other_address"
            counts[kind] += 1
            if len(examples[kind]) < 4:
                examples[kind].append(f"{a.name}={a.rhs}")
        files[name] = {"linked": linked, "total": sum(counts.values()),
                       "categories": dict(counts), "examples": dict(examples)}
    report["linker_assignments"] = files

    # (b)
    pinned = absolute_symbols()
    for path in (LINKERS / "overlays").glob("*.txt"):
        pinned |= {a.name for a in parse_assignments(path.read_text()) if a.value is not None}
    main_src = sorted((ROOT / "src" / "main").rglob("*.[ch]")) + sorted((ROOT / "src" / "main").rglob("*.inc"))
    ovl_src = sorted((ROOT / "src" / "overlays").rglob("*.[ch]")) + sorted((ROOT / "src" / "overlays").rglob("*.inc"))
    inc = sorted((ROOT / "include").rglob("*.h"))
    source = {}
    worst = Counter()
    for scope, paths in (("main", main_src), ("overlays", ovl_src), ("include", inc)):
        totals, per_file = scan_tree(paths, pinned)
        source[scope] = dict(totals)
        for f, c in per_file.items():
            worst[f] = sum(v for k, v in c.items() if k in FIXED_SOURCE_KEYS)
    report["source"] = source
    report["source_worst"] = worst.most_common(15)

    # (c)
    if with_relocs:
        with tempfile.TemporaryDirectory(prefix="shift_audit_") as tmp:
            elf = emit_relocs_link(Path(tmp))
            rel = relocation_audit(elf, layout)
        unrel = rel["unrelocated"]
        by_kind = Counter(k for _, _, k, _ in unrel)
        by_obj = Counter(o for _, _, _, o in unrel)
        report["executable"] = {
            "unrelocated": dict(by_kind),
            "unrelocated_worst": [(str(Path(o).relative_to("build/USA")) if o.startswith("build/USA") else o, n)
                                  for o, n in by_obj.most_common(12)],
            "relocs_to_absolute": dict(rel["abs_relocs"]),
            "absolute_symbols_used": len(rel["abs_reloc_syms"]),
            "abs_worst_files": [(str(Path(o).relative_to("build/USA")) if o.startswith("build/USA") else o, n)
                                for o, n in rel["abs_reloc_files"].most_common(12)],
        }

    # (d)
    report["overlays"] = overlay_audit(main_syms, windows)
    report["overlays"]["per_overlay"] = {}  # keep the JSON small

    # (e)
    header = ROOT / "asm" / VERSION / "main" / "header.s"
    report["fixed_layout"] = fixed_layout((LINKERS / "main.ld").read_text(),
                                          header.read_text() if header.exists() else "")
    return report


def render(report: dict) -> str:
    out = []
    out.append("(a) absolute symbol assignments, main")
    out.append(f"  {'file':38} {'linked':6} {'category':22} {'count':>6}")
    for name, info in report["linker_assignments"].items():
        if not info["categories"]:
            out.append(f"  {name:38} {'yes' if info['linked'] else 'no':6} {'(empty)':22} {0:6}")
            continue
        first = True
        for cat, n in sorted(info["categories"].items(), key=lambda kv: -kv[1]):
            out.append(f"  {name if first else '':38} {('yes' if info['linked'] else 'no') if first else '':6} {cat:22} {n:6}")
            first = False
        out.append(f"  {'':38} {'':6} {'TOTAL':22} {info['total']:6}")
    out.append("")
    out.append("(b) fixed addresses in C")
    keys = SOURCE_KEYS
    out.append(f"  {'scope':10} " + " ".join(f"{k:>22}" for k in keys))
    for scope, c in report["source"].items():
        out.append(f"  {scope:10} " + " ".join(f"{c.get(k, 0):22}" for k in keys))
    out.append("  pointer_integer_cast is the crutch-ratchet regex: any (s32)/(u32)/long cast, mostly")
    out.append("  plain integer casts; only the first three columns bind source to an address.")
    out.append("  worst files (" + " + ".join(FIXED_SOURCE_KEYS) + "):")
    for f, n in report["source_worst"]:
        out.append(f"    {n:5}  {f}")
    out.append("")
    if "executable" in report:
        ex = report["executable"]
        out.append("(c) built main executable")
        out.append(f"  unrelocated address-shaped words: {ex['unrelocated']}")
        for o, n in ex["unrelocated_worst"]:
            out.append(f"    {n:5}  {o}")
        out.append(f"  relocations resolving to absolute symbols: {ex['relocs_to_absolute']} "
                   f"({ex['absolute_symbols_used']} distinct program-range symbols)")
        for o, n in ex["abs_worst_files"]:
            out.append(f"    {n:5}  {o}")
        out.append("")
    ov = report["overlays"]
    out.append("(d) overlay -> main references")
    out.append(f"  linker files: {dict(ov['files'])}")
    for k, v in sorted(ov["totals"].items()):
        out.append(f"  {k:32} {v:6}")
    out.append("")
    out.append("(e) fixed layout")
    for item in report["fixed_layout"]:
        out.append(f"  {item}")
    return "\n".join(out)


def flat_counts(report: dict) -> dict[str, int]:
    flat = {}
    for name, info in report["linker_assignments"].items():
        if info["linked"]:
            for cat, n in info["categories"].items():
                flat[f"{name}:{cat}"] = n
    for scope, c in report["source"].items():
        for k, n in c.items():
            flat[f"src:{scope}:{k}"] = n
    if "executable" in report:
        for k, n in report["executable"]["unrelocated"].items():
            flat[f"exe:unrelocated:{k}"] = n
        flat["exe:relocs_to_absolute_program"] = report["executable"]["relocs_to_absolute"].get("program", 0)
    return flat


def main(argv=None) -> int:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--json", action="store_true")
    parser.add_argument("--no-relocs", action="store_true", help="skip the --emit-relocs relink")
    parser.add_argument("--check", type=Path, help="fail if a count exceeds this JSON baseline")
    args = parser.parse_args(argv)
    report = collect(with_relocs=not args.no_relocs)
    if args.json:
        print(json.dumps({"report": report, "counts": flat_counts(report)}, indent=1, default=list))
    else:
        print(render(report))
    if args.check:
        baseline = json.loads(args.check.read_text())
        worse = [(k, v, baseline.get(k, 0)) for k, v in flat_counts(report).items() if v > baseline.get(k, 0)]
        for k, v, b in worse:
            print(f"shift-audit: {k} grew {b} -> {v}", file=sys.stderr)
        return 1 if worse else 0
    return 0


if __name__ == "__main__":
    sys.exit(main())
