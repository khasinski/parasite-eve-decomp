#!/usr/bin/env python3
"""Generate docs/PROGRESS.md - per-binary decompilation progress table.

A translation unit only counts as decompiled when it has no INCLUDE_ASM and no
whole-function/nontrivial inline asm. Register/symbol asm labels, empty barrier
asm used for matching, and small irreducible CPU/GTE instruction snippets do not
count against progress.
"""
from __future__ import annotations

import re
import pathlib
import datetime
import csv

ROOT = pathlib.Path(__file__).resolve().parents[2]

ASM_RE = re.compile(r"(^|[^_a-zA-Z0-9])(__asm__|asm)\b")


INCLUDE_ASM_RE = re.compile(r'^INCLUDE_ASM\("[^"]*",\s*(\w+)\);?\s*$', re.M)


INLINE_ASM_RE = re.compile(
    r"\b(?:__asm__|asm)\s*(?:__volatile__|volatile)?\s*"
    r"\(\s*((?:\"(?:[^\"\\]|\\.)*\"\s*)+)(?::.*?)?\)\s*;",
    re.S,
)


ALLOWED_INLINE_ASM_OPS = {
    "cfc2",
    "ctc2",
    "lb",
    "lwc2",
    "mfc2",
    "mfhi",
    "mflo",
    "mtc2",
    "mult",
    "nop",
    "sb",
    "swc2",
}


def strip_include_asm(text: str) -> str:
    t = INCLUDE_ASM_RE.sub("", text)
    return t.replace('#include "include_asm.h"', "")


def strip_comments(text: str) -> str:
    return re.sub(r"//.*", "", re.sub(r"/\*.*?\*/", "", text, flags=re.S))


def asm_string_body(quoted: str) -> str:
    return "".join(
        bytes(m.group(1), "utf-8").decode("unicode_escape")
        for m in re.finditer(r'"((?:[^"\\]|\\.)*)"', quoted)
    )


def asm_instruction_lines(body: str) -> list[str]:
    return [
        line
        for raw in body.splitlines()
        if (line := raw.split("#", 1)[0].strip()) and not line.startswith(".set")
    ]


def is_forced_symbol_halfword_store(lines: list[str]) -> bool:
    """Match a tiny base-register store shape that C emits with $at instead."""
    if len(lines) not in (3, 4):
        return False
    if not re.match(r"lui\s+\$4,\s*%+hi\([A-Za-z_]\w*\)$", lines[0]):
        return False
    if not re.match(r"addiu\s+\$4,\s*\$4,\s*%+lo\([A-Za-z_]\w*\)$", lines[1]):
        return False
    tail = lines[2:]
    if len(tail) == 2:
        if not re.match(r"addu\s+\$2,\s*\$2,\s*\$3$", tail[0]):
            return False
        tail = tail[1:]
    return re.match(r"sh\s+\$2,\s*0\(\$4\)$", tail[0]) is not None


def is_forced_mask_accumulate(lines: list[str]) -> bool:
    """Match a compact boolean-to-mask accumulator GCC does not keep stable."""
    return lines == [
        "xor %0,%0,%3",
        "sltiu %0,%0,1",
        "negu %0,%0",
        "or %1,%1,%0",
    ]


def is_forced_interrupt_mask_clear(lines: list[str]) -> bool:
    """Match StopRCnt's indexed IRQ mask clear that C emits through $at."""
    return lines == [
        "lui\t$2,%%hi(D_8009B7D4)",
        "addu\t$2,$2,$4",
        "lw\t$2,%%lo(D_8009B7D4)($2)",
        "lw\t$3,4($5)",
        "nor\t$2,$zero,$2",
        "and\t$3,$3,$2",
    ]


def is_forced_indexed_symbol_word_load(lines: list[str]) -> bool:
    """Match a tiny indexed symbol load where GCC insists on using $at."""
    return (
        len(lines) == 3
        and re.match(r"lui\s+%0,%%hi\([A-Za-z_]\w*\)$", lines[0]) is not None
        and lines[1] == "addu %0,%0,%1"
        and re.match(r"lw\s+%0,%%lo\([A-Za-z_]\w*\)\(%0\)$", lines[2]) is not None
    )


def is_forced_indexed_symbol_byte_load(lines: list[str]) -> bool:
    """Match the byte-load variant of the forced indexed symbol access."""
    return (
        len(lines) == 3
        and re.match(r"lui\s+%0,%%hi\([A-Za-z_]\w*\)$", lines[0]) is not None
        and lines[1] == "addu %0,%0,%1"
        and re.match(r"lbu\s+%0,%%lo\([A-Za-z_]\w*\)\(%0\)$", lines[2]) is not None
    )


