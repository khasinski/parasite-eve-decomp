#!/usr/bin/env python3
"""Read-only view of linked PE1 modules for the organization analyses.

A module is the main executable or one overlay after `make build` /
`make overlay-build-all`. For every compiled object the linker map gives the
placement of its sections; the object gives function symbols and relocations;
the linked ELF gives the final instruction words. Relocated operands are
resolved from the linked words, so every reference is reported as an
absolute address independent of how the source spells the symbol.
"""
from __future__ import annotations

import dataclasses
import pathlib
import re
from functools import lru_cache

from elftools.elf.elffile import ELFFile
from elftools.elf.relocation import RelocationSection

ROOT = pathlib.Path(__file__).resolve().parents[2]
BUILD = ROOT / "build" / "USA"
GP_MAIN = 0x8009CD70

R_MIPS_32 = 2
R_MIPS_26 = 4
R_MIPS_HI16 = 5
R_MIPS_LO16 = 6
R_MIPS_GPREL16 = 7

MAP_SECTION = re.compile(
    r"^ (\.[A-Za-z0-9_.]+)?\s+0x([0-9a-fA-F]+)\s+0x([0-9a-fA-F]+)\s+(\S+\.o)$"
)


@dataclasses.dataclass
class Reloc:
    offset: int  # offset inside the function
    rtype: int
    target: int  # resolved absolute address (0 when unknown)
    symbol: str


@dataclasses.dataclass
class Function:
    name: str
    address: int
    size: int
    obj: str  # object path relative to ROOT
    source: str  # source path relative to ROOT ("" for asm objects)
    words: list[int]
    relocs: list[Reloc]


@dataclasses.dataclass
class Module:
    name: str
    elf: pathlib.Path
    map: pathlib.Path
    lo: int
    hi: int
    functions: list[Function]
    # object -> {section name: (address, size)}
    placement: dict[str, dict[str, tuple[int, int]]]

    def contains(self, address: int) -> bool:
        return self.lo <= address < self.hi


def parse_map(path: pathlib.Path) -> dict[str, dict[str, tuple[int, int]]]:
    placement: dict[str, dict[str, tuple[int, int]]] = {}
    pending = None
    for line in path.read_text(errors="replace").splitlines():
        if pending is not None and line.startswith("                "):
            line = " " + pending + line
            pending = None
        else:
            pending = None
            m = re.match(r"^ (\.[A-Za-z0-9_.]+)$", line)
            if m:
                pending = m.group(1)
                continue
        m = MAP_SECTION.match(line)
        if not m or not m.group(1):
            continue
        section, addr, size, obj = m.group(1), int(m.group(2), 16), int(m.group(3), 16), m.group(4)
        if size == 0 or addr == 0:
            continue
        sections = placement.setdefault(obj, {})
        # A wildcard map may list the same input section once; keep the first.
        sections.setdefault(section, (addr, size))
    return placement


class LinkedImage:
    def __init__(self, elf_path: pathlib.Path):
        self.segments = []
        with elf_path.open("rb") as handle:
            elf = ELFFile(handle)
            for section in elf.iter_sections():
                if section["sh_type"] != "SHT_PROGBITS" or not section["sh_addr"]:
                    continue
                self.segments.append((section["sh_addr"], section.data()))

    def word(self, address: int) -> int | None:
        for base, data in self.segments:
            if base <= address and address + 4 <= base + len(data):
                off = address - base
                return int.from_bytes(data[off:off + 4], "little")
        return None


def _sext16(value: int) -> int:
    value &= 0xFFFF
    return value - 0x10000 if value & 0x8000 else value


def _object_functions(obj_path: pathlib.Path):
    """Yield (name, offset, size, relocs) for .text FUNC symbols plus all relocs."""
    with obj_path.open("rb") as handle:
        elf = ELFFile(handle)
        text_index = None
        for index, section in enumerate(elf.iter_sections()):
            if section.name == ".text":
                text_index = index
        if text_index is None:
            return [], []
        symtab = elf.get_section_by_name(".symtab")
        funcs = []
        for sym in symtab.iter_symbols():
            if sym["st_info"]["type"] == "STT_FUNC" and sym["st_shndx"] == text_index:
                funcs.append((sym.name, sym["st_value"], sym["st_size"]))
        relocs = []
        for section in elf.iter_sections():
            if isinstance(section, RelocationSection) and section.name == ".rel.text":
                for rel in section.iter_relocations():
                    sym = symtab.get_symbol(rel["r_info_sym"])
                    relocs.append((rel["r_offset"], rel["r_info_type"], sym.name or f"#{rel['r_info_sym']}"))
        return funcs, relocs


