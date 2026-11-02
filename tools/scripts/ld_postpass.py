#!/usr/bin/env python3
"""Checked post-pass over splat's main linker script (run by `make split`).

Splat 0.50 writes main's BSS as a separate NOLOAD output section and places
no small data or COMMON that the manifest does not list. Retail's BSS is zero
bytes inside the PS-X EXE image and the field engine follows main in both the
file and memory, so this pass rewrites `linkers/USA/main.ld`
(docs/SHIFTABILITY_PLAN.md 3.5 and 3.6):

- in-image BSS: the `.bss` inputs of `.main_bss (NOLOAD)` move into `.main`
  after main's data, so `__romPos` (SIZEOF(.main)) covers them and the field
  engine's load and run addresses cannot diverge;
- catch-alls for data that no manifest row or data label places, all empty
  in the retail layout: `*(.sdata)` at the end of .sdata (the manifest's
  `main_sbss` linker_offset row), `*(.sbss)` and `*(.scommon)` at the end of
  the small COMMON pool (`main_scommon`), so that new small data stays in
  the positive $gp window, and `*(COMMON)` after main's BSS;
- startup symbols: `_edata` and `__bss_start` at `main_sbss` (end of .sdata,
  where the startup's BSS clear starts) and `_end` at the end of main, where
  the clear stops and the heap begins; `__sn_ctor_table` and
  `__sn_dtor_table` at the start of `.main`, the constructor and destructor
  tables that snmain's `__main` and `__do_global_dtors` walk (both empty in
  this game, so retail's SN linker put them at the first address of the image);
- an assertion that the field engine has no NOLOAD BSS: its zero data is the
  in-image blob `engine_bss_800E21A8`, followed by Boot_InitMemoryLayout's
  work area (plan 3.4);
- the overlay window block floats: the `overlay_windows` segment follows
  the boot work area in memory and in the file (splat `vram_symbol`), the
  window table `linkers/USA/overlay_windows.txt` is INCLUDEd so main.elf
  defines every window as an offset from `overlay_windows_VRAM`, and the
  link fails when the image reaches the initial stack pointer
  (`PSX_EXE_STACK_TOP`). `PSX_EXE_HEADROOM` is the room left (0x1FF0 in the
  retail layout) and `PSX_EXE_TEXT_SIZE` the header's text size (0x1EE000);
- the PS-X EXE header (`asm/USA/main/header.s`, second argument) takes its
  initial PC (`__SN_ENTRY_POINT`, the startup), text size and initial $sp
  from the link's symbols instead of literals: the BIOS jumps to the initial
  PC, so a literal entry point would start a shifted build in the middle of
  whatever code then lies at the retail address.

The catch-alls come after every explicit input of their kind, because ld
gives an input section to the first statement in the script that matches it.
Every rewrite checks the exact shape it expects and fails otherwise, so a
splat upgrade or a manifest change cannot silently produce a different link.
The pass is idempotent.

Usage: ld_postpass.py [linker_script [header.s]]
"""
from __future__ import annotations

import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
DEFAULT = ROOT / "linkers/USA/main.ld"
DEFAULT_HEADER = ROOT / "asm/USA/main/header.s"
WINDOW_TABLE = "linkers/USA/overlay_windows.txt"
# The BIOS convention for the initial $sp (the header field); the image must
# load below it (docs/SHIFTABILITY_PLAN.md 6).
STACK_TOP = 0x801FFFF0
MARKER = "/* ld_postpass: in-image BSS */"

BSS_BLOCK = re.compile(
    r"    main_bss_VRAM = ADDR\(\.main_bss\);\n"
    r"    \.main_bss \(NOLOAD\) : SUBALIGN\(4\)\n"
    r"    \{\n"
    r"        FILL\(0x00000000\);\n"
    r"        main_BSS_START = \.;\n"
    r"(?P<inputs>(?:        \S+\(\.bss\*\);\n)*)"
    r"        \. = ALIGN\(\., 4\);\n"
    r"        main_BSS_END = \.;\n"
    r"        main_BSS_SIZE = ABSOLUTE\(main_BSS_END - main_BSS_START\);\n"
    r"    \}\n")