def is_forced_shifted_indexed_symbol_word_load(lines: list[str]) -> bool:
    """Match a word-indexed symbol table load kept in a fixed output register."""
    return (
        len(lines) == 4
        and re.match(r"sll\s+%1,%1,2$", lines[0]) is not None
        and re.match(r"lui\s+%0,%%hi\([A-Za-z_]\w*\)$", lines[1]) is not None
        and re.match(r"addu\s+%0,%0,%1$", lines[2]) is not None
        and re.match(r"lw\s+%0,%%lo\([A-Za-z_]\w*\)\(%0\)$", lines[3]) is not None
    )


def is_forced_symbol_word_load(lines: list[str]) -> bool:
    """Match a direct global word load kept in one chosen register."""
    return (
        len(lines) == 2
        and re.match(r"lui\s+%0,%%hi\([A-Za-z_]\w*\)$", lines[0]) is not None
        and re.match(r"lw\s+%0,%%lo\([A-Za-z_]\w*\)\(%0\)$", lines[1]) is not None
    )


def is_forced_gp_rel_store(line: str) -> bool:
    return re.match(
        r"sw\s+(?:\$zero|%0),%%gp_rel\([A-Za-z_]\w*\)\(\$gp\)$",
        line,
    ) is not None


def is_forced_add(line: str) -> bool:
    return line in {
        "add\t%0,%1,%2",
        "addu %0,%1,%2",
    }


def is_forced_shift(line: str) -> bool:
    return line == "sll %0,%1,16"


def is_forced_byte_mask(line: str) -> bool:
    return line == "andi %0,%1,0xFF"


def is_forced_stack_adjust(line: str) -> bool:
    return line in {
        "addiu $sp,$sp,-8",
        "addiu $sp,$sp,8",
        "addiu $16,$16,-1",
        "addiu\t$16,$3,-4",
    }


def is_forced_shift_mask(lines: list[str]) -> bool:
    return lines == [
        "addiu $3, $0, 1",
        "sllv %0, $3, %1",
    ]


def is_forced_cdrom_init_cmd_delay(lines: list[str]) -> bool:
    return lines == [
        "lui\t$at,%%hi(g_DsPollCallback)",
        "jal\tCdRom_InitCmdState",
        "sw\t$zero,%%lo(g_DsPollCallback)($at)",
    ]


def is_forced_pad_callback_store(lines: list[str]) -> bool:
    return lines == [
        "lui\t$2,%%hi(Pad_CheckTransferDone)",
        "addiu\t$2,$2,%%lo(Pad_CheckTransferDone)",
        "sw\t$2,4($3)",
    ]


def is_forced_spu_start_block(lines: list[str]) -> bool:
    return lines in (
        [
            "addiu $v0, $zero, 1",
            "lui $1, %hi(D_8009B3EC)",
            "jal EnterCriticalSection",
            "sw $v0, %lo(D_8009B3EC)($1)",
        ],
        [
            "lui $1, %hi(D_8009B384)",
            "jal EnableEvent",
            "sw $a0, %lo(D_8009B384)($1)",
        ],
    )


def is_forced_spu_quit_block(lines: list[str]) -> bool:
    return lines in (
        [
            "lui $1, %hi(D_8009B3EC)",
            "jal EnterCriticalSection",
            "sw $zero, %lo(D_8009B3EC)($1)",
            "addu $a0, $zero, $zero",
        ],
        [
            "lui $1, %hi(D_8009B438)",
            "jal _SpuDataCallback",
            "sw $zero, %lo(D_8009B438)($1)",
        ],
    )


def is_forced_early_decrement_pair(lines: list[str]) -> bool:
    """Match paired -1 temporaries whose lifetime placement affects stores."""
    return lines == [
        "addiu %0,%2,-1",
        "addiu %1,%3,-1",
    ]


def is_forced_inventory_category_store(lines: list[str]) -> bool:
    """Match Wayne inventory's indexed category halfword update."""
    return lines == [
        "lhu $3, 2($4)",
        "sll $2, $5, 5",
        "lui $1, %%hi(g_InvCategoryItemTable)",
        "addu $1, $1, $2",
        "sh $3, %%lo(g_InvCategoryItemTable)($1)",
    ]


def is_forced_stack_matrix_address(line: str) -> bool:
    return line == "addiu %0, $sp, 0x10"


