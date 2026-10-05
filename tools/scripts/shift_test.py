#!/usr/bin/env python3
"""Shifted-build acceptance test (prototype) for the main executable.

Links main twice from the objects of an existing `make build`, both times
into a scratch directory with --emit-relocs: once with the stock linker
script, and once with PAD bytes inserted at the start of `.main`, before the
first unit. Nothing in the repository is written.

Every 32-bit word of the base image is then compared with the word that holds
the same content in the shifted image (address + PAD):

* a relocated word that changed by the shift is an explained address field;
* a relocated word that did NOT change refers to a symbol the linker pins to
  an absolute address (it now points at the wrong bytes);
* an unrelocated word that looks like a program address (data word, `lui`
  upper half, j/jal target) is a literal address the compiler or assembler
  baked in: a hidden fixed pointer;
* any other difference is unexplained and listed.

Sections placed at a fixed VMA in the linker script cannot follow the shift.
The test first links with the script unchanged to show which sections pin the
layout, then rebases those sections (and `_gp`) by PAD in the scratch copy so
the comparison can run over the whole image.
"""
from __future__ import annotations

import argparse
import re
import subprocess
import sys
import tempfile
from bisect import bisect_right
from collections import Counter, defaultdict
from dataclasses import dataclass
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT))
from tools.scripts.shift_audit import (  # noqa: E402
    BUILD, LINKERS, MAIN_START, Layout, address_region, in_program, lui_imm,
    parse_map_sections, tool)

LD = tool("mipsel-none-elf-ld")
OBJCOPY = tool("mipsel-none-elf-objcopy")
FIXED_VMA_RE = re.compile(r"^(\s*)(\.[\w.]+)\s+(0x[0-9A-Fa-f]+)(\s*(?:\(NOLOAD\))?\s*:)", re.M)
GP_RE = re.compile(r"^(\s*_gp\s*=\s*)(0x[0-9A-Fa-f]+)(\s*;)", re.M)


def pad_script(text: str, pad: int, rebase: bool) -> tuple[str, list[str]]:
    """Insert PAD at the start of .main; optionally rebase fixed VMAs and _gp."""
    marker = re.search(r"^(\s*\.main\s+0x[0-9A-Fa-f]+\s*:[^\n]*\n\s*\{\n\s*FILL\([^)]*\);\n)", text, re.M)
    if not marker:
        raise SystemExit("could not find the start of .main in the linker script")
    text = text[:marker.end()] + f"        . += 0x{pad:X}; /* shift_test padding */\n" + text[marker.end():]
    pinned = []
    for m in FIXED_VMA_RE.finditer(text):
        if m.group(2) != ".main":
            pinned.append(f"{m.group(2)} @ {m.group(3)}")
    gp = GP_RE.search(text)
    if gp:
        pinned.append(f"_gp = {gp.group(2)}")
    if rebase:
        def move(m):
            if m.group(2) == ".main":
                return m.group(0)
            return f"{m.group(1)}{m.group(2)} ({m.group(3)} + 0x{pad:X}){m.group(4)}"
        text = FIXED_VMA_RE.sub(move, text)
        text = GP_RE.sub(lambda m: f"{m.group(1)}({m.group(2)} + 0x{pad:X}){m.group(3)}", text)
    return text, pinned


def link(script: Path, out: Path, extra_scripts: list[Path]) -> subprocess.CompletedProcess:
    cmd = [LD, "-EL", "-q", "-T", str(script)]
    for s in extra_scripts:
        cmd += ["-T", str(s)]
    cmd += ["-Map", str(out.with_suffix(".map")), "-o", str(out)]
    return subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True)


@dataclass
class Image:
    data: bytes
    base: int
    relocs: dict[int, tuple[str, str, int]]  # addr -> (type, symbol, symbol value)
    symbols: list[tuple[int, str]]
    sections: list[tuple[int, int, str]]

    def word(self, addr: int) -> int | None:
        off = addr - self.base
        if 0 <= off <= len(self.data) - 4:
            return int.from_bytes(self.data[off:off + 4], "little")
        return None

    def symbol_at(self, addr: int) -> str:
        if not hasattr(self, "_keys"):
            self._keys = [a for a, _ in self.symbols]
        i = bisect_right(self._keys, addr) - 1
        if i < 0:
            return "?"
        a, name = self.symbols[i]
        return f"{name}+0x{addr - a:X}" if addr != a else name