DATA_END = re.compile(
    r"(        main_DATA_SIZE = ABSOLUTE\(main_DATA_END - main_DATA_START\);\n)"
    r"(    \}\n)")
ROM_ADVANCE = "    __romPos += SIZEOF(.main);\n"
MAIN_START = re.compile(
    r"    \.main 0x[0-9A-Fa-f]+ : AT\(main_ROM_START\) SUBALIGN\(4\)\n"
    r"    \{\n"
    r"        FILL\(0x00000000\);\n")
SDATA_END = "        main_main_sbss_OFFSET = .;\n"
SCOMMON_END = "        main_main_scommon_OFFSET = .;\n"
ENGINE_BSS_END = ("        field_engine_BSS_SIZE = ABSOLUTE(field_engine_BSS_END - field_engine_BSS_START);\n"
                  "    }\n")
WINDOWS_START = re.compile(
    r"    overlay_windows_ROM_START = __romPos;\n"
    r"    overlay_windows_VRAM = ADDR\(\.overlay_windows\);\n"
    r"    \.overlay_windows (?P<vram>\S+) : AT\(overlay_windows_ROM_START\) SUBALIGN\(4\)\n")
WINDOWS_END = "    overlay_windows_VRAM_END = .;\n"
ENGINE_END = "    field_engine_VRAM_END = .;\n"
HEADER_FIELDS = (
    # (comment splat writes, symbol the link defines)
    ("Initial PC", "__SN_ENTRY_POINT"),
    (".text size", "PSX_EXE_TEXT_SIZE"),
    ("Initial $sp/r29 & $fp/r30 base", "PSX_EXE_STACK_TOP"),
)


class PostPassError(Exception):
    pass