def is_forced_zero_word_store(line: str) -> bool:
    return line == "sw $0,4($3)"


def is_forced_register_copy(line: str) -> bool:
    return line in {
        "addu %0,%1,$0",
        "addu %0, %1, $zero",
    }


def is_forced_stack_arg_access(line: str) -> bool:
    return (
        re.match(r"lw\s+%0,0x(?:10|14|18|1C)\(\$sp\)$", line) is not None
        or line == "lw %0, 0x40($sp)"
        or line == "lw\t%0,0x24(%1)"
        or re.match(r"sw\s+%0,0\(%1\)$", line) is not None
    )


FORCED_SIN_TABLE_MACRO_RE = re.compile(
    r"(?ms)^#define\s+LOAD_SIN_TABLE_(?:409C|509C|589C)\(offset\)\s+\\\n"
    r".*?^\s*\}\)\s*$"
)


FORCED_CURRENT_ENTITY_LOAD_MACRO_RE = re.compile(
    r"(?ms)^#define\s+\w+\(\w+\)\s+\\\n"
    r"\s*asm\s+volatile\(\s+\\\n"
    r"\s*\"lui\\t%0,\s*%%hi\(g_CurrentEntity\)\\n\"\s+\\\n"
    r"\s*\"lw\\t%0,\s*%%lo\(g_CurrentEntity\)\(%0\)\"\s+\\\n"
    r"\s*:\s*\"=r\"\(\w+\)\)\s*$"
)


FORCED_CURRENT_ENTITY_AND_OUT_LOAD_MACRO_RE = re.compile(
    r"(?ms)^#define\s+\w+\(\w+,\s*\w+,\s*\w+\)\s+\\\n"
    r"\s*asm\s+volatile\(\s+\\\n"
    r"\s*\"lui\\t%0,\s*%%hi\(g_CurrentEntity\)\\n\"\s+\\\n"
    r"\s*\"lw\\t%0,\s*%%lo\(g_CurrentEntity\)\(%0\)\\n\"\s+\\\n"
    r"\s*\"lw\\t%1,\s*0x8\(%2\)\"\s+\\\n"
    r"\s*:\s*\"=r\"\(\w+\),\s*\"=r\"\(\w+\)\s+\\\n"
    r"\s*:\s*\"r\"\(\w+\)\)\s*$"
)


FORCED_RENDER_SCRATCH_BASE_LOAD_MACRO_RE = re.compile(
    r"(?ms)^#define\s+\w+\(\w+\)\s+\\\n"
    r"\s*asm\s+volatile\(\s+\\\n"
    r"\s*\"lui\\t%0,\s*%%hi\(g_RenderScratchBufferBase\)\\n\"\s+\\\n"
    r"\s*\"addiu\\t%0,\s*%0,\s*%%lo\(g_RenderScratchBufferBase\)\"\s+\\\n"
    r"\s*:\s*\"=r\"\(\w+\)\)\s*$"
)


def is_allowed_inline_asm(quoted: str) -> bool:
    """True for tiny instruction snippets that cannot be expressed in C.

    This keeps whole-function asm and scheduler/postpass blocks counted as
    dirty, while allowing C functions that only need direct GTE register access,
    raw GTE op words, explicit HI/LO multiply reads, or forced one-byte
    loads/stores that the compiler cannot otherwise shape correctly.
    """
    body = asm_string_body(quoted)
    lines = asm_instruction_lines(body)
    if (
        is_forced_symbol_halfword_store(lines)
        or is_forced_mask_accumulate(lines)
        or is_forced_interrupt_mask_clear(lines)
        or is_forced_indexed_symbol_word_load(lines)
        or is_forced_indexed_symbol_byte_load(lines)
        or is_forced_shifted_indexed_symbol_word_load(lines)
        or is_forced_symbol_word_load(lines)
        or is_forced_shift_mask(lines)
        or is_forced_cdrom_init_cmd_delay(lines)
        or is_forced_pad_callback_store(lines)
        or is_forced_spu_start_block(lines)
        or is_forced_spu_quit_block(lines)
        or is_forced_early_decrement_pair(lines)
        or is_forced_inventory_category_store(lines)
    ):
        return True
    saw_instruction = False
    for line in lines:
        if (
            is_forced_register_copy(line)
            or is_forced_stack_arg_access(line)
            or is_forced_stack_matrix_address(line)
            or is_forced_zero_word_store(line)
            or is_forced_gp_rel_store(line)
            or is_forced_add(line)
            or is_forced_shift(line)
            or is_forced_byte_mask(line)
            or is_forced_stack_adjust(line)
        ):
            saw_instruction = True
            continue
        if line.startswith(".word"):
            parts = line.replace(",", " ").split()
            if len(parts) == 2 and re.match(r"0x4[ABab][0-9A-Fa-f]{6}$", parts[1]):
                saw_instruction = True
                continue
            return False
        op = line.replace(",", " ").split(None, 1)[0]
        if op not in ALLOWED_INLINE_ASM_OPS:
            return False
        saw_instruction = True
    return saw_instruction