def load_image(elf_path: Path) -> Image:
    from elftools.elf.elffile import ELFFile
    from elftools.elf.relocation import RelocationSection

    with open(elf_path, "rb") as fh:
        elf = ELFFile(fh)
        symtab = elf.get_section_by_name(".symtab")
        chunks = []
        sections = []
        for sec in elf.iter_sections():
            if sec["sh_type"] == "SHT_PROGBITS" and sec["sh_flags"] & 2 and sec["sh_addr"] >= MAIN_START:
                chunks.append((sec["sh_addr"], sec.data()))
                sections.append((sec["sh_addr"], sec["sh_addr"] + sec["sh_size"], sec.name))
        chunks.sort()
        base = chunks[0][0]
        end = max(a + len(d) for a, d in chunks)
        buf = bytearray(end - base)
        for a, d in chunks:
            buf[a - base:a - base + len(d)] = d
        relocs = {}
        for sec in elf.iter_sections():
            if not isinstance(sec, RelocationSection):
                continue
            for rel in sec.iter_relocations():
                sym = symtab.get_symbol(rel["r_info_sym"])
                name = sym.name or elf.get_section(sym["st_shndx"]).name if isinstance(sym["st_shndx"], int) else sym.name
                kind = "ABS" if sym["st_shndx"] == "SHN_ABS" else "REL"
                relocs[rel["r_offset"]] = (rel["r_info_type"], name or "?", sym["st_value"], kind)
        symbols = sorted((s["st_value"], s.name) for s in symtab.iter_symbols()
                         if s.name and s["st_shndx"] not in ("SHN_ABS", "SHN_UNDEF")
                         and s["st_info"]["type"] in ("STT_FUNC", "STT_OBJECT", "STT_NOTYPE")
                         and MAIN_START <= s["st_value"] < end and not s.name.startswith((".L", "$"))
                         and s.name not in ("gcc2_compiled.", "__gnu_compiled_c"))
    return Image(bytes(buf), base, relocs, symbols, sections)


R_MIPS_32, R_MIPS_26, R_MIPS_HI16, R_MIPS_LO16, R_MIPS_GPREL16 = 2, 4, 5, 6, 7


def explained(rtype: int, wb: int, ws: int, pad: int) -> bool:
    if rtype == R_MIPS_32:
        return (ws - wb) & 0xFFFFFFFF == pad
    if rtype == R_MIPS_26:
        return ((ws - wb) & 0x03FFFFFF) == (pad >> 2) & 0x03FFFFFF and (ws ^ wb) >> 26 == 0
    if rtype == R_MIPS_HI16:
        d = ((ws & 0xFFFF) - (wb & 0xFFFF)) & 0xFFFF
        return (ws ^ wb) >> 16 == 0 and d in ((pad >> 16) & 0xFFFF, ((pad >> 16) + 1) & 0xFFFF)
    if rtype == R_MIPS_LO16:
        return (ws ^ wb) >> 16 == 0 and ((ws - wb) & 0xFFFF) == pad & 0xFFFF
    return False


def address_shaped(word: int, addr: int, is_text: bool) -> str | None:
    if is_text:
        hi = lui_imm(word)
        if hi is not None and 0x8001 <= hi <= 0x8020:
            return "lui"
        if (word >> 26) in (2, 3) and word & 0x03FFFFFF:
            if in_program((addr & 0xF0000000) | ((word & 0x03FFFFFF) << 2)):
                return "jump"
        return None
    return "data_word" if in_program(word) else None


def compare(base: Image, shifted: Image, pad: int, layout: Layout):
    findings = defaultdict(list)
    counts = Counter()
    for off in range(0, len(base.data) - 3, 4):
        a = base.base + off
        wb = base.word(a)
        ws = shifted.word(a + pad)
        if ws is None:
            counts["missing_in_shifted"] += 1
            continue
        sec = layout.find(a)
        is_text = sec is not None and sec.section.startswith(".text") and \
            not ("/asm/" in sec.obj and "/data/" in sec.obj)
        rel = base.relocs.get(a)
        where = (a, base.symbol_at(a), sec.obj if sec else "?")
        if rel is not None:
            rtype, sym, value, kind = rel
            target_fixed = kind == "ABS"
            if wb == ws:
                if rtype == R_MIPS_GPREL16:
                    counts["gprel_unchanged"] += 1  # gp and target moved together
                elif target_fixed and address_region(value) is None:
                    counts["pinned_symbol_reference"] += 1
                    findings["pinned_symbol_reference"].append(where + (sym,))
                elif target_fixed:
                    counts["hardware_reference"] += 1
                elif rtype in (R_MIPS_LO16, R_MIPS_HI16):
                    counts["unchanged_half"] += 1  # e.g. hi16 below a 64K carry
                else:
                    counts["relocated_unchanged_other"] += 1
                    findings["relocated_unchanged_other"].append(where + (sym,))
            elif rtype == R_MIPS_GPREL16:
                counts["gprel_to_pinned"] += 1
                findings["gprel_to_pinned"].append(where + (sym,))
            elif explained(rtype, wb, ws, pad):
                counts["shifted_address_field"] += 1
            else:
                counts["unexplained_relocated"] += 1
                findings["unexplained_relocated"].append(where + (f"type {rtype} {sym} {wb:08X}->{ws:08X}",))
        else:
            shape = address_shaped(wb, a, is_text)
            if wb == ws:
                if shape:
                    counts[f"literal_{shape}"] += 1
                    findings[f"literal_{shape}"].append(where + (f"{wb:08X}",))
                else:
                    counts["unchanged"] += 1
            else:
                counts["unexplained_difference"] += 1
                findings["unexplained_difference"].append(where + (f"{wb:08X}->{ws:08X}",))
    return counts, findings