def _source_for(obj: str) -> str:
    # build/USA/src/main/x.c.o, build/USA/overlays/<ov>/src/overlays/<ov>/x.c.o
    marker = "/src/"
    if obj.endswith(".c.o") and marker in obj:
        return "src/" + obj.split(marker, 1)[1][:-2]
    return ""


def load_module(name: str, gp: int = GP_MAIN) -> Module:
    if name == "main":
        elf_path, map_path = BUILD / "main.elf", BUILD / "main.map"
    else:
        elf_path = BUILD / "overlays" / name / f"{name}.elf"
        map_path = BUILD / "overlays" / name / f"{name}.map"
    if not elf_path.exists() or not map_path.exists():
        raise FileNotFoundError(f"{elf_path} missing: build the module first")
    image = LinkedImage(elf_path)
    placement = parse_map(map_path)
    lo = min(base for base, _ in image.segments)
    hi = max(base + len(data) for base, data in image.segments)
    functions: list[Function] = []
    for obj, sections in placement.items():
        if ".text" not in sections:
            continue
        obj_path = ROOT / obj
        if not obj_path.exists():
            continue
        text_addr, _ = sections[".text"]
        funcs, relocs = _object_functions(obj_path)
        # Resolve every relocation against linked words.
        resolved = []
        last_hi: dict[str, int] = {}
        for offset, rtype, sym in relocs:
            addr = text_addr + offset
            word = image.word(addr) or 0
            target = 0
            if rtype == R_MIPS_26:
                target = (addr & 0xF0000000) | ((word & 0x3FFFFFF) << 2)
            elif rtype == R_MIPS_HI16:
                last_hi[sym] = (word & 0xFFFF) << 16
                target = last_hi[sym]
            elif rtype == R_MIPS_LO16:
                target = (last_hi.get(sym, 0) + _sext16(word)) & 0xFFFFFFFF
            elif rtype == R_MIPS_GPREL16:
                target = (gp + _sext16(word)) & 0xFFFFFFFF
            elif rtype == R_MIPS_32:
                target = word
            resolved.append((offset, rtype, target, sym))
        # HI16 targets are only complete once paired; patch from the next LO16.
        for i, (offset, rtype, target, sym) in enumerate(resolved):
            if rtype == R_MIPS_HI16:
                for j in range(i + 1, len(resolved)):
                    if resolved[j][1] == R_MIPS_LO16 and resolved[j][3] == sym:
                        resolved[i] = (offset, rtype, resolved[j][2], sym)
                        break
        source = _source_for(obj)
        for fname, foff, fsize in sorted(funcs, key=lambda f: f[1]):
            if fsize == 0:
                continue
            base = text_addr + foff
            words = [image.word(base + k) or 0 for k in range(0, fsize, 4)]
            frel = [Reloc(o - foff, t, tg, s) for o, t, tg, s in resolved if foff <= o < foff + fsize]
            functions.append(Function(fname, base, fsize, obj, source, words, frel))
    functions.sort(key=lambda f: f.address)
    return Module(name, elf_path, map_path, lo, hi, functions, placement)


@lru_cache(maxsize=None)
def overlay_names() -> tuple[str, ...]:
    configs = sorted((ROOT / "configs" / "USA" / "overlays").glob("*.yaml"))
    return tuple(c.stem for c in configs)


def mask_word(word: int, rtype: int) -> int:
    if rtype == R_MIPS_26:
        return word & 0xFC000000
    if rtype in (R_MIPS_HI16, R_MIPS_LO16, R_MIPS_GPREL16):
        return word & 0xFFFF0000
    if rtype == R_MIPS_32:
        return 0
    return word