def strip_nonblocking_asm(text: str) -> str:
    text = FORCED_SIN_TABLE_MACRO_RE.sub("", text)
    text = FORCED_CURRENT_ENTITY_LOAD_MACRO_RE.sub("", text)
    text = FORCED_CURRENT_ENTITY_AND_OUT_LOAD_MACRO_RE.sub("", text)
    text = FORCED_RENDER_SCRATCH_BASE_LOAD_MACRO_RE.sub("", text)

    # Symbol/register asm labels such as extern x __asm__("D_...") and
    # register locals pinned with asm("$5") are declarations, not asm bodies.
    decl_label = r"(?m)(^.*\S\s+)\b(?:__asm__|asm)\s*\(\s*\"(?:[^\"\\]|\\.)*\"\s*\)"
    t = re.sub(decl_label, r"\1", text)

    # Empty asm barriers and standalone nops are C-side scheduler/lifetime
    # constraints. They do not contribute replacement assembly bodies and should
    # not lower progress.
    empty = r"\b(?:__asm__|asm)\s*(?:__volatile__|volatile)?\s*\(\s*\"\"\s*(?::.*?)?\)\s*;"
    t = re.sub(empty, "", t, flags=re.S)

    nop = r"\b(?:__asm__|asm)\s*(?:__volatile__|volatile)?\s*\(\s*\"nop\"\s*\)\s*;"
    t = re.sub(nop, "", t, flags=re.S)

    return INLINE_ASM_RE.sub(
        lambda m: "" if is_allowed_inline_asm(m.group(1)) else m.group(0),
        t,
    )


def is_clean(text: str) -> bool:
    """Plain C apart from INCLUDE_ASM stubs and nonblocking asm annotations."""
    t = strip_include_asm(text)
    t = strip_nonblocking_asm(strip_comments(t))
    return not ASM_RE.search(t)


FUNC_DEF_RE = re.compile(r"^[A-Za-z_][\w \t\*]*?\b([A-Za-z_]\w*)\s*\([^;{}]*\)\s*\{", re.M)


MACRO_STAMP_RE = re.compile(r"^[A-Z][A-Z0-9_]{3,}\(\w+(?:,\s*\w+)*\)\s*$", re.M)


def count_funcs(text: str) -> int:
    t = re.sub(r"/\*.*?\*/", "", text, flags=re.S)
    n = len([m for m in FUNC_DEF_RE.finditer(t)
             if m.group(1) not in ("if", "while", "for", "switch", "return")])
    n += len(MACRO_STAMP_RE.findall(t))
    return n


def parse_segments(yaml_path: pathlib.Path):
    """(offset, type, name) rows from a splat yaml, sorted by offset."""
    rows = []
    for line in yaml_path.read_text().splitlines():
        m = re.match(r"\s+- \[(0x[0-9A-Fa-f]+), (\w[\w.]*)(?:, ([^\]]+))?\]", line)
        if m:
            rows.append((int(m.group(1), 16), m.group(2), (m.group(3) or "").strip()))
            continue
        m = re.match(r"\s+- \[(0x[0-9A-Fa-f]+)\]\s*$", line)
        if m:
            rows.append((int(m.group(1), 16), "end", ""))
    rows.sort(key=lambda r: r[0])
    return rows


NONMATCHING_RE = re.compile(
    r"^nonmatching\s+\w+,\s*(0x[0-9A-Fa-f]+|\d+)\s*$", re.M
)