def rewrite(text: str, window_table: str = WINDOW_TABLE) -> str:
    if MARKER in text:
        return text
    blocks = list(BSS_BLOCK.finditer(text))
    if len(blocks) != 1:
        raise PostPassError(f"expected one .main_bss (NOLOAD) block of .bss inputs, found {len(blocks)}")
    block = blocks[0]
    if not text[block.end():].startswith(ROM_ADVANCE):
        raise PostPassError(".main_bss is not followed by the __romPos advance over .main")
    ends = list(DATA_END.finditer(text, 0, block.start()))
    if not ends or ends[-1].end() != block.start():
        raise PostPassError("main_DATA_SIZE does not close .main right before .main_bss")
    for mark, row in ((SDATA_END, "main_sbss"), (SCOMMON_END, "main_scommon")):
        if text.count(mark) != 1:
            raise PostPassError(f"expected one `{row}` linker_offset row in main (configs/USA/main.yaml)")
    if not text.index(SDATA_END) < text.index(SCOMMON_END) < ends[-1].start():
        raise PostPassError("`main_sbss` and `main_scommon` must be in that order inside main's data")
    if text.count(ENGINE_BSS_END) != 1:
        raise PostPassError("expected one .field_engine_bss section")
    main_starts = list(MAIN_START.finditer(text))
    if len(main_starts) != 1:
        raise PostPassError("expected one `.main` section opening with FILL(0x00000000)")
    windows = list(WINDOWS_START.finditer(text))
    if len(windows) != 1:
        raise PostPassError("expected one `overlay_windows` segment (configs/USA/main.yaml)")
    if windows[0].group("vram") != "field_engine_VRAM_END":
        raise PostPassError("the `overlay_windows` segment must follow the engine "
                            "(vram_symbol: field_engine_VRAM_END), not sit at a fixed vram")
    if text.count(WINDOWS_END) != 1 or text.index(WINDOWS_END) < windows[0].end():
        raise PostPassError("expected one overlay_windows_VRAM_END after the `overlay_windows` section")
    if text.count(ENGINE_END) != 1 or not text.index(ENGINE_END) < windows[0].start():
        raise PostPassError("the field engine must end before the `overlay_windows` segment")

    in_image = (
        f"        {MARKER}\n"
        "        main_BSS_START = .;\n"
        f"{block.group('inputs')}"
        "        *(COMMON)\n"
        "        . = ALIGN(., 4);\n"
        "        main_BSS_END = .;\n"
        "        main_BSS_SIZE = ABSOLUTE(main_BSS_END - main_BSS_START);\n"
        "        _end = .;\n")
    # The window block follows the engine; what stays fixed is the layout
    # inside it (the window table) and the stack the image must end below.
    text = text.replace(
        WINDOWS_END,
        WINDOWS_END
        + "    /* ld_postpass: the overlay windows are offsets from the floating block */\n"
        f"    INCLUDE {window_table}\n"
        "    /* PS-X EXE header fields: the image runs from .main to the end of the\n"
        "     * window block and must load below the initial stack pointer. */\n"
        f"    PSX_EXE_STACK_TOP = 0x{STACK_TOP:08X};\n"
        "    PSX_EXE_TEXT_SIZE = ABSOLUTE(overlay_windows_VRAM_END - main_VRAM);\n"
        "    ASSERT(overlay_windows_VRAM_END <= PSX_EXE_STACK_TOP,\n"
        "        \"the image (main, field engine, boot work area, overlay window block) "
        "runs into the initial stack (docs/SHIFTABILITY_PLAN.md 6)\")\n"
        "    ASSERT(overlay_windows_ROM_END - main_ROM_START == PSX_EXE_TEXT_SIZE,\n"
        "        \"the image is not contiguous in the file\")\n"
        "    PSX_EXE_HEADROOM = ABSOLUTE(PSX_EXE_STACK_TOP - overlay_windows_VRAM_END);\n")
    data_end = ends[-1]
    text = (text[:data_end.start()] + data_end.group(1) + in_image + data_end.group(2)
            + "    main_bss_VRAM = main_BSS_START;\n" + text[block.end():])
    text = text.replace(SDATA_END, "        *(.sdata .sdata.*)\n" + SDATA_END
                        + "        _edata = .;\n        __bss_start = .;\n")
    text = text.replace(SCOMMON_END, "        *(.sbss .sbss.*)\n        *(.scommon)\n" + SCOMMON_END)
    text = text.replace(
        ENGINE_BSS_END,
        ENGINE_BSS_END + "    /* No semicolon: ASSERT is a statement inside SECTIONS. */\n"
        "    ASSERT(SIZEOF(.field_engine_bss) == 0,\n"
        "        \"field engine BSS must be in the image, before the work area (docs/SHIFTABILITY_PLAN.md 3.4)\")\n")
    opening = main_starts[0].group(0)
    text = text.replace(opening, opening
                        + "        /* SN constructor and destructor tables (snmain): empty in this game. */\n"
                        "        __sn_ctor_table = .;\n"
                        "        __sn_dtor_table = .;\n", 1)
    return text


def rewrite_header(text: str) -> str:
    """The header's initial PC, text size and initial $sp become the link's symbols."""
    for comment, symbol in HEADER_FIELDS:
        pattern = re.compile(r"^\.word \S+(\s*/\* " + re.escape(comment) + r" \*/)$", re.M)
        if len(pattern.findall(text)) != 1:
            raise PostPassError(f"expected one `{comment}` word in the PS-X EXE header")
        text = pattern.sub(lambda m: f".word {symbol}{m.group(1)}", text)
    return text


def main(argv: list[str]) -> int:
    path = Path(argv[1]) if len(argv) > 1 else DEFAULT
    header = Path(argv[2]) if len(argv) > 2 else DEFAULT_HEADER
    window_table = (f"linkers/{path.parent.name}/overlay_windows.txt"
                    if path.parent.name in ("USA", "JP") else WINDOW_TABLE)
    for target, fn in ((path, lambda t: rewrite(t, window_table)), (header, rewrite_header)):
        try:
            new = fn(target.read_text())
        except PostPassError as exc:
            print(f"ld_postpass: {target}: {exc}", file=sys.stderr)
            return 1
        target.write_text(new)
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