def run(pad: int, scratch: Path, verbose: int) -> int:
    script = LINKERS / "main.ld"
    extra = [LINKERS / "undefined_syms_manual.txt", LINKERS / "undefined_addr_aliases.main.txt"]
    for need in [script, *extra, BUILD / "main.map"]:
        if not need.exists():
            sys.exit(f"missing {need}: run `make build` first")
    text = script.read_text()

    base_elf = scratch / "base.elf"
    res = link(script, base_elf, extra)
    if res.returncode:
        sys.exit("base link failed:\n" + res.stderr)
    subprocess.run([OBJCOPY, "-O", "binary", str(base_elf), str(scratch / "base.exe")], check=True)
    retail = (BUILD / "main.exe").read_bytes()
    same = (scratch / "base.exe").read_bytes() == retail
    print(f"base relink identical to build/USA/main.exe: {same}")

    # 1. shift with the layout as written: which sections pin it?
    plain, pinned = pad_script(text, pad, rebase=False)
    (scratch / "plain.ld").write_text(plain)
    res = link(scratch / "plain.ld", scratch / "plain.elf", extra)
    print(f"fixed placements in the script: {', '.join(pinned) or 'none'}")
    if res.returncode:
        errors = [l for l in res.stderr.splitlines() if "error" in l or "overlap" in l]
        print("shift with the script unchanged: link FAILS")
        for line in errors[:6]:
            print(f"  {line.strip()}")
    else:
        print("shift with the script unchanged: links")

    # 2. shift with the fixed placements rebased in the scratch copy.
    rebased, _ = pad_script(text, pad, rebase=True)
    (scratch / "shifted.ld").write_text(rebased)
    res = link(scratch / "shifted.ld", scratch / "shifted.elf", extra)
    if res.returncode:
        print("shifted link failed:\n" + "\n".join(res.stderr.splitlines()[:20]))
        return 2
    base = load_image(base_elf)
    shifted = load_image(scratch / "shifted.elf")
    layout = Layout(parse_map_sections((scratch / "base.map").read_text()))
    counts, findings = compare(base, shifted, pad, layout)

    print(f"\nshifted by 0x{pad:X}: compared {sum(counts.values())} words")
    for k, v in sorted(counts.items(), key=lambda kv: -kv[1]):
        print(f"  {k:28} {v:7}")

    hidden = ["pinned_symbol_reference", "gprel_to_pinned", "literal_data_word", "literal_lui",
              "literal_jump", "unexplained_relocated", "unexplained_difference",
              "relocated_unchanged_other"]
    for key in hidden:
        items = findings.get(key)
        if not items:
            continue
        by_file = Counter(Path(obj).as_posix().replace("build/USA/", "") for _, _, obj, _ in items)
        print(f"\n{key}: {len(items)} words in {len(by_file)} objects")
        for obj, n in by_file.most_common(verbose or 10):
            print(f"  {n:6}  {obj}")
        if key == "pinned_symbol_reference":
            syms = Counter(s for _, _, _, s in items)
            print(f"  {len(syms)} distinct pinned symbols; most used:")
            for s, n in syms.most_common(verbose or 10):
                print(f"  {n:6}  {s}")
        else:
            for a, sym, obj, info in items[:verbose or 10]:
                print(f"    {a:08X} {sym:40} {info}")

    # The header is not part of an allocated program section.
    header = retail[:0x800]
    pc = int.from_bytes(header[0x10:0x14], "little")
    size = int.from_bytes(header[0x1C:0x20], "little")
    print(f"\nPS-X EXE header (asm/USA/main/header.s): initial PC 0x{pc:08X} and text size 0x{size:X}"
          " are literals; they do not follow the shift (size-dependent fields).")
    return 0


def main(argv=None) -> int:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--pad", type=lambda s: int(s, 0), default=0x10,
                        help="bytes inserted before the first unit (multiple of 4, default 0x10)")
    parser.add_argument("--scratch", type=Path, help="keep the scratch links here")
    parser.add_argument("-v", "--verbose", type=int, default=0, help="rows per listing")
    args = parser.parse_args(argv)
    if args.pad % 4:
        parser.error("--pad must be a multiple of 4")
    if args.scratch:
        args.scratch.mkdir(parents=True, exist_ok=True)
        return run(args.pad, args.scratch, args.verbose)
    with tempfile.TemporaryDirectory(prefix="shift_test_") as tmp:
        return run(args.pad, Path(tmp), args.verbose)


if __name__ == "__main__":
    sys.exit(main())