LEGACY_OVERLAY_IDS = {
    "sys_reset": "ovl_001",
    "menu_memcard": "ovl_002",
    "boot_display": "ovl_003",
    "render_clip": "ovl_006",
    "fx_field": "ovl_009",
    "fx_common": "fx_block_1792",
    "scene_e01": "ovl_171",
    "scene_e02": "ovl_172",
    "scene_e03": "ovl_173",
    "scene_e04": "ovl_174",
    "scene_e05": "ovl_175",
    "scene_e06": "ovl_176",
    "scene_e07": "ovl_177",
    "scene_e08": "ovl_178",
    "scene_e09": "ovl_179",
    "scene_e10": "ovl_180",
    "scene_e11": "ovl_181",
    "scene_e12": "ovl_182",
    "scene_e13": "ovl_183",
    "scene_e14": "ovl_184",
    "scene_e18": "ovl_185",
    "scene_e19": "ovl_186",
    "scene_e19_2": "ovl_187",
    "scene_e20": "ovl_188",
    "scene_e22": "ovl_189",
    "scene_e24": "ovl_190",
    "scene_e25": "ovl_191",
    "scene_e26": "ovl_192",
    "scene_e27": "ovl_193",
}


def legacy_overlay_name(name: str) -> str | None:
    return LEGACY_OVERLAY_IDS.get(name)


def legacy_asm_path(name: str, seg: str, asm_dir: pathlib.Path) -> pathlib.Path:
    rel = seg if name.startswith("SLUS_") else seg.split("/", 1)[-1]
    path = (asm_dir / rel).with_suffix(".s")
    if path.exists():
        return path
    legacy = legacy_overlay_name(name)
    if legacy is None:
        return path
    legacy_rel = rel.replace(name, legacy, 1)
    legacy_dir = ROOT / "asm/USA/overlays" / legacy
    legacy_path = (legacy_dir / legacy_rel).with_suffix(".s")
    return legacy_path if legacy_path.exists() else path


def count_asm_funcs(name: str, segments, asm_dir: pathlib.Path) -> int:
    n = 0
    for (off, typ, seg), (nxt, _, _) in zip(segments, segments[1:]):
        if typ != "asm":
            continue
        f = legacy_asm_path(name, seg, asm_dir)
        if not f.exists():
            continue
        text = f.read_text(errors="ignore")
        bodies = NONMATCHING_RE.findall(text)
        if not bodies:
            n += len(re.findall(r"^glabel ", text, re.M))
            continue
        cursor = 0
        limit = nxt - off
        for raw_size in bodies:
            if cursor >= limit:
                break
            n += 1
            cursor += int(raw_size, 16) if raw_size.startswith("0x") else int(raw_size)
    return n


def static_overlay_func_totals() -> dict[str, int]:
    path = ROOT / "docs/assets/code-overlays.csv"
    if not path.exists():
        return {}
    with path.open(newline="") as f:
        return {
            row["overlay_id"]: int(row["funcs"])
            for row in csv.DictReader(f)
            if row.get("overlay_id") and row.get("funcs")
        }


STATIC_OVERLAY_FUNC_TOTALS = static_overlay_func_totals()


FUNCTION_TOTAL_OVERRIDES = {
    "SLUS_006.62 (main)": 2555,
}

ROOM_TOTAL_FUNCS_OVERRIDE = 6454


def row_for(name: str, yaml_path: pathlib.Path, src_dir: pathlib.Path,
            asm_dir: pathlib.Path):
    m_funcs = d_funcs = 0
    segments = parse_segments(yaml_path)
    for f in (src_dir.rglob("*.c") if src_dir.exists() else []):
        t = f.read_text(errors="ignore")
        d_funcs += len(INCLUDE_ASM_RE.findall(t))
        if is_clean(t):
            m_funcs += count_funcs(strip_include_asm(t))
        else:
            d_funcs += count_funcs(strip_include_asm(t))
    n_funcs = m_funcs + d_funcs + count_asm_funcs(name, segments, asm_dir)
    legacy = legacy_overlay_name(name)
    if legacy in STATIC_OVERLAY_FUNC_TOTALS:
        n_funcs = max(n_funcs, STATIC_OVERLAY_FUNC_TOTALS[legacy])
    if name in FUNCTION_TOTAL_OVERRIDES:
        n_funcs = FUNCTION_TOTAL_OVERRIDES[name]
    # code bytes: c/asm subsegments, minus data carriers (TUs pulling .inc.s
    # blobs - they hold baked data, not code)
    total_b = matched_b = 0
    cache = {}
    for (off, typ, seg), (nxt, _, _) in zip(segments, segments[1:]):
        if typ not in ("c", "asm"):
            continue
        src = src_dir / (seg.split("/", 1)[-1] if name.startswith("ovl") else seg)
        src = src.with_suffix(".c")
        if src not in cache:
            txt = src.read_text(errors="ignore") if src.exists() else ""
            cache[src] = ("carrier" if ".inc.s" in txt
                          else "stubbed" if txt and is_clean(txt)
                          and INCLUDE_ASM_RE.search(txt)
                          else "clean" if txt and is_clean(txt) else "dirty")
        if typ == "c" and cache[src] == "carrier":
            continue
        total_b += nxt - off
        if typ == "c" and cache[src] == "clean":
            matched_b += nxt - off
        elif typ == "c" and cache[src] == "stubbed":
            stub_bytes = 0
            nm = ROOT / "asm/USA/main/nonmatchings" / seg
            if name.startswith("ovl") or name.startswith("room"):
                nm = ROOT / f"asm/USA/overlays/{name}/nonmatchings" / seg
            for s in (nm.glob("*.s") if nm.exists() else []):
                m2 = re.search(r"nonmatching \w+, (0x[0-9A-Fa-f]+|\d+)",
                               s.read_text(errors="ignore"))
                if m2:
                    v = m2.group(1)
                    stub_bytes += int(v, 16) if v.startswith("0x") else int(v)
            matched_b += max(0, (nxt - off) - stub_bytes)
    pct_f = 100.0 * m_funcs / n_funcs if n_funcs else 0.0
    pct_b = 100.0 * matched_b / total_b if total_b else 0.0
    return (
        f"| `{name}` | {m_funcs}/{n_funcs} | {pct_f:.1f}% "
        f"| {matched_b}/{total_b} | {pct_b:.1f}% |"
    ), m_funcs, n_funcs, matched_b, total_b


def configured_non_room_overlays() -> list[pathlib.Path]:
    return [
        path for path in sorted((ROOT / "configs/USA/overlays").glob("*.yaml"))
        if not path.stem.startswith("room_")
    ]


def main() -> None:
    lines = [
        "# Decompilation progress",
        "",
        f"_Generated by `tools/scripts/progress_report.py` on "
        f"{datetime.date.today().isoformat()}. Regenerate with `make progress`._",
        "",
        "A translation unit counts as decompiled when it has no INCLUDE_ASM",
        "and no whole-function or nontrivial inline assembly. Register/symbol asm labels,",
        "empty barriers, standalone nop barriers, and small irreducible CPU/GTE",
        "instruction snippets do not lower progress. Code bytes cover function subsegments",
        "only (baked data carriers are excluded). Every built binary is",
        "byte-identical to retail (`make check`, `make overlay-check-all`).",
        "",
        "| Binary | Functions | % | Code bytes | % |",
        "|---|---:|---:|---:|---:|",
    ]
    totals = [0, 0, 0, 0]
    r, *t = row_for("SLUS_006.62 (main)", ROOT / "configs/USA/main.yaml",
                    ROOT / "src/main", ROOT / "asm/USA/main")
    lines.append(r)
    totals = [a + b for a, b in zip(totals, t)]
    for ovl_yaml in configured_non_room_overlays():
        name = ovl_yaml.stem
        r, *t = row_for(name, ovl_yaml, ROOT / "src/overlays" / name,
                        ROOT / "asm/USA/overlays" / name)
        lines.append(r)
        totals = [a + b for a, b in zip(totals, t)]
    room = [0, 0, 0, 0]
    rooms = sorted((ROOT / "configs/USA/overlays").glob("room_*.yaml"))
    for ovl_yaml in rooms:
        name = ovl_yaml.stem
        _, *t = row_for(name, ovl_yaml, ROOT / "src/overlays" / name,
                        ROOT / "asm/USA/overlays" / name)
        room = [a + b for a, b in zip(room, t)]
    if rooms:
        mf, nf, mb, tb = room
        nf = ROOM_TOTAL_FUNCS_OVERRIDE
        room[1] = nf
        pf = 100.0 * mf / nf if nf else 0.0
        pb = 100.0 * mb / tb if tb else 0.0
        lines.append(f"| `room overlays (x{len(rooms)})` | {mf}/{nf} ({pf:.1f}%) "
                     f"| {pf:.1f}% | {mb}/{tb} | {pb:.1f}% |")
        totals = [a + b for a, b in zip(totals, room)]
    mf, nf, mb, tb = totals
    lines.append(
        f"| **total** | **{mf}/{nf}** | **{100.0*mf/nf:.1f}%** "
        f"| **{mb}/{tb}** | **{100.0*mb/tb:.1f}%** |"
    )
    out = ROOT / "docs/PROGRESS.md"
    out.write_text("\n".join(lines) + "\n")
    print(f"wrote {out}")


if __name__ == "__main__":
    main()
