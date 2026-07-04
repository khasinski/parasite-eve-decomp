#!/usr/bin/env python3
"""Catalog PE.IMG MIPS overlay candidates and correlate them with room data.

The output is metadata only. It never writes executable bytes from PE.IMG.
Use it to reproduce docs/assets/code-overlays.md and to decide which candidate
slices are ready to turn into local, user-generated splat overlay targets.
"""

from __future__ import annotations

import argparse
import csv
import hashlib
import json
import re
import struct
import sys
from collections import Counter
from dataclasses import asdict, dataclass
from pathlib import Path

SECTOR_SIZE = 2048
WORDS_PER_SECTOR = SECTOR_SIZE // 4
EXE_LOAD_ADDR = 0x80010000
EXE_FILE_HEADER = 0x800
ROOM_TABLE_VADDR = 0x80093378
ROOM_COUNT = 410
FUNCTION_KIND_AUDIT_CSV = Path("docs/assets/overlay-function-kind-audit.csv")


@dataclass
class RoomEntry:
    room_id: int
    sector: int
    size1: int
    size2: int
    size3: int
    section3_sector: int
    end_sector: int


@dataclass
class OverlayCandidate:
    overlay_id: str
    offset: int
    sector_start: int
    sector_end: int
    sector_count: int
    size_bytes: int
    funcs: int
    prologues: int
    text_calls: int
    first_prologue_offset: int | None
    first_jrra_offset: int | None
    shape: str
    sha1: str
    sha1_12: str
    tags: list[str]
    room_id: int | None = None
    room_section3_sector: int | None = None
    room_delta: int | None = None
    room_base_sector: int | None = None
    room_size1: int | None = None
    room_size2: int | None = None
    room_size3: int | None = None
    room_section3_size_bytes: int | None = None
    room_section3_sha1: str | None = None
    room_section3_sha1_12: str | None = None
    match_status: str = ""
    recommended_target_slice: str = ""
    default_vram: int | None = None
    load_vram: int | None = None
    canonical_vram: int | None = None
    entry_vram: int | None = None
    duplicate_count: int = 1
    duplicate_group: str = ""
    role_hypothesis: str = ""
    role_confidence: str = ""
    top_calls: str = ""
    call_domains: str = ""


@dataclass
class OverlayConfigStatus:
    overlay_id: str
    target_slice: str
    vram: int
    sha1: str


@dataclass
class OverlaySubsegment:
    start: int
    kind: str
    name: str


@dataclass
class OverlayDecompStatus:
    overlay_id: str
    target_slice: str
    target_size: int
    c_bytes: int
    asm_bytes: int
    other_bytes: int
    c_segments: list[str]
    asm_segments: list[str]


def exe_offset(vaddr: int) -> int:
    return (vaddr - EXE_LOAD_ADDR) + EXE_FILE_HEADER


def load_room_table(main_exe: Path) -> list[RoomEntry]:
    data = main_exe.read_bytes()
    base = exe_offset(ROOM_TABLE_VADDR)
    rooms: list[RoomEntry] = []
    for i in range(ROOM_COUNT):
        sector, sizes = struct.unpack_from("<II", data, base + i * 8)
        if sector == 0 or sizes == 0:
            continue
        size1 = sizes & 0xFF
        size2 = (sizes >> 8) & 0xFFF
        size3 = (sizes >> 20) & 0xFFF
        section3 = sector + size1 + size2
        rooms.append(
            RoomEntry(
                room_id=i + 1,
                sector=sector,
                size1=size1,
                size2=size2,
                size3=size3,
                section3_sector=section3,
                end_sector=section3 + size3,
            )
        )
    return rooms


def scan_code_blocks(pe_img: Path) -> tuple[int, list[dict[str, int]]]:
    """Return scanner rows compatible with pc/tools/overlay_scan.py."""
    try:
        import numpy as np
    except ImportError:
        return scan_code_blocks_fallback(pe_img)

    words = np.fromfile(pe_img, dtype="<u4")
    nsec = len(words) // WORDS_PER_SECTOR
    usable = words[: nsec * WORDS_PER_SECTOR].reshape(nsec, WORDS_PER_SECTOR)
    jrra = (usable == 0x03E00008).sum(axis=1)
    prol = ((usable & 0xFFFF8000) == 0x27BD8000).sum(axis=1)
    jalt = ((usable & 0xFFF80000) == 0x0C000000).sum(axis=1)
    code = (jrra >= 1) & ((prol >= 1) | (jalt >= 3))
    sectors = np.nonzero(code)[0].tolist()
    return nsec, make_blocks(sectors, jrra.tolist(), prol.tolist(), jalt.tolist())


def scan_code_blocks_fallback(pe_img: Path) -> tuple[int, list[dict[str, int]]]:
    data = pe_img.read_bytes()
    nsec = len(data) // SECTOR_SIZE
    jrra: list[int] = []
    prol: list[int] = []
    jalt: list[int] = []
    sectors: list[int] = []

    for sec in range(nsec):
        base = sec * SECTOR_SIZE
        c_jrra = 0
        c_prol = 0
        c_jalt = 0
        for i in range(WORDS_PER_SECTOR):
            word = struct.unpack_from("<I", data, base + i * 4)[0]
            c_jrra += word == 0x03E00008
            c_prol += (word & 0xFFFF8000) == 0x27BD8000
            c_jalt += (word & 0xFFF80000) == 0x0C000000
        jrra.append(c_jrra)
        prol.append(c_prol)
        jalt.append(c_jalt)
        if c_jrra >= 1 and (c_prol >= 1 or c_jalt >= 3):
            sectors.append(sec)

    return nsec, make_blocks(sectors, jrra, prol, jalt)


def first_word_offset(data: bytes, predicate) -> int | None:
    for offset in range(0, len(data) - 3, 4):
        word = struct.unpack_from("<I", data, offset)[0]
        if predicate(word):
            return offset
    return None


def word_offsets(data: bytes, predicate) -> list[int]:
    offsets: list[int] = []
    for offset in range(0, len(data) - 3, 4):
        word = struct.unpack_from("<I", data, offset)[0]
        if predicate(word):
            offsets.append(offset)
    return offsets


def format_offset_list(offsets: list[int], limit: int = 16) -> str:
    values = [fmt_hex(offset) for offset in offsets[:limit]]
    if len(offsets) > limit:
        values.append(f"+{len(offsets) - limit} more")
    return ";".join(values)


def scan_sector_metrics(pe_img: Path) -> tuple[int, list[dict[str, int | bool]]]:
    try:
        import numpy as np
    except ImportError:
        return scan_sector_metrics_fallback(pe_img)

    words = np.fromfile(pe_img, dtype="<u4")
    nsec = len(words) // WORDS_PER_SECTOR
    usable = words[: nsec * WORDS_PER_SECTOR].reshape(nsec, WORDS_PER_SECTOR)
    jrra = (usable == 0x03E00008).sum(axis=1)
    prologues = ((usable & 0xFFFF8000) == 0x27BD8000).sum(axis=1)
    text_calls = ((usable & 0xFFF80000) == 0x0C000000).sum(axis=1)
    included = (jrra >= 1) & ((prologues >= 1) | (text_calls >= 3))
    rows: list[dict[str, int | bool]] = []
    for sector in range(nsec):
        rows.append(
            {
                "sector": sector,
                "jrra": int(jrra[sector]),
                "prologues": int(prologues[sector]),
                "text_calls": int(text_calls[sector]),
                "scanner_included": bool(included[sector]),
            }
        )
    return nsec, rows


def scan_sector_metrics_fallback(pe_img: Path) -> tuple[int, list[dict[str, int | bool]]]:
    data = pe_img.read_bytes()
    nsec = len(data) // SECTOR_SIZE
    rows: list[dict[str, int | bool]] = []
    for sector in range(nsec):
        base = sector * SECTOR_SIZE
        jrra = 0
        prologues = 0
        text_calls = 0
        for i in range(WORDS_PER_SECTOR):
            word = struct.unpack_from("<I", data, base + i * 4)[0]
            jrra += word == 0x03E00008
            prologues += (word & 0xFFFF8000) == 0x27BD8000
            text_calls += (word & 0xFFF80000) == 0x0C000000
        rows.append(
            {
                "sector": sector,
                "jrra": jrra,
                "prologues": prologues,
                "text_calls": text_calls,
                "scanner_included": jrra >= 1 and (prologues >= 1 or text_calls >= 3),
            }
        )
    return nsec, rows


def make_blocks(
    sectors: list[int],
    jrra: list[int],
    prologues: list[int],
    text_calls: list[int],
) -> list[dict[str, int]]:
    blocks: list[list[int]] = []
    cur: list[int] | None = None
    for sector in sectors:
        if cur is not None and sector - cur[1] <= 2:
            cur[1] = sector
        else:
            if cur is not None:
                blocks.append(cur)
            cur = [sector, sector]
    if cur is not None:
        blocks.append(cur)

    rows: list[dict[str, int]] = []
    for start, end in blocks:
        rows.append(
            {
                "sector_start": start,
                "sector_end": end,
                "offset": start * SECTOR_SIZE,
                "sector_count": end - start + 1,
                "funcs": sum(jrra[start : end + 1]),
                "prologues": sum(prologues[start : end + 1]),
                "text_calls": sum(text_calls[start : end + 1]),
            }
        )
    return rows


def classify_tags(index: int, row: dict[str, int]) -> list[str]:
    tags: list[str] = []
    if row["sector_count"] <= 2:
        tags.append("tiny")
    if index == 2:
        tags.append("title/intro vicinity")
    if row["offset"] == 0x01579000:
        tags.append("known-intro-runtime")
    if row["text_calls"] >= 350:
        tags.append("call-heavy")
    return tags


def default_vram(candidate: OverlayCandidate) -> int | None:
    if candidate.overlay_id == "ovl_009":
        return 0x8018EFF0
    return None


def known_load_vram(candidate: OverlayCandidate) -> int | None:
    if candidate.overlay_id == "ovl_009":
        return 0x8018EFF0
    return None


def known_canonical_vram(candidate: OverlayCandidate) -> int | None:
    if candidate.overlay_id == "ovl_009":
        return 0x801909A0
    return None


def known_entry_vram(candidate: OverlayCandidate) -> int | None:
    if candidate.overlay_id == "ovl_009":
        return 0x801909B4
    return None


def recommended_target_slice(candidate: OverlayCandidate) -> str:
    if candidate.room_id is not None and candidate.overlay_id != "ovl_009":
        return "room-section3"
    return "scanner"


def match_status(candidate: OverlayCandidate) -> str:
    if default_vram(candidate) is not None:
        return "match-ready"
    if candidate.room_id is not None:
        return "owner-known-needs-vram"
    if "tiny" in candidate.tags:
        return "review-tiny-or-false-positive"
    return "needs-runtime-trace"


def load_symbols(sym_file: Path | None, config_yaml: Path | None) -> dict[int, str]:
    symbols = {
        0x80078004: "RotMatrix",
        0x80078134: "CompMatrix",
        0x80078CC4: "Gte_ScaleMatrix",
        0x80079FB4: "Gte_Atan2",
        0x8003A088: "Render_TransformVertices",
        0x8003AC90: "Render_TransformMorphVertices",
    }
    if config_yaml and config_yaml.exists():
        pattern = re.compile(r"- \[(0x[0-9A-Fa-f]+), c, ([^\]]+)\]")
        for line in config_yaml.read_text().splitlines():
            match = pattern.search(line)
            if not match:
                continue
            vaddr = EXE_LOAD_ADDR + int(match.group(1), 16)
            symbols.setdefault(vaddr, Path(match.group(2)).stem)
    if sym_file and sym_file.exists():
        pattern = re.compile(r"([A-Za-z_]\w*)\s*=\s*(0x[0-9A-Fa-f]+);\s*// type:func")
        for line in sym_file.read_text().splitlines():
            match = pattern.match(line)
            if match:
                symbols[int(match.group(2), 16)] = match.group(1)
    return symbols


def call_domain(name: str, addr: int) -> str:
    if name.startswith(
        (
            "Render",
            "Gpu",
            "Draw",
            "SetDraw",
            "SetDef",
            "SetDisp",
            "PutDraw",
            "AddPrim",
            "ClearImage",
            "LoadImage",
            "GetClut",
            "GetTPage",
        )
    ):
        return "render/gpu"
    if name.startswith(
        (
            "Rot",
            "Gte",
            "SetRot",
            "SetTrans",
            "SetGeom",
            "CompMatrix",
            "OuterProduct",
        )
    ):
        return "gte/math"
    if name.startswith(("Anim", "Entity", "Actor")):
        return "entity/anim"
    if name.startswith(("Scene", "Field", "Geo")):
        return "field/scene"
    if name.startswith(("Akao", "Spu", "Seq", "Sfx")):
        return "audio"
    if name.startswith("Battle"):
        return "battle"
    if name.startswith(("Menu", "Widget", "Window", "Save")):
        return "menu/ui"
    if name.startswith(("Cd", "CD_", "CdRom")):
        return "cdrom"
    if name.startswith("Asset"):
        return "asset lookup"
    if name in {"rand", "rsin", "rcos", "memset", "memcpy", "VSync", "DrawSync"}:
        return "lib/util"
    if 0x800C0000 <= addr < 0x800E2000:
        return "overlay/shared data-code?"
    return "unknown"


def call_profile(data: bytes, symbols: dict[int, str]) -> tuple[str, str]:
    calls = call_counts(data)
    top_parts: list[str] = []
    domains: Counter[str] = Counter()
    for addr, count in calls.items():
        name = symbols.get(addr, f"0x{addr:08X}")
        domains[call_domain(name, addr)] += count
    for addr, count in calls.most_common(8):
        name = symbols.get(addr, f"0x{addr:08X}")
        top_parts.append(f"{name}:{count}")
    domain_parts = [f"{domain}:{count}" for domain, count in domains.most_common(8)]
    return "; ".join(top_parts), "; ".join(domain_parts)


def call_counts(data: bytes) -> Counter[int]:
    calls: Counter[int] = Counter()
    for (word,) in struct.iter_unpack("<I", data[: len(data) // 4 * 4]):
        if (word & 0xFC000000) != 0x0C000000:
            continue
        target = 0x80000000 | ((word & 0x03FFFFFF) << 2)
        if EXE_LOAD_ADDR <= target < 0x80100000:
            calls[target] += 1
    return calls


def enrich_match_metadata(candidates: list[OverlayCandidate]) -> None:
    by_sha: dict[str, list[OverlayCandidate]] = {}
    for candidate in candidates:
        candidate.default_vram = default_vram(candidate)
        candidate.load_vram = known_load_vram(candidate)
        candidate.canonical_vram = known_canonical_vram(candidate)
        candidate.entry_vram = known_entry_vram(candidate)
        candidate.recommended_target_slice = recommended_target_slice(candidate)
        candidate.match_status = match_status(candidate)
        candidate.role_hypothesis, candidate.role_confidence = role_hypothesis(candidate)
        by_sha.setdefault(candidate.sha1, []).append(candidate)

    for group in by_sha.values():
        if len(group) == 1:
            continue
        group_ids = ",".join(candidate.overlay_id for candidate in group)
        for candidate in group:
            candidate.duplicate_count = len(group)
            candidate.duplicate_group = group_ids


def role_hypothesis(candidate: OverlayCandidate) -> tuple[str, str]:
    if candidate.overlay_id == "ovl_009":
        return "intro/SquareSoft logo runtime overlay", "high"
    if "title/intro vicinity" in candidate.tags:
        return "title/intro subsystem", "medium"
    if candidate.room_id is not None:
        if candidate.sector_count <= 2 and candidate.funcs <= 10:
            return "tiny per-room scene/render script or stub", "low-medium"
        return "per-room field scene/entity/render code", "medium"
    if candidate.overlay_id in {
        "ovl_003",
        "ovl_004",
        "ovl_007",
        "ovl_178",
        "ovl_181",
        "ovl_182",
        "ovl_183",
        "ovl_187",
        "ovl_189",
    }:
        return "non-room 3D/render/effect subsystem", "medium-low"
    return "non-room subsystem, late-game event, or code-like block", "low"


def correlate_room(row: dict[str, int], rooms: list[RoomEntry]) -> RoomEntry | None:
    if not rooms:
        return None
    sector = row["sector_start"]
    exact = [room for room in rooms if room.section3_sector == sector]
    if exact:
        return exact[0]
    near = min(rooms, key=lambda room: abs(sector - room.section3_sector))
    if abs(sector - near.section3_sector) <= 2:
        return near
    return None


def build_catalog(
    pe_img: Path,
    main_exe: Path | None,
    sym_file: Path | None = None,
    config_yaml: Path | None = None,
) -> tuple[int, list[OverlayCandidate]]:
    image_sectors, rows = scan_code_blocks(pe_img)
    image = pe_img.read_bytes()
    rooms = load_room_table(main_exe) if main_exe else []
    symbols = load_symbols(sym_file, config_yaml)
    candidates: list[OverlayCandidate] = []

    for idx, row in enumerate(rows, 1):
        start = row["offset"]
        size = row["sector_count"] * SECTOR_SIZE
        room = correlate_room(row, rooms)
        shape = (
            f"({row['sector_count']}, {row['funcs']}, "
            f"{row['prologues']}, {row['text_calls']})"
        )
        sha1 = hashlib.sha1(image[start : start + size]).hexdigest()
        block = image[start : start + size]
        top_calls, call_domains = call_profile(image[start : start + size], symbols)
        candidate = OverlayCandidate(
            overlay_id=f"ovl_{idx:03d}",
            offset=start,
            sector_start=row["sector_start"],
            sector_end=row["sector_end"],
            sector_count=row["sector_count"],
            size_bytes=size,
            funcs=row["funcs"],
            prologues=row["prologues"],
            text_calls=row["text_calls"],
            first_prologue_offset=first_word_offset(
                block, lambda word: (word & 0xFFFF8000) == 0x27BD8000
            ),
            first_jrra_offset=first_word_offset(block, lambda word: word == 0x03E00008),
            shape=shape,
            sha1=sha1,
            sha1_12=sha1[:12],
            tags=classify_tags(idx, row),
            top_calls=top_calls,
            call_domains=call_domains,
        )
        if room is not None:
            candidate.room_id = room.room_id
            candidate.room_section3_sector = room.section3_sector
            candidate.room_delta = row["sector_start"] - room.section3_sector
            candidate.room_base_sector = room.sector
            candidate.room_size1 = room.size1
            candidate.room_size2 = room.size2
            candidate.room_size3 = room.size3
            candidate.room_section3_size_bytes = room.size3 * SECTOR_SIZE
            room_start = room.section3_sector * SECTOR_SIZE
            room_end = room_start + candidate.room_section3_size_bytes
            room_sha1 = hashlib.sha1(image[room_start:room_end]).hexdigest()
            candidate.room_section3_sha1 = room_sha1
            candidate.room_section3_sha1_12 = room_sha1[:12]
        candidates.append(candidate)

    enrich_match_metadata(candidates)
    return image_sectors, candidates


def sector_range(candidate: OverlayCandidate) -> str:
    if candidate.sector_start == candidate.sector_end:
        return str(candidate.sector_start)
    return f"{candidate.sector_start}..{candidate.sector_end}"


def room_label(candidate: OverlayCandidate) -> str:
    if candidate.room_id is None:
        return ""
    label = f"m{candidate.room_id:03d}"
    if candidate.room_delta:
        label += f"{candidate.room_delta:+d}"
    return label


def emit_summary(image_sectors: int, candidates: list[OverlayCandidate]) -> None:
    exact = sum(c.room_delta == 0 for c in candidates)
    near = sum(c.room_delta not in (None, 0) for c in candidates)
    tiny = sum("tiny" in c.tags for c in candidates)
    call_heavy = sum("call-heavy" in c.tags for c in candidates)
    print(f"PE.IMG sectors: {image_sectors}")
    print(f"Overlay candidates: {len(candidates)}")
    print(f"Candidate code: {sum(c.sector_count for c in candidates)} sectors")
    print(f"Room section-3 exact: {exact}")
    print(f"Room section-3 near: {near}")
    print(f"Outside room table: {len(candidates) - exact - near}")
    print(f"Tiny: {tiny}")
    print(f"Call-heavy: {call_heavy}")


def emit_csv(candidates: list[OverlayCandidate]) -> None:
    fields = [
        "overlay_id",
        "offset_hex",
        "sector_start",
        "sector_end",
        "sector_count",
        "size_kib",
        "funcs",
        "prologues",
        "text_calls",
        "shape",
        "sha1_12",
        "room",
        "room_section3_sector",
        "room_delta",
        "room_sizes",
        "room_section3_size_kib",
        "room_section3_sha1_12",
        "match_status",
        "recommended_target_slice",
        "default_vram",
        "load_vram",
        "canonical_vram",
        "entry_vram",
        "duplicate_count",
        "duplicate_group",
        "role_hypothesis",
        "role_confidence",
        "top_calls",
        "call_domains",
        "tags",
    ]
    writer = csv.DictWriter(sys.stdout, fieldnames=fields)
    writer.writeheader()
    for c in candidates:
        writer.writerow(
            {
                "overlay_id": c.overlay_id,
                "offset_hex": f"0x{c.offset:08X}",
                "sector_start": c.sector_start,
                "sector_end": c.sector_end,
                "sector_count": c.sector_count,
                "size_kib": c.size_bytes // 1024,
                "funcs": c.funcs,
                "prologues": c.prologues,
                "text_calls": c.text_calls,
                "shape": c.shape,
                "sha1_12": c.sha1_12,
                "room": room_label(c),
                "room_section3_sector": c.room_section3_sector or "",
                "room_delta": c.room_delta if c.room_delta is not None else "",
                "room_sizes": (
                    f"{c.room_size1}/{c.room_size2}/{c.room_size3}"
                    if c.room_id is not None
                    else ""
                ),
                "room_section3_size_kib": (
                    c.room_section3_size_bytes // 1024
                    if c.room_section3_size_bytes is not None
                    else ""
                ),
                "room_section3_sha1_12": c.room_section3_sha1_12 or "",
                "match_status": c.match_status,
                "recommended_target_slice": c.recommended_target_slice,
                "default_vram": f"0x{c.default_vram:08X}" if c.default_vram is not None else "",
                "load_vram": f"0x{c.load_vram:08X}" if c.load_vram is not None else "",
                "canonical_vram": (
                    f"0x{c.canonical_vram:08X}" if c.canonical_vram is not None else ""
                ),
                "entry_vram": f"0x{c.entry_vram:08X}" if c.entry_vram is not None else "",
                "duplicate_count": c.duplicate_count,
                "duplicate_group": c.duplicate_group,
                "role_hypothesis": c.role_hypothesis,
                "role_confidence": c.role_confidence,
                "top_calls": c.top_calls,
                "call_domains": c.call_domains,
                "tags": ", ".join(c.tags),
            }
        )


def emit_markdown(candidates: list[OverlayCandidate]) -> None:
    print("# PE.IMG Code Overlay Table")
    print()
    print(
        "Generated from `tools/scripts/catalog_pe_overlays.py --format md`. "
        "This is the human-readable full table for every cataloged code-overlay "
        "candidate. It records metadata only; original bytes stay local."
    )
    print()
    print(f"- Candidate rows: {len(candidates)}")
    print("- Machine-readable source: `docs/assets/code-overlays.csv`")
    print("- Match readiness audit: `docs/assets/overlay-match-readiness.md`")
    print("- Match worklist: `docs/assets/overlay-match-worklist.md`")
    print("- Match worklist trace template: `docs/assets/overlay-match-worklist-trace-template.csv`")
    print("- Complete trace backlog: `docs/assets/overlay-trace-sheet-all.csv`")
    print("- Trace action plan: `docs/assets/overlay-trace-actions.md`")
    print("- Hinted config audit: `docs/assets/overlay-hinted-config-audit.md`")
    print("- Static function queue: `docs/assets/overlay-static-function-queue.md`")
    print("- Function boundary audit: `docs/assets/overlay-function-boundary-audit.md`")
    print("- Trace session plan: `docs/assets/overlay-trace-session-plan.md`")
    print("- Match worklist: `docs/assets/overlay-match-worklist.md` / `docs/assets/overlay-match-worklist.csv`")
    print("- Family match worklist: `docs/assets/overlay-family-match-worklist.md` / `docs/assets/overlay-family-match-worklist.csv`")
    print("- Match worklist trace template: `docs/assets/overlay-match-worklist-trace-template.csv`")
    print("- Family trace template: `docs/assets/overlay-family-trace-template.csv`")
    print()
    print(
        "| ID | PE.IMG offset | sectors | size | ~funcs | prologues | "
        "text-calls | room | role | confidence | shape | sha1/12 | tags |"
    )
    print("|---|---:|---:|---:|---:|---:|---:|---|---|---|---|---|---|")
    for c in candidates:
        tags = ", ".join(c.tags)
        room = room_label(c)
        print(
            f"| `{c.overlay_id}` | `0x{c.offset:08X}` | `{sector_range(c)}` | "
            f"{c.size_bytes // 1024} KiB | {c.funcs} | {c.prologues} | "
            f"{c.text_calls} | {f'`{room}`' if room else ''} | "
            f"{c.role_hypothesis} | {c.role_confidence} | `{c.shape}` | "
            f"`{c.sha1_12}` | {tags} |"
        )


def emit_json(candidates: list[OverlayCandidate]) -> None:
    print(json.dumps([asdict(candidate) for candidate in candidates], indent=2))


def priority_score(candidate: OverlayCandidate) -> tuple[int, int, int, int]:
    if candidate.match_status == "match-ready":
        return (1000, candidate.text_calls, candidate.size_bytes, -candidate.sector_start)
    if candidate.overlay_id == "ovl_002":
        return (900, candidate.text_calls, candidate.size_bytes, -candidate.sector_start)
    if "call-heavy" in candidate.tags:
        return (800, candidate.text_calls, candidate.size_bytes, -candidate.sector_start)
    if candidate.duplicate_count > 1:
        return (700, candidate.duplicate_count, candidate.text_calls, -candidate.sector_start)
    if candidate.role_confidence == "medium":
        return (600, candidate.text_calls, candidate.size_bytes, -candidate.sector_start)
    if candidate.role_confidence == "medium-low":
        return (500, candidate.text_calls, candidate.size_bytes, -candidate.sector_start)
    return (100, candidate.text_calls, candidate.size_bytes, -candidate.sector_start)


def trace_command(candidate: OverlayCandidate) -> str:
    if candidate.default_vram is not None:
        return f"make overlay-check OVERLAY={candidate.overlay_id}"
    if candidate.recommended_target_slice == "room-section3":
        return (
            f"make overlay-init OVERLAY={candidate.overlay_id} "
            "OVERLAY_SLICE=room-section3 VRAM=<traced-vram>"
        )
    return f"make overlay-init OVERLAY={candidate.overlay_id} VRAM=<traced-vram>"


def loader_trace_fields(candidate: OverlayCandidate) -> dict[str, str]:
    if candidate.overlay_id == "ovl_009":
        return {
            "loader_site": "Overlay_LoadInitialImage scanner read",
            "source_expr": f"g_PeImageBaseLba + {candidate.sector_start}",
            "caller_pc_hint": "0x8006F034",
            "dst_probe": "trace-backed 0x8018EFF0",
            "entry_probe": "trace-backed 0x801909B4",
        }
    if candidate.recommended_target_slice == "room-section3" and candidate.room_id is not None:
        return {
            "loader_site": "Scene_LoadRoom section3 read",
            "source_expr": f"g_PeImageBaseLba + {candidate.room_section3_sector}",
            "caller_pc_hint": "0x8006B6EC",
            "dst_probe": "*(u32 *)(g_GameState + 0x18C)",
            "entry_probe": "first PC executed inside section3 destination range",
        }
    return {
        "loader_site": "CdRom_ReadSectors/CdRom_ReadSectorsFromLba",
        "source_expr": f"g_PeImageBaseLba + {candidate.sector_start}",
        "caller_pc_hint": "capture runtime caller_pc",
        "dst_probe": "read call dst argument/register",
        "entry_probe": "first PC executed inside loaded destination range",
    }


def trace_breakpoint(candidate: OverlayCandidate) -> str:
    if candidate.overlay_id == "ovl_009":
        return "CdRom_ReadSectors 0x8006E6D4, caller_pc 0x8006F034"
    if candidate.recommended_target_slice == "room-section3" and candidate.room_id is not None:
        return "CdRom_ReadSectorsFromLba 0x8006E6A8, caller_pc 0x8006B6EC"
    return "CdRom_ReadSectors 0x8006E6D4 or wrapper 0x8006E6A8, filter by pe_sector"


def emit_priority_markdown(candidates: list[OverlayCandidate]) -> None:
    print("# Overlay Match Priority Queue")
    print()
    print(
        "Generated from `tools/scripts/catalog_pe_overlays.py --format priority-md`. "
        "This is a work queue for runtime tracing and FF8-style matching. It records "
        "metadata only; extracted overlay bytes stay local and ignored."
    )
    print()
    print("## Selection Rules")
    print()
    print("- Keep `ovl_009` as the known-good split/build/check template.")
    print("- Trace `ovl_002` early because it has title/intro evidence.")
    print("- Prefer call-heavy overlays because they expose more shared engine contracts.")
    print("- For duplicate groups, decompile one representative before touching siblings.")
    print("- For room-correlated overlays, use `room-section3` once the real VRAM is traced.")
    print()

    top = sorted(candidates, key=priority_score, reverse=True)[:20]
    print("## Top Queue")
    print()
    print(
        "| Overlay | Status | Role | Slice | Size | Room | Why | First command |"
    )
    print("|---|---|---|---|---:|---|---|---|")
    for c in top:
        why: list[str] = []
        if c.default_vram is not None:
            why.append("known VRAM")
        if c.overlay_id == "ovl_002":
            why.append("title/intro anchor")
        if "call-heavy" in c.tags:
            why.append("call-heavy")
        if c.duplicate_count > 1:
            why.append(f"duplicate group x{c.duplicate_count}")
        if c.room_id is not None:
            why.append("room section-3 owner")
        if not why:
            why.append(c.role_confidence)
        print(
            f"| `{c.overlay_id}` | `{c.match_status}` | {c.role_hypothesis} | "
            f"`{c.recommended_target_slice}` | {c.size_bytes // 1024} KiB | "
            f"{f'`{room_label(c)}`' if room_label(c) else ''} | "
            f"{', '.join(why)} | `{trace_command(c)}` |"
        )
    print()

    print("## Representative Families")
    print()
    families: dict[str, list[OverlayCandidate]] = {}
    for candidate in candidates:
        families.setdefault(candidate.shape, []).append(candidate)
    repeated = sorted(
        (group for group in families.values() if len(group) >= 3),
        key=lambda group: (len(group), max(c.text_calls for c in group)),
        reverse=True,
    )
    print("| Shape | Count | Representative | Role | Top calls |")
    print("|---|---:|---|---|---|")
    for group in repeated[:12]:
        representative = max(group, key=priority_score)
        print(
            f"| `{representative.shape}` | {len(group)} | `{representative.overlay_id}` | "
            f"{representative.role_hypothesis} | {representative.top_calls} |"
        )
    print()

    print("## Runtime Trace Targets")
    print()
    trace_targets = [
        c
        for c in candidates
        if c.default_vram is None
        and (
            c.overlay_id == "ovl_002"
            or "call-heavy" in c.tags
            or c.role_confidence in {"medium", "medium-low"}
        )
    ]
    trace_targets = sorted(trace_targets, key=priority_score, reverse=True)[:15]
    print("| Overlay | PE.IMG sector | Sector count | Role | Domains |")
    print("|---|---:|---:|---|---|")
    for c in trace_targets:
        sector, sector_count, _size, _sha, _sha_full = target_slice_info(
            c, c.recommended_target_slice
        )
        print(
            f"| `{c.overlay_id}` | {sector} | {sector_count} | "
            f"{c.role_hypothesis} | {c.call_domains} |"
        )
    print()

    print("## Notes")
    print()
    print(
        "A generated config should only be committed after the VRAM/load address is "
        "traced. Placeholder `VRAM=<traced-vram>` commands are intentionally not "
        "ready to run as-is."
    )


def read_config_value(text: str, pattern: str) -> str | None:
    match = re.search(pattern, text, re.MULTILINE)
    return match.group(1) if match else None


def read_overlay_subsegments(text: str) -> tuple[list[OverlaySubsegment], int | None]:
    subsegments: list[OverlaySubsegment] = []
    for match in re.finditer(
        r"^\s*-\s*\[(0x[0-9A-Fa-f]+|\d+),\s*([A-Za-z0-9_]+),\s*([^\]\s#]+)\]",
        text,
        re.MULTILINE,
    ):
        subsegments.append(
            OverlaySubsegment(
                start=int(match.group(1), 0),
                kind=match.group(2),
                name=match.group(3),
            )
        )

    end_match = None
    for match in re.finditer(r"^\s*-\s*\[(0x[0-9A-Fa-f]+|\d+)\]\s*$", text, re.MULTILINE):
        end_match = match
    end = int(end_match.group(1), 0) if end_match else None
    return sorted(subsegments, key=lambda segment: segment.start), end


def read_overlay_configs(config_dir: Path, candidates: list[OverlayCandidate]) -> dict[str, OverlayConfigStatus]:
    candidates_by_id = {candidate.overlay_id: candidate for candidate in candidates}
    configured: dict[str, OverlayConfigStatus] = {}
    for path in sorted(config_dir.glob("*.yaml")):
        overlay_id = path.stem
        candidate = candidates_by_id.get(overlay_id)
        if candidate is None:
            continue
        text = path.read_text()
        sha1 = read_config_value(text, r"^sha1:\s*([0-9a-f]{40})\s*$")
        vram_text = read_config_value(text, r"^\s*vram:\s*(0x[0-9A-Fa-f]+|\d+)\s*$")
        if sha1 is None or vram_text is None:
            continue
        target_slice = "unknown"
        for candidate_slice in ("scanner", "room-section3"):
            try:
                _sector, _sector_count, _size, _sha12, target_sha1 = target_slice_info(
                    candidate, candidate_slice
                )
            except ValueError:
                continue
            if target_sha1 == sha1:
                target_slice = candidate_slice
                break
        configured[overlay_id] = OverlayConfigStatus(
            overlay_id=overlay_id,
            target_slice=target_slice,
            vram=int(vram_text, 0),
            sha1=sha1,
        )
    return configured


def read_overlay_decomp_status(
    config_dir: Path,
    candidates: list[OverlayCandidate],
) -> list[OverlayDecompStatus]:
    candidates_by_id = {candidate.overlay_id: candidate for candidate in candidates}
    statuses: list[OverlayDecompStatus] = []
    configs = read_overlay_configs(config_dir, candidates)
    for path in sorted(config_dir.glob("*.yaml")):
        overlay_id = path.stem
        candidate = candidates_by_id.get(overlay_id)
        config = configs.get(overlay_id)
        if candidate is None or config is None:
            continue
        text = path.read_text()
        subsegments, config_end = read_overlay_subsegments(text)
        _sector, _sector_count, target_size, _sha12, _sha1 = target_slice_info(
            candidate, config.target_slice
        )
        end = config_end if config_end is not None else target_size

        c_bytes = 0
        asm_bytes = 0
        other_bytes = 0
        c_segments: list[str] = []
        asm_segments: list[str] = []
        for index, segment in enumerate(subsegments):
            next_start = subsegments[index + 1].start if index + 1 < len(subsegments) else end
            size = max(0, next_start - segment.start)
            if segment.kind == "c":
                c_bytes += size
                c_segments.append(segment.name)
            elif segment.kind == "asm":
                asm_bytes += size
                asm_segments.append(segment.name)
            else:
                other_bytes += size

        statuses.append(
            OverlayDecompStatus(
                overlay_id=overlay_id,
                target_slice=config.target_slice,
                target_size=target_size,
                c_bytes=c_bytes,
                asm_bytes=asm_bytes,
                other_bytes=other_bytes,
                c_segments=c_segments,
                asm_segments=asm_segments,
            )
        )
    return statuses


def format_percent(part: int, total: int) -> str:
    if total <= 0:
        return "0.00%"
    return f"{(part * 100.0 / total):.2f}%"


OVERLAY_MATCH_CAVEATS = {
    "ovl_006": (
        "resolve the scanner-slice function boundary before replacing ASM: "
        "func_80170000 starts without a local prologue and func_801703B0 "
        "reaches the 0x7FC slice end without an epilogue"
    ),
}


def configured_next_action(overlay_id: str) -> str:
    caveat = OVERLAY_MATCH_CAVEATS.get(overlay_id)
    command = f"make overlay-check OVERLAY={overlay_id}"
    if caveat is not None:
        return f"{caveat}, then verify with {command}"
    return f"replace asm with C, then run {command}"


def configured_worklist_action(overlay_id: str) -> str:
    caveat = OVERLAY_MATCH_CAVEATS.get(overlay_id)
    if caveat is not None:
        return caveat
    return "start C work on the smallest asm function, then run overlay-check"


def emit_decomp_status_markdown(candidates: list[OverlayCandidate], config_dir: Path) -> None:
    statuses = read_overlay_decomp_status(config_dir, candidates)
    total_size = sum(status.target_size for status in statuses)
    total_c = sum(status.c_bytes for status in statuses)
    total_asm = sum(status.asm_bytes for status in statuses)
    total_other = sum(status.other_bytes for status in statuses)

    print("# Overlay Decompilation Status")
    print()
    print(
        "Generated from configured overlay YAML subsegments. It tracks source/ASM "
        "coverage only for overlays that have trace-backed configs."
    )
    print()
    print("## Summary")
    print()
    print(f"- Configured overlays: {len(statuses)}")
    print(f"- Total configured bytes: {total_size}")
    print(f"- C bytes: {total_c} ({format_percent(total_c, total_size)})")
    print(f"- ASM bytes: {total_asm} ({format_percent(total_asm, total_size)})")
    print(f"- Other bytes: {total_other} ({format_percent(total_other, total_size)})")
    print()
    print("## Configured Overlay Coverage")
    print()
    print("| Overlay | Slice | Size | C bytes | C % | ASM bytes | ASM % | C segments |")
    print("|---|---|---:|---:|---:|---:|---:|---|")
    for status in statuses:
        c_names = ", ".join(f"`{name}`" for name in status.c_segments) or ""
        print(
            f"| `{status.overlay_id}` | `{status.target_slice}` | {status.target_size} | "
            f"{status.c_bytes} | {format_percent(status.c_bytes, status.target_size)} | "
            f"{status.asm_bytes} | {format_percent(status.asm_bytes, status.target_size)} | "
            f"{c_names} |"
        )
    print()
    print("## Next Action")
    print()
    if statuses:
        lowest = min(statuses, key=lambda status: status.c_bytes / max(1, status.target_size))
        print(f"- For `{lowest.overlay_id}`, {configured_next_action(lowest.overlay_id)}.")
    print(
        "- Add more overlay configs only after runtime traces prove source sector, "
        "load destination, and first executed PC."
    )


def emit_decomp_status_csv(candidates: list[OverlayCandidate], config_dir: Path) -> None:
    statuses = read_overlay_decomp_status(config_dir, candidates)
    writer = csv.writer(sys.stdout, lineterminator="\n")
    writer.writerow(
        [
            "overlay_id",
            "target_slice",
            "target_size",
            "c_bytes",
            "c_percent",
            "asm_bytes",
            "asm_percent",
            "other_bytes",
            "other_percent",
            "c_segments",
            "asm_segments",
        ]
    )
    for status in statuses:
        writer.writerow(
            [
                status.overlay_id,
                status.target_slice,
                status.target_size,
                status.c_bytes,
                format_percent(status.c_bytes, status.target_size),
                status.asm_bytes,
                format_percent(status.asm_bytes, status.target_size),
                status.other_bytes,
                format_percent(status.other_bytes, status.target_size),
                ";".join(status.c_segments),
                ";".join(status.asm_segments),
            ]
        )


MATCH_TARGET_FIELDS = [
    "overlay_id",
    "target_state",
    "configured",
    "byte_checkable",
    "target_slice",
    "target_sector",
    "target_sector_count",
    "target_size",
    "configured_vram",
    "c_bytes",
    "c_percent",
    "asm_bytes",
    "asm_percent",
    "blocking_evidence",
    "next_action",
    "command",
]

MATCH_WORKLIST_FIELDS = [
    "rank",
    "overlay_id",
    "target_state",
    "match_status",
    "target_slice",
    "target_sector",
    "target_size",
    "configured",
    "byte_checkable",
    "boundary_risk",
    "boundary_flags",
    "first_function_symbol",
    "first_function_offset",
    "first_function_size",
    "first_c_target_symbol",
    "first_c_target_size",
    "first_c_target_action",
    "function_hint_count",
    "blocking_evidence",
    "immediate_action",
    "trace_or_check_command",
]


def match_target_state(candidate: OverlayCandidate, configured: bool) -> str:
    if configured:
        return "byte-checkable"
    if candidate.load_vram is not None and candidate.entry_vram is not None:
        return "trace-backed-needs-config"
    if candidate.room_id is not None:
        return "owner-known-needs-vram"
    if "tiny" in candidate.tags:
        return "confirm-execution"
    return "runtime-trace-needed"


def match_target_rows(
    candidates: list[OverlayCandidate], config_dir: Path
) -> list[dict[str, str | int]]:
    configured = read_overlay_configs(config_dir, candidates)
    decomp_by_id = {
        status.overlay_id: status
        for status in read_overlay_decomp_status(config_dir, candidates)
    }
    rows: list[dict[str, str | int]] = []
    for candidate in candidates:
        target_sector, target_sector_count, target_size, _target_sha, _target_sha_full = (
            target_slice_info(candidate, candidate.recommended_target_slice)
        )
        config = configured.get(candidate.overlay_id)
        decomp = decomp_by_id.get(candidate.overlay_id)
        flags = evidence_booleans(candidate, configured)
        target_state = match_target_state(candidate, config is not None)
        if config is not None:
            command = f"make overlay-check OVERLAY={candidate.overlay_id}"
            next_action = configured_next_action(candidate.overlay_id)
        elif target_state == "trace-backed-needs-config":
            command = trace_command(candidate)
            next_action = "write overlay config from confirmed trace, then run overlay-check"
        else:
            command = trace_command(candidate)
            next_action = evidence_next_action(candidate, flags)
        c_bytes = decomp.c_bytes if decomp is not None else 0
        asm_bytes = decomp.asm_bytes if decomp is not None else 0
        rows.append(
            {
                "overlay_id": candidate.overlay_id,
                "target_state": target_state,
                "configured": "yes" if config is not None else "no",
                "byte_checkable": "yes" if config is not None else "no",
                "target_slice": candidate.recommended_target_slice,
                "target_sector": target_sector,
                "target_sector_count": target_sector_count,
                "target_size": target_size,
                "configured_vram": f"0x{config.vram:08X}" if config is not None else "",
                "c_bytes": c_bytes,
                "c_percent": format_percent(c_bytes, target_size if config is not None else 0),
                "asm_bytes": asm_bytes,
                "asm_percent": format_percent(asm_bytes, target_size if config is not None else 0),
                "blocking_evidence": blocking_evidence(candidate, flags),
                "next_action": next_action,
                "command": command,
            }
        )
    return rows


def emit_match_targets_csv(candidates: list[OverlayCandidate], config_dir: Path) -> None:
    writer = csv.DictWriter(sys.stdout, fieldnames=MATCH_TARGET_FIELDS)
    writer.writeheader()
    writer.writerows(match_target_rows(candidates, config_dir))


def emit_match_targets_markdown(candidates: list[OverlayCandidate], config_dir: Path) -> None:
    rows = match_target_rows(candidates, config_dir)
    state_counts = Counter(str(row["target_state"]) for row in rows)
    configured = [row for row in rows if row["configured"] == "yes"]
    configured_size = sum(int(row["target_size"]) for row in configured)
    configured_c = sum(int(row["c_bytes"]) for row in configured)

    print("# Overlay Match Target Coverage")
    print()
    print(
        "Generated from catalog metadata, committed overlay configs, and configured "
        "overlay subsegments. This is the one-page coverage view for turning every "
        "PE.IMG code candidate into an FF8-style split/build/check target."
    )
    print()
    print("## Summary")
    print()
    print(f"- Overlay candidates: {len(rows)}")
    print(f"- Byte-checkable targets: {len(configured)}")
    print(f"- Configured target bytes: {configured_size}")
    print(f"- Configured C bytes: {configured_c} ({format_percent(configured_c, configured_size)})")
    print("- Target states:")
    for state, count in sorted(state_counts.items()):
        print(f"  - `{state}`: {count}")
    print()
    print("## Targets")
    print()
    print(
        "| Overlay | State | Checkable | Slice | Sector | Size | C % | "
        "Blocking evidence | Next action | Command |"
    )
    print("|---|---|---|---|---:|---:|---:|---|---|---|")
    for row in rows:
        print(
            f"| `{row['overlay_id']}` | `{row['target_state']}` | "
            f"{row['byte_checkable']} | `{row['target_slice']}` | "
            f"{row['target_sector']} | {row['target_size']} | {row['c_percent']} | "
            f"`{row['blocking_evidence']}` | {row['next_action']} | "
            f"`{row['command']}` |"
        )


def match_worklist_rows(
    pe_img: Path,
    candidates: list[OverlayCandidate],
    config_dir: Path,
) -> list[dict[str, str | int]]:
    targets_by_overlay = {
        str(row["overlay_id"]): row for row in match_target_rows(candidates, config_dir)
    }
    boundary_by_overlay = {
        str(row["overlay_id"]): row
        for row in function_boundary_audit_rows(pe_img, candidates, config_dir)
    }
    first_function_by_overlay: dict[str, dict[str, str | int]] = {}
    for row in static_function_queue_rows(pe_img, candidates, config_dir):
        first_function_by_overlay.setdefault(str(row["overlay_id"]), row)
    first_c_target_by_overlay: dict[str, dict[str, str]] = {}
    if FUNCTION_KIND_AUDIT_CSV.exists():
        with FUNCTION_KIND_AUDIT_CSV.open(newline="") as handle:
            c_targets = [
                row for row in csv.DictReader(handle) if row.get("match_class") == "c-target"
            ]
        c_targets.sort(key=lambda row: (int(row["size_bytes"]), row["overlay_id"], row["symbol"]))
        for row in c_targets:
            first_c_target_by_overlay.setdefault(row["overlay_id"], row)

    risk_order = {"low": 0, "medium": 1, "high": 2}
    state_order = {
        "byte-checkable": 0,
        "runtime-trace-needed": 1,
        "owner-known-needs-vram": 2,
        "confirm-execution": 3,
    }
    rows: list[dict[str, str | int]] = []
    for candidate in candidates:
        target = targets_by_overlay[candidate.overlay_id]
        boundary = boundary_by_overlay[candidate.overlay_id]
        first_function = first_function_by_overlay.get(candidate.overlay_id)
        first_c_target = first_c_target_by_overlay.get(candidate.overlay_id)
        target_state = str(target["target_state"])
        boundary_risk = str(boundary["risk_level"])
        if target_state == "byte-checkable":
            if first_c_target is not None:
                immediate_action = (
                    f"start C work on {first_c_target['symbol']}: "
                    f"{first_c_target['next_action']}"
                )
            else:
                immediate_action = configured_worklist_action(candidate.overlay_id)
        elif target_state == "confirm-execution":
            immediate_action = "confirm execution before committing an overlay config"
        elif boundary_risk == "high":
            immediate_action = "capture trace, generate config, then review split/data boundaries before C"
        else:
            immediate_action = "capture trace, generate config, then start from first_function_symbol"
        rows.append(
            {
                "rank": 0,
                "overlay_id": candidate.overlay_id,
                "target_state": target_state,
                "match_status": candidate.match_status,
                "target_slice": target["target_slice"],
                "target_sector": target["target_sector"],
                "target_size": target["target_size"],
                "configured": target["configured"],
                "byte_checkable": target["byte_checkable"],
                "boundary_risk": boundary_risk,
                "boundary_flags": boundary["risk_flags"],
                "first_function_symbol": (
                    first_function["suggested_symbol"] if first_function is not None else ""
                ),
                "first_function_offset": (
                    first_function["function_offset"] if first_function is not None else ""
                ),
                "first_function_size": (
                    first_function["estimated_size_bytes"] if first_function is not None else ""
                ),
                "first_c_target_symbol": first_c_target["symbol"] if first_c_target is not None else "",
                "first_c_target_size": (
                    first_c_target["size_bytes"] if first_c_target is not None else ""
                ),
                "first_c_target_action": (
                    first_c_target["next_action"] if first_c_target is not None else ""
                ),
                "function_hint_count": boundary["function_hint_count"],
                "blocking_evidence": target["blocking_evidence"],
                "immediate_action": immediate_action,
                "trace_or_check_command": target["command"],
            }
        )
    rows.sort(
        key=lambda row: (
            state_order.get(str(row["target_state"]), 99),
            risk_order.get(str(row["boundary_risk"]), 99),
            int(row["first_function_size"] or 1 << 30),
            str(row["overlay_id"]),
        )
    )
    for rank, row in enumerate(rows, start=1):
        row["rank"] = rank
    return rows


def emit_match_worklist_csv(
    pe_img: Path,
    candidates: list[OverlayCandidate],
    config_dir: Path,
) -> None:
    writer = csv.DictWriter(sys.stdout, fieldnames=MATCH_WORKLIST_FIELDS)
    writer.writeheader()
    writer.writerows(match_worklist_rows(pe_img, candidates, config_dir))


def emit_match_worklist_markdown(
    pe_img: Path,
    candidates: list[OverlayCandidate],
    config_dir: Path,
) -> None:
    rows = match_worklist_rows(pe_img, candidates, config_dir)
    state_counts = Counter(str(row["target_state"]) for row in rows)
    risk_counts = Counter(str(row["boundary_risk"]) for row in rows)
    print("# Overlay Match Worklist")
    print()
    print(
        "Generated from match-target coverage, static function queue, and function "
        "boundary audit. This is the per-overlay handoff list for getting from "
        "trace evidence to the first practical C match target."
    )
    print()
    print(f"- Worklist rows: {len(rows)}")
    print("- CSV source: `docs/assets/overlay-match-worklist.csv`")
    print("- Trace/check template: `docs/assets/overlay-match-worklist-trace-template.csv`")
    print("- Match targets: `docs/assets/overlay-match-targets.md`")
    print("- Static function queue: `docs/assets/overlay-static-function-queue.md`")
    print("- Function boundary audit: `docs/assets/overlay-function-boundary-audit.md`")
    print("- Function kind audit: `docs/assets/overlay-function-kind-audit.md`")
    print("- Target states:")
    for state, count in sorted(state_counts.items()):
        print(f"  - `{state}`: {count}")
    print("- Boundary risk:")
    for risk, count in sorted(risk_counts.items()):
        print(f"  - `{risk}`: {count}")
    print()
    print(
        "| Rank | Overlay | State | Risk | Slice | First static | Static size | "
        "First C target | C size | "
        "Blocking evidence | Immediate action | Command |"
    )
    print("|---:|---|---|---|---|---|---:|---|---:|---|---|---|")
    for row in rows:
        print(
            f"| {row['rank']} | `{row['overlay_id']}` | `{row['target_state']}` | "
            f"`{row['boundary_risk']}` | `{row['target_slice']}` | "
            f"`{row['first_function_symbol']}` | {row['first_function_size']} | "
            f"`{row['first_c_target_symbol']}` | {row['first_c_target_size']} | "
            f"{row['blocking_evidence']} | {row['immediate_action']} | "
            f"`{row['trace_or_check_command']}` |"
        )


def emit_config_markdown(candidates: list[OverlayCandidate], config_dir: Path) -> None:
    configured = read_overlay_configs(config_dir, candidates)
    pending = [candidate for candidate in candidates if candidate.overlay_id not in configured]
    pending_statuses = Counter(candidate.match_status for candidate in pending)

    print("# Overlay Match Configuration Status")
    print()
    print(
        "Generated from `tools/scripts/catalog_pe_overlays.py --format config-md`. "
        "This tracks which PE.IMG code candidates have committed FF8-style splat "
        "configs and which candidates still need runtime evidence before matching."
    )
    print()
    print("## Summary")
    print()
    print(f"- Total code-overlay candidates: {len(candidates)}")
    print(f"- Configured match targets: {len(configured)}")
    print(f"- Pending candidates: {len(pending)}")
    print("- Pending by status:")
    for status, count in sorted(pending_statuses.items()):
        print(f"  - `{status}`: {count}")
    print()

    print("## Configured Targets")
    print()
    if configured:
        print("| Overlay | Slice | VRAM | Role | Check |")
        print("|---|---|---:|---|---|")
        for overlay_id in sorted(configured):
            status = configured[overlay_id]
            candidate = next(candidate for candidate in candidates if candidate.overlay_id == overlay_id)
            print(
                f"| `{overlay_id}` | `{status.target_slice}` | `0x{status.vram:08X}` | "
                f"{candidate.role_hypothesis} | `make overlay-check OVERLAY={overlay_id}` |"
            )
    else:
        print("No committed overlay configs yet.")
    print()

    print("## Next Config Candidates")
    print()
    print("| Overlay | Status | Recommended slice | PE.IMG sector | Sectors | Role | Init command |")
    print("|---|---|---|---:|---:|---|---|")
    for candidate in sorted(pending, key=priority_score, reverse=True)[:20]:
        sector, sector_count, _size, _sha, _sha_full = target_slice_info(
            candidate, candidate.recommended_target_slice
        )
        print(
            f"| `{candidate.overlay_id}` | `{candidate.match_status}` | "
            f"`{candidate.recommended_target_slice}` | {sector} | {sector_count} | "
            f"{candidate.role_hypothesis} | `{trace_command(candidate)}` |"
        )
    print()
    print("## Policy")
    print()
    print(
        "Only commit a new `configs/USA/overlays/*.yaml` after a runtime trace proves "
        "the source sector, sector count, load destination, and executable VRAM. "
        "Generated `original/`, `asm/`, `linkers/`, and `build/` outputs remain local."
    )


def evidence_needed(candidate: OverlayCandidate, configured: bool) -> str:
    if configured:
        return "configured; run overlay-check-all and start replacing asm with C"
    if candidate.default_vram is not None:
        return "commit generated config and run overlay-check"
    if candidate.room_id is not None:
        return "trace runtime load destination and execution PC for room-section3 payload"
    if "tiny" in candidate.tags:
        return "validate this tiny code-like block with runtime trace before matching"
    return "trace runtime CD read, owner/caller, load destination, and execution PC"


PLAN_FIELDS = [
    "overlay_id",
    "configured",
    "match_status",
    "role_hypothesis",
    "role_confidence",
    "recommended_target_slice",
    "target_sector",
    "target_sector_count",
    "target_size_kib",
    "room",
    "default_vram",
    "configured_vram",
    "evidence_needed",
    "init_command",
    "check_command",
    "duplicate_group",
    "top_calls",
    "call_domains",
]


def plan_rows(candidates: list[OverlayCandidate], config_dir: Path) -> list[dict[str, str | int]]:
    configured = read_overlay_configs(config_dir, candidates)
    rows: list[dict[str, str | int]] = []
    for candidate in candidates:
        target_sector, target_sector_count, target_size, _target_sha, _target_sha_full = target_slice_info(
            candidate, candidate.recommended_target_slice
        )
        config = configured.get(candidate.overlay_id)
        is_configured = config is not None
        rows.append(
            {
                "overlay_id": candidate.overlay_id,
                "configured": "yes" if is_configured else "no",
                "match_status": candidate.match_status,
                "role_hypothesis": candidate.role_hypothesis,
                "role_confidence": candidate.role_confidence,
                "recommended_target_slice": candidate.recommended_target_slice,
                "target_sector": target_sector,
                "target_sector_count": target_sector_count,
                "target_size_kib": target_size // 1024,
                "room": room_label(candidate),
                "default_vram": (
                    f"0x{candidate.default_vram:08X}"
                    if candidate.default_vram is not None
                    else ""
                ),
                "configured_vram": f"0x{config.vram:08X}" if config is not None else "",
                "evidence_needed": evidence_needed(candidate, is_configured),
                "init_command": (
                    f"make overlay-check OVERLAY={candidate.overlay_id}"
                    if is_configured
                    else trace_command(candidate)
                ),
                "check_command": f"make overlay-check OVERLAY={candidate.overlay_id}",
                "duplicate_group": candidate.duplicate_group,
                "top_calls": candidate.top_calls,
                "call_domains": candidate.call_domains,
            }
        )
    return rows


def emit_plan_csv(candidates: list[OverlayCandidate], config_dir: Path) -> None:
    writer = csv.DictWriter(sys.stdout, fieldnames=PLAN_FIELDS)
    writer.writeheader()
    writer.writerows(plan_rows(candidates, config_dir))


def emit_plan_markdown(candidates: list[OverlayCandidate], config_dir: Path) -> None:
    rows = plan_rows(candidates, config_dir)
    print("# Overlay Match Plan")
    print()
    print(
        "Generated from `tools/scripts/catalog_pe_overlays.py --format plan-md`. "
        "This is the human-readable one-row-per-overlay plan for moving each "
        "PE.IMG candidate toward an FF8-style split/build/check target."
    )
    print()
    print(f"- Plan rows: {len(rows)}")
    print("- CSV source: `docs/assets/overlay-match-plan.csv`")
    print("- Complete trace backlog: `docs/assets/overlay-trace-sheet-all.csv`")
    print()
    print(
        "| Overlay | Configured | Status | Slice | Sector | Sectors | Room | "
        "Evidence needed | First command |"
    )
    print("|---|---|---|---|---:|---:|---|---|---|")
    for row in rows:
        room = f"`{row['room']}`" if row["room"] else ""
        print(
            f"| `{row['overlay_id']}` | {row['configured']} | `{row['match_status']}` | "
            f"`{row['recommended_target_slice']}` | {row['target_sector']} | "
            f"{row['target_sector_count']} | {room} | {row['evidence_needed']} | "
            f"`{row['init_command']}` |"
        )


EVIDENCE_FIELDS = [
    "overlay_id",
    "configured",
    "byte_checkable",
    "catalog_source_known",
    "runtime_read_proven",
    "load_vram_proven",
    "entry_pc_proven",
    "canonical_base_proven",
    "room_owner_known",
    "target_slice",
    "target_sector",
    "target_sector_count",
    "target_sha1_12",
    "load_vram",
    "entry_vram",
    "canonical_vram",
    "blocking_evidence",
    "next_action",
]


def evidence_booleans(
    candidate: OverlayCandidate, configured: dict[str, OverlayConfigStatus]
) -> dict[str, bool]:
    is_configured = candidate.overlay_id in configured
    return {
        "configured": is_configured,
        "byte_checkable": is_configured,
        "catalog_source_known": True,
        "runtime_read_proven": is_configured or candidate.load_vram is not None,
        "load_vram_proven": is_configured or candidate.load_vram is not None,
        "entry_pc_proven": candidate.entry_vram is not None,
        "canonical_base_proven": candidate.canonical_vram is not None,
        "room_owner_known": candidate.room_id is not None,
    }


def blocking_evidence(candidate: OverlayCandidate, flags: dict[str, bool]) -> str:
    missing: list[str] = []
    if not flags["runtime_read_proven"]:
        missing.append("runtime-read")
    if not flags["load_vram_proven"]:
        missing.append("load-vram")
    if not flags["entry_pc_proven"]:
        missing.append("entry-pc")
    if candidate.room_id is None and "tiny" in candidate.tags:
        missing.append("execution-confirmation")
    if not flags["configured"]:
        missing.append("overlay-config")
    if not missing:
        return "none"
    return ";".join(missing)


def evidence_next_action(candidate: OverlayCandidate, flags: dict[str, bool]) -> str:
    if flags["byte_checkable"]:
        return f"replace asm with C, then run make overlay-check OVERLAY={candidate.overlay_id}"
    if candidate.room_id is not None:
        return "trace room section-3 load destination and first executed PC"
    if "tiny" in candidate.tags:
        return "confirm this tiny scanner hit executes before creating a config"
    return "trace CD read caller/source, load destination, and first executed PC"


def evidence_rows(
    candidates: list[OverlayCandidate], config_dir: Path
) -> list[dict[str, str | int]]:
    configured = read_overlay_configs(config_dir, candidates)
    rows: list[dict[str, str | int]] = []
    for candidate in candidates:
        target_sector, target_sector_count, _target_size, target_sha, _target_sha_full = (
            target_slice_info(candidate, candidate.recommended_target_slice)
        )
        flags = evidence_booleans(candidate, configured)
        rows.append(
            {
                "overlay_id": candidate.overlay_id,
                "configured": "yes" if flags["configured"] else "no",
                "byte_checkable": "yes" if flags["byte_checkable"] else "no",
                "catalog_source_known": "yes" if flags["catalog_source_known"] else "no",
                "runtime_read_proven": "yes" if flags["runtime_read_proven"] else "no",
                "load_vram_proven": "yes" if flags["load_vram_proven"] else "no",
                "entry_pc_proven": "yes" if flags["entry_pc_proven"] else "no",
                "canonical_base_proven": "yes" if flags["canonical_base_proven"] else "no",
                "room_owner_known": "yes" if flags["room_owner_known"] else "no",
                "target_slice": candidate.recommended_target_slice,
                "target_sector": target_sector,
                "target_sector_count": target_sector_count,
                "target_sha1_12": target_sha,
                "load_vram": (
                    f"0x{candidate.load_vram:08X}"
                    if candidate.load_vram is not None
                    else ""
                ),
                "entry_vram": (
                    f"0x{candidate.entry_vram:08X}"
                    if candidate.entry_vram is not None
                    else ""
                ),
                "canonical_vram": (
                    f"0x{candidate.canonical_vram:08X}"
                    if candidate.canonical_vram is not None
                    else ""
                ),
                "blocking_evidence": blocking_evidence(candidate, flags),
                "next_action": evidence_next_action(candidate, flags),
            }
        )
    return rows


def emit_evidence_csv(candidates: list[OverlayCandidate], config_dir: Path) -> None:
    writer = csv.DictWriter(sys.stdout, fieldnames=EVIDENCE_FIELDS)
    writer.writeheader()
    writer.writerows(evidence_rows(candidates, config_dir))


def emit_evidence_markdown(candidates: list[OverlayCandidate], config_dir: Path) -> None:
    rows = evidence_rows(candidates, config_dir)
    flag_fields = [
        "byte_checkable",
        "catalog_source_known",
        "runtime_read_proven",
        "load_vram_proven",
        "entry_pc_proven",
        "canonical_base_proven",
        "room_owner_known",
    ]
    counts = {
        field: sum(1 for row in rows if row[field] == "yes")
        for field in flag_fields
    }

    print("# Overlay Evidence Matrix")
    print()
    print(
        "Generated from `tools/scripts/catalog_pe_overlays.py --format evidence-md`. "
        "This is the per-overlay evidence checklist for deciding whether a PE.IMG "
        "code candidate can be turned into an FF8-style split/build/check target."
    )
    print()
    print(f"- Matrix rows: {len(rows)}")
    print("- CSV source: `docs/assets/overlay-evidence-matrix.csv`")
    print("- Full trace backlog: `docs/assets/overlay-trace-sheet-all.csv`")
    print()
    print("## Evidence Counts")
    print()
    print("| Evidence | Yes | Missing |")
    print("|---|---:|---:|")
    for field in flag_fields:
        yes = counts[field]
        print(f"| `{field}` | {yes} | {len(rows) - yes} |")
    print()
    print("## Matrix")
    print()
    print(
        "| Overlay | Checkable | Source | Read | Load VRAM | Entry PC | Canonical | "
        "Owner | Slice | Sector | Blocking evidence | Next action |"
    )
    print("|---|---|---|---|---|---|---|---|---|---:|---|---|")
    for row in rows:
        print(
            f"| `{row['overlay_id']}` | {row['byte_checkable']} | "
            f"{row['catalog_source_known']} | {row['runtime_read_proven']} | "
            f"{row['load_vram_proven']} | {row['entry_pc_proven']} | "
            f"{row['canonical_base_proven']} | {row['room_owner_known']} | "
            f"`{row['target_slice']}` | {row['target_sector']} | "
            f"`{row['blocking_evidence']}` | {row['next_action']} |"
        )


SCANNER_AUDIT_FIELDS = [
    "sector",
    "offset_hex",
    "jrra",
    "prologues",
    "text_calls",
    "category",
    "nearest_overlay",
    "nearest_distance",
    "covered_by_recommended_slice",
    "covering_overlay",
    "note",
]


def scanner_excluded_category(row: dict[str, int | bool]) -> str:
    jrra = int(row["jrra"])
    prologues = int(row["prologues"])
    text_calls = int(row["text_calls"])
    if bool(row["scanner_included"]):
        return "scanner-included"
    if jrra >= 1 and text_calls >= 1:
        return "jr+jalt-below-threshold"
    if jrra >= 1:
        return "jr-only-no-prologue-low-jal"
    if prologues >= 1 and text_calls >= 1:
        return "prologue+jalt-no-jr"
    if prologues >= 1:
        return "prologue-only-no-jr"
    if text_calls >= 1:
        return "jal-only-no-jr"
    return "no-code-signals"


def nearest_candidate(
    sector: int, candidates: list[OverlayCandidate]
) -> tuple[OverlayCandidate, int]:
    def distance(candidate: OverlayCandidate) -> int:
        if candidate.sector_start <= sector <= candidate.sector_end:
            return 0
        return min(abs(sector - candidate.sector_start), abs(sector - candidate.sector_end))

    candidate = min(candidates, key=distance)
    return candidate, distance(candidate)


def target_slice_contains(candidate: OverlayCandidate, sector: int) -> bool:
    target_sector, target_count, _size, _sha, _sha_full = target_slice_info(
        candidate, candidate.recommended_target_slice
    )
    return target_sector <= sector < target_sector + target_count


def scanner_audit_rows(
    pe_img: Path, candidates: list[OverlayCandidate]
) -> tuple[Counter[str], list[dict[str, str | int]]]:
    _nsec, sector_rows = scan_sector_metrics(pe_img)
    summary = Counter(scanner_excluded_category(row) for row in sector_rows)
    audit_rows: list[dict[str, str | int]] = []
    for row in sector_rows:
        category = scanner_excluded_category(row)
        if category not in {
            "jr+jalt-below-threshold",
            "jr-only-no-prologue-low-jal",
            "prologue+jalt-no-jr",
        }:
            continue
        sector = int(row["sector"])
        nearest, nearest_distance = nearest_candidate(sector, candidates)
        covering = [
            candidate
            for candidate in candidates
            if target_slice_contains(candidate, sector)
        ]
        covering_overlay = ",".join(candidate.overlay_id for candidate in covering)
        covered = "yes" if covering else "no"
        if covered == "yes":
            note = "covered-by-recommended-target-slice"
        elif nearest_distance <= 2:
            note = "adjacent-to-catalog-block-review-boundary"
        else:
            note = "isolated-near-miss-review"
        audit_rows.append(
            {
                "sector": sector,
                "offset_hex": f"0x{sector * SECTOR_SIZE:08X}",
                "jrra": int(row["jrra"]),
                "prologues": int(row["prologues"]),
                "text_calls": int(row["text_calls"]),
                "category": category,
                "nearest_overlay": nearest.overlay_id,
                "nearest_distance": nearest_distance,
                "covered_by_recommended_slice": covered,
                "covering_overlay": covering_overlay,
                "note": note,
            }
        )
    audit_rows.sort(
        key=lambda item: (
            str(item["covered_by_recommended_slice"]) != "no",
            int(item["nearest_distance"]),
            int(item["sector"]),
        )
    )
    return summary, audit_rows


def emit_scanner_audit_csv(pe_img: Path, candidates: list[OverlayCandidate]) -> None:
    _summary, rows = scanner_audit_rows(pe_img, candidates)
    writer = csv.DictWriter(sys.stdout, fieldnames=SCANNER_AUDIT_FIELDS)
    writer.writeheader()
    writer.writerows(rows)


def emit_scanner_audit_markdown(pe_img: Path, candidates: list[OverlayCandidate]) -> None:
    summary, rows = scanner_audit_rows(pe_img, candidates)
    covered = sum(1 for row in rows if row["covered_by_recommended_slice"] == "yes")
    uncovered = len(rows) - covered

    print("# Overlay Scanner Audit")
    print()
    print(
        "Generated from `tools/scripts/catalog_pe_overlays.py --format scanner-audit-md`. "
        "This records code-like PE.IMG sectors that did not become standalone overlay "
        "candidates under the main scanner threshold."
    )
    print()
    print("## Scanner Threshold")
    print()
    print(
        "A sector is selected by the main scanner when it has at least one `jr $ra` "
        "and either at least one stack prologue or at least three direct calls. "
        "Selected sectors are merged into overlay candidate blocks when separated "
        "by at most two sectors."
    )
    print()
    print(f"- Cataloged overlay blocks: {len(candidates)}")
    print(f"- Direct scanner-hit sectors: {summary['scanner-included']}")
    print(f"- Near-miss review sectors: {len(rows)}")
    print(f"- Covered by recommended target slices: {covered}")
    print(f"- Uncovered near-miss sectors: {uncovered}")
    print()
    print("## PE.IMG Sector Signal Summary")
    print()
    print("| Category | Sectors | Included in review table |")
    print("|---|---:|---|")
    for category, count in sorted(summary.items()):
        included = (
            "yes"
            if category
            in {
                "jr+jalt-below-threshold",
                "jr-only-no-prologue-low-jal",
                "prologue+jalt-no-jr",
            }
            else "no"
        )
        print(f"| `{category}` | {count} | {included} |")
    print()
    print("## Near-Miss Review")
    print()
    print(
        "| Sector | jrra | prologues | calls | Category | Nearest overlay | "
        "Distance | Covered | Covering overlay | Note |"
    )
    print("|---:|---:|---:|---:|---|---|---:|---|---|---|")
    for row in rows:
        covering_overlay = f"`{row['covering_overlay']}`" if row["covering_overlay"] else ""
        print(
            f"| {row['sector']} | {row['jrra']} | {row['prologues']} | "
            f"{row['text_calls']} | `{row['category']}` | "
            f"`{row['nearest_overlay']}` | {row['nearest_distance']} | "
            f"{row['covered_by_recommended_slice']} | "
            f"{covering_overlay} | "
            f"{row['note']} |"
        )


CALL_MAP_FIELDS = [
    "target_addr",
    "target_name",
    "domain",
    "overlay_count",
    "total_calls",
    "target_slices",
    "role_hypotheses",
    "example_overlays",
]


def call_map_rows(
    pe_img: Path,
    candidates: list[OverlayCandidate],
    sym_file: Path | None,
    config_yaml: Path | None,
) -> list[dict[str, str | int]]:
    image = pe_img.read_bytes()
    symbols = load_symbols(sym_file, config_yaml)
    by_target: dict[int, dict[str, object]] = {}
    for candidate in candidates:
        target_sector, _target_count, target_size, _sha, _sha_full = target_slice_info(
            candidate, candidate.recommended_target_slice
        )
        start = target_sector * SECTOR_SIZE
        data = image[start : start + target_size]
        for addr, count in call_counts(data).items():
            entry = by_target.setdefault(
                addr,
                {
                    "total_calls": 0,
                    "overlays": Counter(),
                    "target_slices": Counter(),
                    "role_hypotheses": Counter(),
                },
            )
            entry["total_calls"] = int(entry["total_calls"]) + count
            entry["overlays"][candidate.overlay_id] += count  # type: ignore[index]
            entry["target_slices"][candidate.recommended_target_slice] += 1  # type: ignore[index]
            entry["role_hypotheses"][candidate.role_hypothesis] += 1  # type: ignore[index]

    rows: list[dict[str, str | int]] = []
    for addr, entry in by_target.items():
        name = symbols.get(addr, f"0x{addr:08X}")
        overlays: Counter[str] = entry["overlays"]  # type: ignore[assignment]
        target_slices: Counter[str] = entry["target_slices"]  # type: ignore[assignment]
        role_hypotheses: Counter[str] = entry["role_hypotheses"]  # type: ignore[assignment]
        rows.append(
            {
                "target_addr": f"0x{addr:08X}",
                "target_name": name,
                "domain": call_domain(name, addr),
                "overlay_count": len(overlays),
                "total_calls": int(entry["total_calls"]),
                "target_slices": ";".join(
                    f"{target_slice}:{count}"
                    for target_slice, count in sorted(target_slices.items())
                ),
                "role_hypotheses": ";".join(
                    f"{role}:{count}"
                    for role, count in role_hypotheses.most_common(6)
                ),
                "example_overlays": ";".join(
                    overlay_id for overlay_id, _count in overlays.most_common(12)
                ),
            }
        )
    rows.sort(
        key=lambda row: (
            int(row["overlay_count"]),
            int(row["total_calls"]),
            str(row["target_name"]),
        ),
        reverse=True,
    )
    return rows


def emit_call_map_csv(
    pe_img: Path,
    candidates: list[OverlayCandidate],
    sym_file: Path | None,
    config_yaml: Path | None,
) -> None:
    writer = csv.DictWriter(sys.stdout, fieldnames=CALL_MAP_FIELDS)
    writer.writeheader()
    writer.writerows(call_map_rows(pe_img, candidates, sym_file, config_yaml))


def emit_call_map_markdown(
    pe_img: Path,
    candidates: list[OverlayCandidate],
    sym_file: Path | None,
    config_yaml: Path | None,
) -> None:
    rows = call_map_rows(pe_img, candidates, sym_file, config_yaml)
    domains = Counter(str(row["domain"]) for row in rows)
    calls_by_domain = Counter(
        {
            domain: sum(int(row["total_calls"]) for row in rows if row["domain"] == domain)
            for domain in domains
        }
    )

    print("# Overlay External Call Map")
    print()
    print(
        "Generated from `tools/scripts/catalog_pe_overlays.py --format call-map-md`. "
        "This groups PE.IMG overlay candidates by external call targets in their "
        "recommended match slice. It is evidence for role hypotheses and for "
        "choosing cohesive decompilation batches."
    )
    print()
    print(f"- Call targets: {len(rows)}")
    print("- CSV source: `docs/assets/overlay-call-map.csv`")
    print("- Overlay plan: `docs/assets/overlay-match-plan.md`")
    print()
    print("## Domains")
    print()
    print("| Domain | Targets | Calls |")
    print("|---|---:|---:|")
    for domain, target_count in domains.most_common():
        print(f"| `{domain}` | {target_count} | {calls_by_domain[domain]} |")
    print()
    print("## Highest-Coverage Call Targets")
    print()
    print(
        "| Target | Domain | Overlays | Calls | Slices | Example overlays | Roles |"
    )
    print("|---|---|---:|---:|---|---|---|")
    for row in rows[:80]:
        examples = str(row["example_overlays"]).replace(";", ", ")
        roles = str(row["role_hypotheses"]).replace(";", "; ")
        print(
            f"| `{row['target_name']}` (`{row['target_addr']}`) | `{row['domain']}` | "
            f"{row['overlay_count']} | {row['total_calls']} | "
            f"{row['target_slices']} | {examples} | {roles} |"
        )


DOMAIN_PLAN_FIELDS = [
    "overlay_id",
    "configured",
    "primary_domain",
    "primary_domain_calls",
    "secondary_domains",
    "role_hypothesis",
    "role_confidence",
    "recommended_target_slice",
    "target_sector",
    "target_sector_count",
    "room",
    "trace_or_check_command",
    "top_calls",
    "call_domains",
]


def parse_domain_counts(value: str) -> Counter[str]:
    counts: Counter[str] = Counter()
    for part in value.split(";"):
        part = part.strip()
        if not part:
            continue
        if ":" not in part:
            continue
        domain, count_text = part.rsplit(":", 1)
        try:
            counts[domain.strip()] += int(count_text)
        except ValueError:
            continue
    return counts


def domain_plan_rows(
    candidates: list[OverlayCandidate], config_dir: Path
) -> list[dict[str, str | int]]:
    configured = read_overlay_configs(config_dir, candidates)
    rows: list[dict[str, str | int]] = []
    for candidate in candidates:
        target_sector, target_sector_count, _size, _sha, _sha_full = target_slice_info(
            candidate, candidate.recommended_target_slice
        )
        domains = parse_domain_counts(candidate.call_domains)
        if domains:
            primary_domain, primary_calls = domains.most_common(1)[0]
            secondary_domains = ";".join(
                f"{domain}:{count}" for domain, count in domains.most_common()[1:5]
            )
        else:
            primary_domain = "none"
            primary_calls = 0
            secondary_domains = ""
        is_configured = candidate.overlay_id in configured
        rows.append(
            {
                "overlay_id": candidate.overlay_id,
                "configured": "yes" if is_configured else "no",
                "primary_domain": primary_domain,
                "primary_domain_calls": primary_calls,
                "secondary_domains": secondary_domains,
                "role_hypothesis": candidate.role_hypothesis,
                "role_confidence": candidate.role_confidence,
                "recommended_target_slice": candidate.recommended_target_slice,
                "target_sector": target_sector,
                "target_sector_count": target_sector_count,
                "room": room_label(candidate),
                "trace_or_check_command": (
                    f"make overlay-check OVERLAY={candidate.overlay_id}"
                    if is_configured
                    else trace_command(candidate)
                ),
                "top_calls": candidate.top_calls,
                "call_domains": candidate.call_domains,
            }
        )
    rows.sort(
        key=lambda row: (
            str(row["primary_domain"]),
            -int(row["primary_domain_calls"]),
            str(row["overlay_id"]),
        )
    )
    return rows


def emit_domain_plan_csv(candidates: list[OverlayCandidate], config_dir: Path) -> None:
    writer = csv.DictWriter(sys.stdout, fieldnames=DOMAIN_PLAN_FIELDS)
    writer.writeheader()
    writer.writerows(domain_plan_rows(candidates, config_dir))


def emit_domain_plan_markdown(candidates: list[OverlayCandidate], config_dir: Path) -> None:
    rows = domain_plan_rows(candidates, config_dir)
    domains = Counter(str(row["primary_domain"]) for row in rows)
    calls = Counter(
        {
            domain: sum(
                int(row["primary_domain_calls"])
                for row in rows
                if row["primary_domain"] == domain
            )
            for domain in domains
        }
    )

    print("# Overlay Domain Plan")
    print()
    print(
        "Generated from `tools/scripts/catalog_pe_overlays.py --format domain-plan-md`. "
        "This is a one-row-per-overlay batching view based on the dominant external "
        "call domain in each recommended target slice."
    )
    print()
    print(f"- Overlay rows: {len(rows)}")
    print("- CSV source: `docs/assets/overlay-domain-plan.csv`")
    print("- External call target map: `docs/assets/overlay-call-map.md`")
    print()
    print("## Primary Domain Summary")
    print()
    print("| Primary domain | Overlays | Primary-domain calls |")
    print("|---|---:|---:|")
    for domain, count in domains.most_common():
        print(f"| `{domain}` | {count} | {calls[domain]} |")
    print()
    print("## Suggested Domain Batches")
    print()
    print("| Domain | Top overlays | Why |")
    print("|---|---|---|")
    for domain, _count in domains.most_common():
        if domain == "none":
            continue
        domain_rows = [row for row in rows if row["primary_domain"] == domain]
        top = sorted(domain_rows, key=lambda row: int(row["primary_domain_calls"]), reverse=True)[:10]
        overlays = ", ".join(f"`{row['overlay_id']}`" for row in top)
        print(
            f"| `{domain}` | {overlays} | "
            f"{len(domain_rows)} overlays, {calls[domain]} primary-domain calls |"
        )
    print()
    print("## Per-Overlay Plan")
    print()
    print(
        "| Overlay | Primary domain | Calls | Secondary domains | Role | Slice | "
        "Sector | Room | Command |"
    )
    print("|---|---|---:|---|---|---|---:|---|---|")
    for row in rows:
        room = f"`{row['room']}`" if row["room"] else ""
        print(
            f"| `{row['overlay_id']}` | `{row['primary_domain']}` | "
            f"{row['primary_domain_calls']} | {row['secondary_domains']} | "
            f"{row['role_hypothesis']} | `{row['recommended_target_slice']}` | "
            f"{row['target_sector']} | {room} | `{row['trace_or_check_command']}` |"
        )


FAMILY_FIELDS = [
    "shape",
    "count",
    "representative",
    "member_overlays",
    "match_statuses",
    "role_hypotheses",
    "role_confidences",
    "recommended_target_slices",
    "rooms",
    "top_calls",
    "call_domains",
    "first_command",
]

FAMILY_MATCH_WORKLIST_FIELDS = [
    "rank",
    "representative",
    "member_count",
    "reuse_siblings",
    "target_state",
    "boundary_risk",
    "target_slice",
    "first_function_symbol",
    "first_function_offset",
    "first_function_size",
    "representative_function_hints",
    "family_function_hints",
    "family_target_size_bytes",
    "match_statuses",
    "boundary_risks",
    "target_slices",
    "role_hypotheses",
    "rooms",
    "member_overlays",
    "trace_or_check_command",
    "next_action",
]


def family_rows(candidates: list[OverlayCandidate]) -> list[dict[str, str | int]]:
    families: dict[str, list[OverlayCandidate]] = {}
    for candidate in candidates:
        families.setdefault(candidate.shape, []).append(candidate)

    repeated = sorted(
        (group for group in families.values() if len(group) >= 2),
        key=lambda group: (len(group), max(priority_score(candidate) for candidate in group)),
        reverse=True,
    )

    rows: list[dict[str, str | int]] = []
    for group in repeated:
        representative = max(group, key=priority_score)
        rooms = sorted({room_label(candidate) for candidate in group if room_label(candidate)})
        rows.append(
            {
                "shape": representative.shape,
                "count": len(group),
                "representative": representative.overlay_id,
                "member_overlays": ";".join(candidate.overlay_id for candidate in group),
                "match_statuses": ";".join(
                    f"{status}:{count}"
                    for status, count in sorted(
                        Counter(candidate.match_status for candidate in group).items()
                    )
                ),
                "role_hypotheses": ";".join(
                    f"{role}:{count}"
                    for role, count in sorted(
                        Counter(candidate.role_hypothesis for candidate in group).items()
                    )
                ),
                "role_confidences": ";".join(
                    f"{confidence}:{count}"
                    for confidence, count in sorted(
                        Counter(candidate.role_confidence for candidate in group).items()
                    )
                ),
                "recommended_target_slices": ";".join(
                    f"{target_slice}:{count}"
                    for target_slice, count in sorted(
                        Counter(candidate.recommended_target_slice for candidate in group).items()
                    )
                ),
                "rooms": ";".join(rooms),
                "top_calls": representative.top_calls,
                "call_domains": representative.call_domains,
                "first_command": trace_command(representative),
            }
        )
    return rows


def emit_family_csv(candidates: list[OverlayCandidate]) -> None:
    writer = csv.DictWriter(sys.stdout, fieldnames=FAMILY_FIELDS)
    writer.writeheader()
    writer.writerows(family_rows(candidates))


def emit_family_markdown(candidates: list[OverlayCandidate]) -> None:
    rows = family_rows(candidates)
    print("# Overlay Family Plan")
    print()
    print(
        "Generated from `tools/scripts/catalog_pe_overlays.py --format family-md`. "
        "Repeated shapes should be traced and decompiled through one representative "
        "before applying names or structure guesses to sibling overlays."
    )
    print()
    print(f"- Repeated families: {len(rows)}")
    print("- CSV source: `docs/assets/overlay-family-plan.csv`")
    print("- Family match worklist: `docs/assets/overlay-family-match-worklist.md`")
    print("- Match plan: `docs/assets/overlay-match-plan.md`")
    print()
    print(
        "| Shape | Count | Representative | Members | Slices | Statuses | "
        "Rooms | First command |"
    )
    print("|---|---:|---|---|---|---|---|---|")
    for row in rows:
        members = str(row["member_overlays"]).replace(";", ", ")
        rooms = str(row["rooms"]).replace(";", ", ")
        print(
            f"| `{row['shape']}` | {row['count']} | `{row['representative']}` | "
            f"{members} | {row['recommended_target_slices']} | "
            f"{row['match_statuses']} | {rooms} | `{row['first_command']}` |"
        )


def family_match_worklist_rows(
    pe_img: Path,
    candidates: list[OverlayCandidate],
    config_dir: Path,
) -> list[dict[str, str | int]]:
    families: dict[str, list[OverlayCandidate]] = {}
    for candidate in candidates:
        families.setdefault(candidate.shape, []).append(candidate)

    worklist_by_overlay = {
        str(row["overlay_id"]): row
        for row in match_worklist_rows(pe_img, candidates, config_dir)
    }
    boundary_by_overlay = {
        str(row["overlay_id"]): row
        for row in function_boundary_audit_rows(pe_img, candidates, config_dir)
    }
    target_by_overlay = {
        str(row["overlay_id"]): row
        for row in match_target_rows(candidates, config_dir)
    }

    state_order = {
        "byte-checkable": 0,
        "owner-known-needs-vram": 1,
        "runtime-trace-needed": 2,
        "confirm-execution": 3,
    }
    risk_order = {"low": 0, "medium": 1, "high": 2}
    rows: list[dict[str, str | int]] = []
    for group in families.values():
        if len(group) < 2:
            continue
        configured_members = [
            candidate
            for candidate in group
            if str(worklist_by_overlay[candidate.overlay_id]["target_state"]) == "byte-checkable"
        ]
        dominant_slice = Counter(
            str(target_by_overlay[candidate.overlay_id]["target_slice"]) for candidate in group
        ).most_common(1)[0][0]
        representative_pool = configured_members or [
            candidate
            for candidate in group
            if str(target_by_overlay[candidate.overlay_id]["target_slice"]) == dominant_slice
        ]
        representative = min(
            representative_pool,
            key=lambda candidate: (
                state_order.get(str(worklist_by_overlay[candidate.overlay_id]["target_state"]), 99),
                risk_order.get(str(worklist_by_overlay[candidate.overlay_id]["boundary_risk"]), 99),
                int(worklist_by_overlay[candidate.overlay_id]["first_function_size"] or 1 << 30),
                tuple(-part for part in priority_score(candidate)),
                candidate.overlay_id,
            ),
        )
        rep_worklist = worklist_by_overlay[representative.overlay_id]
        target_rows = [target_by_overlay[candidate.overlay_id] for candidate in group]
        boundary_rows = [boundary_by_overlay[candidate.overlay_id] for candidate in group]
        rooms = sorted({room_label(candidate) for candidate in group if room_label(candidate)})
        family_function_hints = sum(int(row["function_hint_count"]) for row in boundary_rows)
        family_target_size = sum(int(row["target_size"]) for row in target_rows)
        target_state = str(rep_worklist["target_state"])
        boundary_risk = str(rep_worklist["boundary_risk"])
        if target_state == "byte-checkable":
            next_action = "decompile representative first, then transfer names and structure to siblings"
        elif target_state == "confirm-execution":
            next_action = "confirm execution before using this family as a decompilation seed"
        elif boundary_risk == "high":
            next_action = "trace representative, verify hinted split, then review boundaries before C"
        else:
            next_action = "trace representative, generate hinted config, then start from first_function_symbol"
        rows.append(
            {
                "rank": 0,
                "representative": representative.overlay_id,
                "member_count": len(group),
                "reuse_siblings": len(group) - 1,
                "target_state": target_state,
                "boundary_risk": boundary_risk,
                "target_slice": rep_worklist["target_slice"],
                "first_function_symbol": rep_worklist["first_function_symbol"],
                "first_function_offset": rep_worklist["first_function_offset"],
                "first_function_size": rep_worklist["first_function_size"],
                "representative_function_hints": rep_worklist["function_hint_count"],
                "family_function_hints": family_function_hints,
                "family_target_size_bytes": family_target_size,
                "match_statuses": ";".join(
                    f"{status}:{count}"
                    for status, count in sorted(
                        Counter(candidate.match_status for candidate in group).items()
                    )
                ),
                "boundary_risks": ";".join(
                    f"{risk}:{count}"
                    for risk, count in sorted(
                        Counter(str(row["risk_level"]) for row in boundary_rows).items()
                    )
                ),
                "target_slices": ";".join(
                    f"{target_slice}:{count}"
                    for target_slice, count in sorted(
                        Counter(str(row["target_slice"]) for row in target_rows).items()
                    )
                ),
                "role_hypotheses": ";".join(
                    f"{role}:{count}"
                    for role, count in sorted(
                        Counter(candidate.role_hypothesis for candidate in group).items()
                    )
                ),
                "rooms": ";".join(rooms),
                "member_overlays": ";".join(candidate.overlay_id for candidate in group),
                "trace_or_check_command": rep_worklist["trace_or_check_command"],
                "next_action": next_action,
            }
        )

    rows.sort(
        key=lambda row: (
            state_order.get(str(row["target_state"]), 99),
            -int(row["member_count"]),
            risk_order.get(str(row["boundary_risk"]), 99),
            int(row["first_function_size"] or 1 << 30),
            str(row["representative"]),
        )
    )
    for rank, row in enumerate(rows, start=1):
        row["rank"] = rank
    return rows


def emit_family_match_worklist_csv(
    pe_img: Path,
    candidates: list[OverlayCandidate],
    config_dir: Path,
) -> None:
    writer = csv.DictWriter(sys.stdout, fieldnames=FAMILY_MATCH_WORKLIST_FIELDS)
    writer.writeheader()
    writer.writerows(family_match_worklist_rows(pe_img, candidates, config_dir))


def emit_family_match_worklist_markdown(
    pe_img: Path,
    candidates: list[OverlayCandidate],
    config_dir: Path,
) -> None:
    rows = family_match_worklist_rows(pe_img, candidates, config_dir)
    state_counts = Counter(str(row["target_state"]) for row in rows)
    print("# Overlay Family Match Worklist")
    print()
    print(
        "Generated from repeated-shape families, match worklist rows, static function "
        "queue, and boundary audit. This is the family-level queue for choosing one "
        "representative whose C work should transfer to sibling overlays."
    )
    print()
    print(f"- Repeated families: {len(rows)}")
    print(f"- Family members covered: {sum(int(row['member_count']) for row in rows)}")
    print(f"- Reuse siblings: {sum(int(row['reuse_siblings']) for row in rows)}")
    print("- CSV source: `docs/assets/overlay-family-match-worklist.csv`")
    print("- Family plan: `docs/assets/overlay-family-plan.md`")
    print("- Match worklist: `docs/assets/overlay-match-worklist.md`")
    print("- Target states:")
    for state, count in sorted(state_counts.items()):
        print(f"  - `{state}`: {count}")
    print()
    print(
        "| Rank | Representative | Members | Reuse | State | Risk | Slice | "
        "First function | Size | Family funcs | Command | Next action |"
    )
    print("|---:|---|---:|---:|---|---|---|---|---:|---:|---|---|")
    for row in rows:
        print(
            f"| {row['rank']} | `{row['representative']}` | {row['member_count']} | "
            f"{row['reuse_siblings']} | `{row['target_state']}` | "
            f"`{row['boundary_risk']}` | `{row['target_slice']}` | "
            f"`{row['first_function_symbol']}` | {row['first_function_size']} | "
            f"{row['family_function_hints']} | `{row['trace_or_check_command']}` | "
            f"{row['next_action']} |"
        )


ROOM_MAP_FIELDS = [
    "room",
    "room_id",
    "overlay_id",
    "configured",
    "match_status",
    "role_hypothesis",
    "role_confidence",
    "recommended_target_slice",
    "room_base_sector",
    "room_section1_sectors",
    "room_section2_sectors",
    "room_section3_sector",
    "room_section3_sectors",
    "room_section3_file_offset",
    "room_section3_size_kib",
    "scanner_sector_start",
    "scanner_delta",
    "scanner_sectors",
    "target_sector",
    "target_sector_count",
    "loader_site",
    "source_expr",
    "caller_pc_hint",
    "dst_probe",
    "entry_probe",
    "default_vram",
    "configured_vram",
    "shape",
    "duplicate_group",
    "trace_or_check_command",
    "top_calls",
    "call_domains",
]


def room_map_rows(
    candidates: list[OverlayCandidate], config_dir: Path
) -> list[dict[str, str | int | None]]:
    configured = read_overlay_configs(config_dir, candidates)
    room_candidates = sorted(
        (candidate for candidate in candidates if candidate.room_id is not None),
        key=lambda candidate: (
            candidate.room_id if candidate.room_id is not None else 9999,
            candidate.room_delta if candidate.room_delta is not None else 9999,
            candidate.overlay_id,
        ),
    )
    rows: list[dict[str, str | int | None]] = []
    for candidate in room_candidates:
        target_sector, target_sector_count, _target_size, _target_sha, _target_sha_full = (
            target_slice_info(candidate, candidate.recommended_target_slice)
        )
        config = configured.get(candidate.overlay_id)
        loader_fields = loader_trace_fields(candidate)
        rows.append(
            {
                "room": room_label(candidate),
                "room_id": candidate.room_id,
                "overlay_id": candidate.overlay_id,
                "configured": "yes" if config is not None else "no",
                "match_status": candidate.match_status,
                "role_hypothesis": candidate.role_hypothesis,
                "role_confidence": candidate.role_confidence,
                "recommended_target_slice": candidate.recommended_target_slice,
                "room_base_sector": candidate.room_base_sector or "",
                "room_section1_sectors": candidate.room_size1 or "",
                "room_section2_sectors": candidate.room_size2 or "",
                "room_section3_sector": candidate.room_section3_sector,
                "room_section3_sectors": candidate.room_size3,
                "room_section3_file_offset": (
                    f"0x{candidate.room_section3_sector * SECTOR_SIZE:08X}"
                    if candidate.room_section3_sector is not None
                    else ""
                ),
                "room_section3_size_kib": (
                    candidate.room_section3_size_bytes // 1024
                    if candidate.room_section3_size_bytes is not None
                    else ""
                ),
                "scanner_sector_start": candidate.sector_start,
                "scanner_delta": candidate.room_delta,
                "scanner_sectors": candidate.sector_count,
                "target_sector": target_sector,
                "target_sector_count": target_sector_count,
                "loader_site": loader_fields["loader_site"],
                "source_expr": loader_fields["source_expr"],
                "caller_pc_hint": loader_fields["caller_pc_hint"],
                "dst_probe": loader_fields["dst_probe"],
                "entry_probe": loader_fields["entry_probe"],
                "default_vram": (
                    f"0x{candidate.default_vram:08X}"
                    if candidate.default_vram is not None
                    else ""
                ),
                "configured_vram": f"0x{config.vram:08X}" if config is not None else "",
                "shape": candidate.shape,
                "duplicate_group": candidate.duplicate_group,
                "trace_or_check_command": (
                    f"make overlay-check OVERLAY={candidate.overlay_id}"
                    if config is not None
                    else trace_command(candidate)
                ),
                "top_calls": candidate.top_calls,
                "call_domains": candidate.call_domains,
            }
        )
    return rows


def emit_room_map_csv(candidates: list[OverlayCandidate], config_dir: Path) -> None:
    writer = csv.DictWriter(sys.stdout, fieldnames=ROOM_MAP_FIELDS)
    writer.writeheader()
    writer.writerows(room_map_rows(candidates, config_dir))


def emit_room_map_markdown(candidates: list[OverlayCandidate], config_dir: Path) -> None:
    rows = room_map_rows(candidates, config_dir)
    print("# Overlay Room Map")
    print()
    print(
        "Generated from `tools/scripts/catalog_pe_overlays.py --format room-map-md`. "
        "This is the room-owned overlay trace map. These targets usually need only "
        "runtime VRAM/entry evidence before an FF8-style overlay config can be "
        "created."
    )
    print()
    print(f"- Room-owned rows: {len(rows)}")
    print("- CSV source: `docs/assets/overlay-room-map.csv`")
    print("- Full trace backlog: `docs/assets/overlay-trace-sheet-all.csv`")
    print()
    print(
        "The room table stores `base_sector` plus section sizes. `section3_sector` "
        "is `base_sector + section1 + section2`; the `Scene_LoadRoom` read at "
        "`0x8006B6EC` loads exactly that section into `*(u32 *)(g_GameState + 0x18C)`."
    )
    print()
    print(
        "| Room | Overlay | Configured | Status | Target slice | Base sector | "
        "Section sizes | Section3 sector | File offset | Loader site | Caller hint | "
        "Destination probe | Scanner delta | Shape | Command |"
    )
    print("|---|---|---|---|---|---:|---|---:|---:|---|---|---|---:|---|---|")
    for row in rows:
        section_sizes = (
            f"{row['room_section1_sectors']}/"
            f"{row['room_section2_sectors']}/"
            f"{row['room_section3_sectors']}"
        )
        print(
            f"| `{row['room']}` | `{row['overlay_id']}` | {row['configured']} | "
            f"`{row['match_status']}` | `{row['recommended_target_slice']}` | "
            f"{row['room_base_sector']} | `{section_sizes}` | "
            f"{row['room_section3_sector']} | `{row['room_section3_file_offset']}` | "
            f"{row['loader_site']} | `{row['caller_pc_hint']}` | `{row['dst_probe']}` | "
            f"{row['scanner_delta']} | `{row['shape']}` | "
            f"`{row['trace_or_check_command']}` |"
        )


def non_room_trace_category(candidate: OverlayCandidate) -> str:
    if candidate.overlay_id == "ovl_002":
        return "title-intro-anchor"
    if "tiny" in candidate.tags:
        return "tiny-review"
    if "call-heavy" in candidate.tags:
        return "call-heavy-subsystem"
    if candidate.role_confidence == "medium-low":
        return "render-effect-subsystem"
    return "late-non-room-block"


NON_ROOM_PLAN_FIELDS = [
    "priority",
    "overlay_id",
    "configured",
    "match_status",
    "trace_category",
    "role_hypothesis",
    "role_confidence",
    "recommended_target_slice",
    "target_sector",
    "target_sector_count",
    "target_size_kib",
    "loader_site",
    "source_expr",
    "caller_pc_hint",
    "dst_probe",
    "entry_probe",
    "scanner_sector_start",
    "scanner_sectors",
    "shape",
    "duplicate_group",
    "evidence_needed",
    "trace_or_check_command",
    "top_calls",
    "call_domains",
]


def non_room_plan_rows(
    candidates: list[OverlayCandidate], config_dir: Path
) -> list[dict[str, str | int]]:
    configured = read_overlay_configs(config_dir, candidates)
    non_room_candidates = sorted(
        (candidate for candidate in candidates if candidate.room_id is None),
        key=priority_score,
        reverse=True,
    )
    rows: list[dict[str, str | int]] = []
    for priority, candidate in enumerate(non_room_candidates, 1):
        target_sector, target_sector_count, target_size, _target_sha, _target_sha_full = (
            target_slice_info(candidate, candidate.recommended_target_slice)
        )
        config = configured.get(candidate.overlay_id)
        loader_fields = loader_trace_fields(candidate)
        rows.append(
            {
                "priority": priority,
                "overlay_id": candidate.overlay_id,
                "configured": "yes" if config is not None else "no",
                "match_status": candidate.match_status,
                "trace_category": non_room_trace_category(candidate),
                "role_hypothesis": candidate.role_hypothesis,
                "role_confidence": candidate.role_confidence,
                "recommended_target_slice": candidate.recommended_target_slice,
                "target_sector": target_sector,
                "target_sector_count": target_sector_count,
                "target_size_kib": target_size // 1024,
                "loader_site": loader_fields["loader_site"],
                "source_expr": loader_fields["source_expr"],
                "caller_pc_hint": loader_fields["caller_pc_hint"],
                "dst_probe": loader_fields["dst_probe"],
                "entry_probe": loader_fields["entry_probe"],
                "scanner_sector_start": candidate.sector_start,
                "scanner_sectors": candidate.sector_count,
                "shape": candidate.shape,
                "duplicate_group": candidate.duplicate_group,
                "evidence_needed": evidence_needed(candidate, config is not None),
                "trace_or_check_command": (
                    f"make overlay-check OVERLAY={candidate.overlay_id}"
                    if config is not None
                    else trace_command(candidate)
                ),
                "top_calls": candidate.top_calls,
                "call_domains": candidate.call_domains,
            }
        )
    return rows


def emit_non_room_plan_csv(candidates: list[OverlayCandidate], config_dir: Path) -> None:
    writer = csv.DictWriter(sys.stdout, fieldnames=NON_ROOM_PLAN_FIELDS)
    writer.writeheader()
    writer.writerows(non_room_plan_rows(candidates, config_dir))


def emit_non_room_plan_markdown(candidates: list[OverlayCandidate], config_dir: Path) -> None:
    rows = non_room_plan_rows(candidates, config_dir)
    print("# Overlay Non-Room Trace Plan")
    print()
    print(
        "Generated from `tools/scripts/catalog_pe_overlays.py --format non-room-plan-md`. "
        "These scanner-only candidates are not owned by the room table and need "
        "runtime caller/owner evidence before an overlay config can be committed."
    )
    print()
    print(f"- Non-room rows: {len(rows)}")
    print("- Markdown output: `docs/assets/overlay-non-room-plan.md`")
    print("- CSV source: `docs/assets/overlay-non-room-plan.csv`")
    print("- Full trace backlog: `docs/assets/overlay-trace-sheet-all.csv`")
    print()
    print(
        "| Priority | Overlay | Category | Status | Target sector | Sectors | "
        "Loader site | Caller hint | Destination probe | Shape | Evidence needed | Command |"
    )
    print("|---:|---|---|---|---:|---:|---|---|---|---|---|---|")
    for row in rows:
        print(
            f"| {row['priority']} | `{row['overlay_id']}` | {row['trace_category']} | "
            f"`{row['match_status']}` | {row['target_sector']} | "
            f"{row['target_sector_count']} | {row['loader_site']} | "
            f"`{row['caller_pc_hint']}` | `{row['dst_probe']}` | `{row['shape']}` | "
            f"{row['evidence_needed']} | `{row['trace_or_check_command']}` |"
        )


def batch_reason(candidate: OverlayCandidate) -> str:
    reasons: list[str] = []
    if candidate.default_vram is not None:
        reasons.append("known-good configured target")
    if candidate.overlay_id == "ovl_002":
        reasons.append("title/intro anchor")
    if "call-heavy" in candidate.tags:
        reasons.append("call-heavy")
    if candidate.room_id is not None:
        reasons.append(f"room {room_label(candidate)}")
    if candidate.duplicate_count > 1:
        reasons.append(f"family/duplicate x{candidate.duplicate_count}")
    if "tiny" in candidate.tags:
        reasons.append("tiny/review")
    if not reasons:
        reasons.append(candidate.role_confidence)
    return ", ".join(reasons)


def trace_batches(candidates: list[OverlayCandidate]) -> list[tuple[str, str, list[OverlayCandidate]]]:
    by_id = {candidate.overlay_id: candidate for candidate in candidates}

    def selected(ids: list[str]) -> list[OverlayCandidate]:
        return [by_id[overlay_id] for overlay_id in ids if overlay_id in by_id]

    family_representatives: list[OverlayCandidate] = []
    families: dict[str, list[OverlayCandidate]] = {}
    for candidate in candidates:
        families.setdefault(candidate.shape, []).append(candidate)
    repeated = sorted(
        (group for group in families.values() if len(group) >= 2),
        key=lambda group: (len(group), max(priority_score(candidate) for candidate in group)),
        reverse=True,
    )
    for group in repeated:
        representative = max(group, key=priority_score)
        if representative.overlay_id not in {"ovl_009"}:
            family_representatives.append(representative)

    non_room_priority = [
        candidate
        for candidate in sorted(
            (candidate for candidate in candidates if candidate.room_id is None),
            key=priority_score,
            reverse=True,
        )
        if "tiny" not in candidate.tags
    ]
    tiny_review = [
        candidate
        for candidate in sorted(candidates, key=priority_score, reverse=True)
        if candidate.room_id is None and "tiny" in candidate.tags
    ]

    return [
        (
            "Batch 1 - loader and title/intro anchors",
            "Calibrate trace capture against one known-good overlay, then catch early non-room loads.",
            selected(["ovl_009", "ovl_002", "ovl_003", "ovl_004", "ovl_007"]),
        ),
        (
            "Batch 2 - high-value room overlays",
            "Trace room-owned call-heavy targets and the largest repeated-family representatives.",
            selected(["ovl_128", "ovl_024"])
            + [candidate for candidate in family_representatives[:6] if candidate.overlay_id not in {"ovl_024"}],
        ),
        (
            "Batch 3 - late non-room render/effect targets",
            "Trace the highest-value non-room candidates that are not tied to the room table.",
            [candidate for candidate in non_room_priority if candidate.overlay_id not in {"ovl_002", "ovl_003", "ovl_004", "ovl_007"}][:8],
        ),
        (
            "Batch 4 - tiny/review validation",
            "Decide whether tiny non-room scanner hits are real overlays before creating configs.",
            tiny_review,
        ),
    ]


def emit_trace_batches_markdown(candidates: list[OverlayCandidate], config_dir: Path) -> None:
    configured = read_overlay_configs(config_dir, candidates)

    print("# Overlay Runtime Trace Batches")
    print()
    print(
        "Generated from `tools/scripts/catalog_pe_overlays.py --format trace-batches-md`. "
        "This is a session-oriented plan for collecting the runtime evidence needed "
        "before new FF8-style overlay configs can be committed."
    )
    print()
    print("## Evidence To Record")
    print()
    print("- `caller_pc`: return address or owner that requested the CD read.")
    print("- `pe_sector`: PE.IMG-relative sector, or absolute `lba` plus `PE_BASE_LBA`.")
    print("- `dst`: runtime destination address passed to the read.")
    print("- `size`: sector count read.")
    print("- `exec_pc`: first PC observed executing inside the loaded or copied overlay.")
    print("- `canonical_base`: copied/executed base, if execution happens outside the read destination.")
    print()
    print("Use `make overlay-trace-template OVERLAY_TRACE=trace.csv` and then match with:")
    print()
    print("```")
    print("make overlay-trace-match OVERLAY_TRACE=trace.csv ALL_MATCHES=1")
    print("make overlay-init-from-trace OVERLAY_TRACE=trace.csv DRY_RUN=1")
    print("```")
    print()

    for title, description, batch_candidates in trace_batches(candidates):
        print(f"## {title}")
        print()
        print(description)
        print()
        print("| Overlay | Status | Slice | Sector | Sectors | Role | Why | Command after trace |")
        print("|---|---|---|---:|---:|---|---|---|")
        for candidate in batch_candidates:
            target_sector, target_sector_count, _target_size, _sha, _sha_full = target_slice_info(
                candidate, candidate.recommended_target_slice
            )
            command = (
                f"make overlay-check OVERLAY={candidate.overlay_id}"
                if candidate.overlay_id in configured
                else trace_command(candidate)
            )
            print(
                f"| `{candidate.overlay_id}` | `{candidate.match_status}` | "
                f"`{candidate.recommended_target_slice}` | {target_sector} | "
                f"{target_sector_count} | {candidate.role_hypothesis} | "
                f"{batch_reason(candidate)} | `{command}` |"
            )
        print()


TRACE_SHEET_FIELDS = [
    "batch",
    "overlay_id",
    "configured",
    "match_status",
    "role_hypothesis",
    "role_confidence",
    "primary_domain",
    "secondary_domains",
    "target_slice",
    "expected_pe_sector",
    "expected_size",
    "target_sha1_12",
    "loader_site",
    "source_expr",
    "caller_pc_hint",
    "dst_probe",
    "entry_probe",
    "caller_pc",
    "lba",
    "dst",
    "exec_pc",
    "canonical_base",
    "reason",
    "post_trace_command",
    "notes",
]


def emit_trace_sheet_csv(candidates: list[OverlayCandidate], config_dir: Path) -> None:
    configured = read_overlay_configs(config_dir, candidates)
    writer = csv.DictWriter(sys.stdout, fieldnames=TRACE_SHEET_FIELDS)
    writer.writeheader()
    for batch_title, _description, batch_candidates in trace_batches(candidates):
        for candidate in batch_candidates:
            writer.writerow(
                trace_sheet_row(
                    candidate,
                    configured,
                    batch_title,
                    batch_reason(candidate),
                )
            )


def trace_sheet_row(
    candidate: OverlayCandidate,
    configured: dict[str, OverlayConfigStatus],
    batch: str,
    reason: str,
) -> dict[str, str | int]:
    target_sector, target_sector_count, _target_size, target_sha, _sha_full = target_slice_info(
        candidate, candidate.recommended_target_slice
    )
    command = (
        f"make overlay-check OVERLAY={candidate.overlay_id}"
        if candidate.overlay_id in configured
        else trace_command(candidate)
    )
    domains = parse_domain_counts(candidate.call_domains)
    if domains:
        primary_domain, _primary_calls = domains.most_common(1)[0]
        secondary_domains = ";".join(
            f"{domain}:{count}" for domain, count in domains.most_common()[1:5]
        )
    else:
        primary_domain = ""
        secondary_domains = ""
    loader_fields = loader_trace_fields(candidate)
    return {
        "batch": batch,
        "overlay_id": candidate.overlay_id,
        "configured": "yes" if candidate.overlay_id in configured else "no",
        "match_status": candidate.match_status,
        "role_hypothesis": candidate.role_hypothesis,
        "role_confidence": candidate.role_confidence,
        "primary_domain": primary_domain,
        "secondary_domains": secondary_domains,
        "target_slice": candidate.recommended_target_slice,
        "expected_pe_sector": target_sector,
        "expected_size": target_sector_count,
        "target_sha1_12": target_sha,
        "loader_site": loader_fields["loader_site"],
        "source_expr": loader_fields["source_expr"],
        "caller_pc_hint": loader_fields["caller_pc_hint"],
        "dst_probe": loader_fields["dst_probe"],
        "entry_probe": loader_fields["entry_probe"],
        "caller_pc": "",
        "lba": "",
        "dst": f"0x{candidate.load_vram:08X}" if candidate.load_vram is not None else "",
        "exec_pc": f"0x{candidate.entry_vram:08X}" if candidate.entry_vram is not None else "",
        "canonical_base": (
            f"0x{candidate.canonical_vram:08X}"
            if candidate.canonical_vram is not None
            else ""
        ),
        "reason": reason,
        "post_trace_command": command,
        "notes": "",
    }


def full_trace_batch(candidate: OverlayCandidate, configured: dict[str, OverlayConfigStatus]) -> str:
    if candidate.overlay_id in configured:
        return "configured"
    if "tiny" in candidate.tags and candidate.room_id is None:
        return "all-tiny-review"
    if candidate.room_id is None:
        return "all-non-room"
    return "all-room"


def emit_trace_sheet_all_csv(candidates: list[OverlayCandidate], config_dir: Path) -> None:
    configured = read_overlay_configs(config_dir, candidates)
    writer = csv.DictWriter(sys.stdout, fieldnames=TRACE_SHEET_FIELDS)
    writer.writeheader()
    for candidate in sorted(candidates, key=lambda item: item.overlay_id):
        writer.writerow(
            trace_sheet_row(
                candidate,
                configured,
                full_trace_batch(candidate, configured),
                batch_reason(candidate),
            )
        )


TRACE_ACTION_FIELDS = [
    "priority",
    "overlay_id",
    "batch",
    "action_status",
    "target_slice",
    "expected_pe_sector",
    "expected_size",
    "breakpoint",
    "caller_pc_hint",
    "source_expr",
    "dst_probe",
    "entry_probe",
    "target_sha1_12",
    "trace_sheet",
    "post_trace_command",
    "reason",
]


def trace_action_status(candidate: OverlayCandidate, configured: bool) -> str:
    if configured:
        return "check-configured"
    if "tiny" in candidate.tags and candidate.room_id is None:
        return "confirm-execution"
    if candidate.room_id is not None:
        return "trace-room-vram-entry"
    return "trace-read-owner-vram-entry"


def trace_action_rows(
    candidates: list[OverlayCandidate], config_dir: Path
) -> list[dict[str, str | int]]:
    configured = read_overlay_configs(config_dir, candidates)
    rows: list[dict[str, str | int]] = []
    ordered = sorted(candidates, key=priority_score, reverse=True)
    for priority, candidate in enumerate(ordered, 1):
        trace_row = trace_sheet_row(
            candidate,
            configured,
            full_trace_batch(candidate, configured),
            batch_reason(candidate),
        )
        rows.append(
            {
                "priority": priority,
                "overlay_id": candidate.overlay_id,
                "batch": trace_row["batch"],
                "action_status": trace_action_status(
                    candidate, candidate.overlay_id in configured
                ),
                "target_slice": trace_row["target_slice"],
                "expected_pe_sector": trace_row["expected_pe_sector"],
                "expected_size": trace_row["expected_size"],
                "breakpoint": trace_breakpoint(candidate),
                "caller_pc_hint": trace_row["caller_pc_hint"],
                "source_expr": trace_row["source_expr"],
                "dst_probe": trace_row["dst_probe"],
                "entry_probe": trace_row["entry_probe"],
                "target_sha1_12": trace_row["target_sha1_12"],
                "trace_sheet": "docs/assets/overlay-trace-sheet-all.csv",
                "post_trace_command": trace_row["post_trace_command"],
                "reason": trace_row["reason"],
            }
        )
    return rows


def emit_trace_actions_csv(candidates: list[OverlayCandidate], config_dir: Path) -> None:
    writer = csv.DictWriter(sys.stdout, fieldnames=TRACE_ACTION_FIELDS)
    writer.writeheader()
    writer.writerows(trace_action_rows(candidates, config_dir))


def emit_trace_actions_markdown(candidates: list[OverlayCandidate], config_dir: Path) -> None:
    rows = trace_action_rows(candidates, config_dir)
    status_counts = Counter(str(row["action_status"]) for row in rows)
    print("# Overlay Trace Action Plan")
    print()
    print(
        "Generated from `tools/scripts/catalog_pe_overlays.py --format trace-actions-md`. "
        "This is the concrete runtime action list for turning every PE.IMG code-overlay "
        "candidate into an FF8-style split/build/check target."
    )
    print()
    print("## Summary")
    print()
    print(f"- Action rows: {len(rows)}")
    print("- CSV source: `docs/assets/overlay-trace-actions.csv`")
    print("- Trace backlog: `docs/assets/overlay-trace-sheet-all.csv`")
    print("- Capture guide: `docs/assets/overlay-trace-capture.md`")
    print()
    print("| Action status | Count |")
    print("|---|---:|")
    for status, count in sorted(status_counts.items()):
        print(f"| `{status}` | {count} |")
    print()
    print("## Workflow")
    print()
    print("1. Break at the listed `breakpoint` and keep only reads matching `expected_pe_sector`.")
    print("2. Fill `caller_pc`, `dst`, and `exec_pc` in `overlay-trace-sheet-all.csv`.")
    print("3. Run `make overlay-trace-sheet-all-match ALL_MATCHES=1`.")
    print("4. Run `make overlay-init-from-full-sheet DRY_RUN=1` before writing configs.")
    print()
    print("## Actions")
    print()
    print(
        "| Priority | Overlay | Status | Batch | Sector | Size | Breakpoint | "
        "Caller hint | Destination probe | Entry probe | Command |"
    )
    print("|---:|---|---|---|---:|---:|---|---|---|---|---|")
    for row in rows:
        print(
            f"| {row['priority']} | `{row['overlay_id']}` | `{row['action_status']}` | "
            f"`{row['batch']}` | {row['expected_pe_sector']} | {row['expected_size']} | "
            f"{row['breakpoint']} | `{row['caller_pc_hint']}` | `{row['dst_probe']}` | "
            f"`{row['entry_probe']}` | `{row['post_trace_command']}` |"
        )


def emit_status_markdown(candidates: list[OverlayCandidate], config_dir: Path) -> None:
    configured = read_overlay_configs(config_dir, candidates)
    statuses = Counter(
        "match-ready" if candidate.overlay_id in configured else candidate.match_status
        for candidate in candidates
    )
    roles = Counter(candidate.role_hypothesis for candidate in candidates)
    target_slices = Counter(candidate.recommended_target_slice for candidate in candidates)
    room_owned = [candidate for candidate in candidates if candidate.room_id is not None]
    non_room = [candidate for candidate in candidates if candidate.room_id is None]

    print("# Overlay Matching Dashboard")
    print()
    print(
        "Generated from `tools/scripts/catalog_pe_overlays.py --format status-md`. "
        "This is the high-level working view for turning PE.IMG overlay candidates "
        "into FF8-style split/build/check targets. It records metadata only; original "
        "overlay bytes are generated locally from the user's disc image."
    )
    print()

    print("## Current State")
    print()
    print(f"- Candidate code overlays: {len(candidates)}")
    print(f"- Configured match targets: {len(configured)}")
    print(f"- Room-owned candidates: {len(room_owned)}")
    print(f"- Non-room candidates: {len(non_room)}")
    print("- Match statuses:")
    for status, count in sorted(statuses.items()):
        print(f"  - `{status}`: {count}")
    print("- Recommended target slices:")
    for target_slice, count in sorted(target_slices.items()):
        print(f"  - `{target_slice}`: {count}")
    print("- Room trace map: `docs/assets/overlay-room-map.md` / `docs/assets/overlay-room-map.csv`")
    print("- Non-room trace plan: `docs/assets/overlay-non-room-plan.md` / `docs/assets/overlay-non-room-plan.csv`")
    print("- Entry hints: `docs/assets/overlay-entry-hints.md` / `docs/assets/overlay-entry-hints.csv`")
    print("- Static function hints: `docs/assets/overlay-static-function-hints.md` / `docs/assets/overlay-static-function-hints.csv`")
    print("- Static function queue: `docs/assets/overlay-static-function-queue.md` / `docs/assets/overlay-static-function-queue.csv`")
    print("- Function boundary audit: `docs/assets/overlay-function-boundary-audit.md` / `docs/assets/overlay-function-boundary-audit.csv`")
    print("- Undefined tail audit: `docs/assets/overlay-undefined-tail-audit.md` / `docs/assets/overlay-undefined-tail-audit.csv`")
    print("- Match blocker matrix: `docs/assets/overlay-match-blockers.md` / `docs/assets/overlay-match-blockers.csv`")
    print("- Splat subsegment hints: `docs/assets/overlay-splat-subsegment-hints.md` / `docs/assets/overlay-splat-subsegment-hints.csv`")
    print("- Hinted config audit: `docs/assets/overlay-hinted-config-audit.md` / `docs/assets/overlay-hinted-config-audit.csv`")
    print("- Debugger plan: `docs/assets/overlay-debugger-plan.md` / `docs/assets/overlay-debugger-plan.csv`")
    print("- Evidence matrix: `docs/assets/overlay-evidence-matrix.md` / `docs/assets/overlay-evidence-matrix.csv`")
    print("- Scanner audit: `docs/assets/overlay-scanner-audit.md` / `docs/assets/overlay-scanner-audit.csv`")
    print("- External call map: `docs/assets/overlay-call-map.md` / `docs/assets/overlay-call-map.csv`")
    print("- Domain plan: `docs/assets/overlay-domain-plan.md` / `docs/assets/overlay-domain-plan.csv`")
    print("- Trace batches: `docs/assets/overlay-trace-batches.md`")
    print("- Trace sheet: `docs/assets/overlay-trace-sheet.csv`")
    print("- Full trace sheet: `docs/assets/overlay-trace-sheet-all.csv`")
    print("- Trace actions: `docs/assets/overlay-trace-actions.md` / `docs/assets/overlay-trace-actions.csv`")
    print("- Trace session plan: `docs/assets/overlay-trace-session-plan.md`")
    print("- Match worklist: `docs/assets/overlay-match-worklist.md` / `docs/assets/overlay-match-worklist.csv`")
    print("- Match worklist trace template: `docs/assets/overlay-match-worklist-trace-template.csv`")
    print("- Family match worklist: `docs/assets/overlay-family-match-worklist.md` / `docs/assets/overlay-family-match-worklist.csv`")
    print("- Family trace template: `docs/assets/overlay-family-trace-template.csv`")
    print()

    print("## Work Categories")
    print()
    print("| Category | Count | What it means | Next action |")
    print("|---|---:|---|---|")
    print(
        f"| Match-ready | {statuses['match-ready']} | Config exists and local "
        "overlay bytes can be rebuilt byte-for-byte. | Replace generated asm with C, "
        "then run `make overlay-check-all`. |"
    )
    print(
        f"| Owner known, VRAM missing | {statuses['owner-known-needs-vram']} | "
        "Room section-3 owner and source slice are known, but the runtime load/entry "
        "address is not. | Trace room load destination and execution PC, then use "
        "`OVERLAY_SLICE=room-section3`. |"
    )
    print(
        f"| Runtime trace needed | {statuses['needs-runtime-trace']} | Candidate is "
        "not confidently owned by a room table entry. | Trace CD read caller, source "
        "sector, destination, and first executed PC. |"
    )
    print(
        f"| Tiny/review | {statuses['review-tiny-or-false-positive']} | Small "
        "code-like blocks may be real overlays or scanner noise. | Validate with "
        "runtime trace before creating configs. |"
    )
    print()

    print("## Highest-Value Trace Order")
    print()
    print("| Overlay | Status | Slice | Sector | Sectors | Role | Why | Command after trace |")
    print("|---|---|---|---:|---:|---|---|---|")
    trace_targets = [
        candidate
        for candidate in candidates
        if candidate.overlay_id not in configured
        and (
            candidate.overlay_id == "ovl_002"
            or "call-heavy" in candidate.tags
            or candidate.role_confidence in {"medium", "medium-low"}
        )
    ]
    for candidate in sorted(trace_targets, key=priority_score, reverse=True)[:12]:
        sector, sector_count, _size, _sha, _sha_full = target_slice_info(
            candidate, candidate.recommended_target_slice
        )
        why: list[str] = []
        if candidate.overlay_id == "ovl_002":
            why.append("title/intro anchor")
        if "call-heavy" in candidate.tags:
            why.append("call-heavy")
        if candidate.room_id is not None:
            why.append(f"room {room_label(candidate)}")
        if candidate.duplicate_count > 1:
            why.append(f"duplicate group x{candidate.duplicate_count}")
        if not why:
            why.append(candidate.role_confidence)
        print(
            f"| `{candidate.overlay_id}` | `{candidate.match_status}` | "
            f"`{candidate.recommended_target_slice}` | {sector} | {sector_count} | "
            f"{candidate.role_hypothesis} | {', '.join(why)} | "
            f"`{trace_command(candidate)}` |"
        )
    print()

    print("## Representative Family Plan")
    print()
    print(
        "Repeated shapes should be handled by tracing and matching one representative "
        "first, then comparing siblings. The full table is "
        "`docs/assets/overlay-family-plan.md`; the machine-readable table is "
        "`docs/assets/overlay-family-plan.csv`."
    )
    print()
    families: dict[str, list[OverlayCandidate]] = {}
    for candidate in candidates:
        families.setdefault(candidate.shape, []).append(candidate)
    repeated = sorted(
        (group for group in families.values() if len(group) >= 2),
        key=lambda group: (len(group), max(priority_score(candidate) for candidate in group)),
        reverse=True,
    )
    print("| Shape | Count | Representative | Members | Role | First command |")
    print("|---|---:|---|---|---|---|")
    for group in repeated[:10]:
        representative = max(group, key=priority_score)
        members = ", ".join(candidate.overlay_id for candidate in group[:8])
        if len(group) > 8:
            members += f", +{len(group) - 8} more"
        print(
            f"| `{representative.shape}` | {len(group)} | `{representative.overlay_id}` | "
            f"{members} | {representative.role_hypothesis} | "
            f"`{trace_command(representative)}` |"
        )
    print()

    print("## Role Distribution")
    print()
    print("| Role hypothesis | Count |")
    print("|---|---:|")
    for role, count in roles.most_common():
        print(f"| {role} | {count} |")
    print()

    print("## Evidence Gate")
    print()
    print(
        "Do not commit another `configs/USA/overlays/*.yaml` until a trace proves the "
        "PE.IMG sector, sector count or exact containing read, runtime destination, "
        "and first executed PC. If execution happens outside the destination buffer, "
        "record the copied/canonical base before choosing the config VRAM."
    )


def emit_match_readiness_markdown(candidates: list[OverlayCandidate], config_dir: Path) -> None:
    configured = read_overlay_configs(config_dir, candidates)
    statuses = Counter(
        "match-ready" if candidate.overlay_id in configured else candidate.match_status
        for candidate in candidates
    )
    target_slices = Counter(candidate.recommended_target_slice for candidate in candidates)
    room_owned = [candidate for candidate in candidates if candidate.room_id is not None]
    non_room = [candidate for candidate in candidates if candidate.room_id is None]
    by_overlay = {candidate.overlay_id: candidate for candidate in candidates}

    print("# Overlay Match Readiness")
    print()
    print(
        "Generated from `tools/scripts/catalog_pe_overlays.py --format readiness-md`. "
        "This is the release-facing audit that every PE.IMG code-overlay candidate is "
        "cataloged, assigned a target slice, and connected to the FF8-style "
        "trace/split/build/check workflow."
    )
    print()

    print("## Coverage")
    print()
    print(f"- Cataloged code-overlay candidates: {len(candidates)}")
    print(f"- Present in full trace backlog: {len(candidates)}")
    print(f"- Configured match targets: {len(configured)}")
    print(f"- Room-owned candidates: {len(room_owned)}")
    print(f"- Non-room candidates: {len(non_room)}")
    print("- Target slices:")
    for target_slice, count in sorted(target_slices.items()):
        print(f"  - `{target_slice}`: {count}")
    print("- Match statuses:")
    for status, count in sorted(statuses.items()):
        print(f"  - `{status}`: {count}")
    print()

    print("## Matchability Gates")
    print()
    print("| Gate | Count | Required next action | Source of truth |")
    print("|---|---:|---|---|")
    print(
        f"| Configured and byte-checkable | {len(configured)} | Replace asm with C, "
        "then run `make overlay-check-all`. | "
        "`configs/USA/overlays/`, `docs/assets/overlay-decomp-status.csv` |"
    )
    print(
        f"| Owner known, VRAM missing | {statuses['owner-known-needs-vram']} | "
        "Trace room section-3 destination and first executed PC, then run "
        "`make overlay-init-from-full-sheet DRY_RUN=1`. | "
        "`docs/assets/overlay-room-map.md`, `docs/assets/overlay-room-map.csv`, "
        "`docs/assets/overlay-trace-sheet-all.csv` |"
    )
    print(
        f"| Runtime trace needed | {statuses['needs-runtime-trace']} | Trace CD read "
        "caller, PE.IMG sector, destination, size, and execution PC. | "
        "`docs/assets/overlay-non-room-plan.md`, `docs/assets/overlay-non-room-plan.csv`, "
        "`docs/assets/overlay-trace-sheet-all.csv` |"
    )
    print(
        f"| Tiny/review | {statuses['review-tiny-or-false-positive']} | Confirm the "
        "code-like block is executed before creating a config. | "
        "`docs/assets/overlay-trace-sheet-all.csv` |"
    )
    print()

    print("## Proven Smoke Coverage")
    print()
    print(
        "These rows are synthetic trace proofs, not committed match targets. They "
        "show that the trace-to-config path, hinted asm subsegments, split/link, "
        "extra absolute symbols, and SHA check work for each major overlay class."
    )
    print()
    print("| Class | Overlay | Slice | Size | Subsegments | SHA-1 | Command |")
    print("|---|---|---|---:|---:|---|---|")
    smoke_rows = [
        ("title/intro non-room", "ovl_002", "scanner", 48, "make overlay-trace-hinted-smoke"),
        ("room section3", "ovl_024", "room-section3", 40, "make overlay-trace-room-hinted-smoke"),
        ("call-heavy non-room effect", "ovl_189", "scanner", 46, "make overlay-trace-effect-hinted-smoke"),
        ("tiny/review scanner", "ovl_006", "scanner", 2, "make overlay-trace-tiny-hinted-smoke"),
    ]
    for class_name, overlay_id, target_slice, subsegments, command in smoke_rows:
        candidate = by_overlay[overlay_id]
        _sector, _sector_count, target_size, _sha12, target_sha1 = target_slice_info(
            candidate,
            target_slice,
        )
        print(
            f"| {class_name} | `{overlay_id}` | `{target_slice}` | {target_size} | "
            f"{subsegments} | `{target_sha1}` | `{command}` |"
        )
    print()

    print("## Complete Backlog Workflow")
    print()
    print("```")
    print("make overlay-docs")
    print("make overlay-trace-sheet-all-status")
    print("make overlay-trace-session-template OVERLAY_TRACE=build/USA/trace-session.csv OVERLAY_TRACE_LIMIT=10")
    print("make overlay-trace-session-template OVERLAY_TRACE=build/USA/non-room-trace-session.csv OVERLAY_TRACE_ACTION=trace-read-owner-vram-entry OVERLAY_TRACE_LIMIT=10")
    print("make overlay-trace-session-template OVERLAY_TRACE=build/USA/room-trace-session.csv OVERLAY_TRACE_ACTION=trace-room-vram-entry OVERLAY_TRACE_LIMIT=10")
    print("make overlay-trace-session-kit OVERLAY_TRACE_SESSION=build/USA/room-trace-session OVERLAY_TRACE_ACTION=trace-room-vram-entry OVERLAY_TRACE_LIMIT=10")
    print("make overlay-family-trace-session-kit OVERLAY_TRACE_SESSION=build/USA/family-trace-session OVERLAY_TRACE_LIMIT=10")
    print(".venv/bin/python tools/scripts/overlay_debugger_plan.py --trace-action trace-room-vram-entry --limit 10")
    print("make overlay-trace-session-complete OVERLAY_TRACE_SHEET=build/USA/trace-session.csv OVERLAY_TRACE=build/USA/trace-complete.csv")
    print("make overlay-trace-session-preflight OVERLAY_TRACE_SHEET=build/USA/trace-session.csv")
    print("make overlay-trace-sheet-all-match ALL_MATCHES=1")
    print("make overlay-init-from-full-sheet DRY_RUN=1")
    print("make overlay-trace-smoke")
    print("make overlay-trace-hinted-smoke")
    print("make overlay-trace-room-hinted-smoke")
    print("make overlay-trace-effect-hinted-smoke")
    print("make overlay-trace-tiny-hinted-smoke")
    print("make overlay-verify")
    print("```")
    print()

    print("## Configured Targets")
    print()
    if configured:
        print("| Overlay | Slice | VRAM | Check |")
        print("|---|---|---:|---|")
        for overlay_id in sorted(configured):
            config = configured[overlay_id]
            print(
                f"| `{overlay_id}` | `{config.target_slice}` | "
                f"`0x{config.vram:08X}` | `make overlay-check OVERLAY={overlay_id}` |"
            )
    else:
        print("No configured overlay targets yet.")
    print()

    print("## Required Artifacts")
    print()
    print("| Artifact | Purpose |")
    print("|---|---|")
    print("| `docs/assets/code-overlays.csv` | Full 193-row candidate catalog. |")
    print("| `docs/assets/overlay-match-plan.csv` | One row per candidate with slice, evidence gate, and command. |")
    print("| `docs/assets/overlay-match-targets.md` / `docs/assets/overlay-match-targets.csv` | One-page match target coverage: byte-checkable targets, pending blockers, C/ASM coverage, and next commands. |")
    print("| `docs/assets/overlay-match-blockers.md` / `docs/assets/overlay-match-blockers.csv` | One-row-per-overlay blocker matrix combining runtime/config state, boundary risk, C targets, data islands, and shared tails. |")
    print("| `docs/assets/overlay-match-worklist.md` / `docs/assets/overlay-match-worklist.csv` | Per-overlay handoff list combining match state, split risk, first static span, first audited C target, and immediate action. |")
    print("| `docs/assets/overlay-match-worklist-trace-template.csv` | Raw trace/check CSV ordered by match worklist rank, with the first audited C target and post-trace command kept beside runtime blanks. |")
    print("| `docs/assets/overlay-family-match-worklist.md` / `docs/assets/overlay-family-match-worklist.csv` | Family-level match queue: representative, sibling reuse, first function, family function count, and trace/check command. |")
    print("| `docs/assets/overlay-family-trace-template.csv` | Raw trace/check CSV for family representatives only, ordered by family match rank. |")
    print("| `docs/assets/overlay-evidence-matrix.md` / `docs/assets/overlay-evidence-matrix.csv` | Per-overlay proof checklist for source, runtime read, VRAM, entry PC, committed config, and byte-checkability. |")
    print("| `docs/assets/overlay-scanner-audit.md` / `docs/assets/overlay-scanner-audit.csv` | Near-miss sector audit for code-like scanner signals outside standalone overlay candidates. |")
    print("| `docs/assets/overlay-call-map.md` / `docs/assets/overlay-call-map.csv` | External call target map for grouping overlays by render/GTE/audio/CD/shared-engine dependencies. |")
    print("| `docs/assets/overlay-domain-plan.md` / `docs/assets/overlay-domain-plan.csv` | One-row-per-overlay batching plan by dominant external call domain. |")
    print("| `docs/assets/overlay-trace-sheet-all.csv` | One trace row per candidate from `ovl_001` through `ovl_193`. |")
    print("| `docs/assets/overlay-trace-actions.md` / `docs/assets/overlay-trace-actions.csv` | Concrete breakpoint/action list for filling the full trace backlog. |")
    print("| `docs/assets/overlay-debugger-plan.md` / `docs/assets/overlay-debugger-plan.csv` | Combined trace/action and entry-hint checklist for read breakpoints, destination capture, and temporary execution breakpoints. |")
    print("| `docs/assets/overlay-trace-session-plan.md` | Debugger-session view of trace actions grouped by breakpoint, missing runtime fields, and first rows to capture. |")
    print("| `docs/assets/overlay-trace-readiness.md` / `docs/assets/overlay-trace-readiness.csv` | Config-readiness classification for the full trace backlog. |")
    print("| `docs/assets/overlay-consistency.md` / `docs/assets/overlay-consistency.csv` | Cross-report gate proving every overlay appears in the catalog, trace, evidence, readiness, match-target, and room/non-room views. |")
    print("| `docs/assets/overlay-config-backlog.md` / `docs/assets/overlay-config-backlog.csv` | Per-overlay checklist for missing runtime fields and the dry-run/write/check commands needed to create splat configs. |")
    print("| `docs/assets/overlay-config-priority.md` / `docs/assets/overlay-config-priority.csv` | Ranked queue for promoting traced rows into committed configs and byte-checkable match targets. |")
    print("| `docs/assets/overlay-priority-trace-template.csv` | Raw trace CSV template ordered by config priority, with sector/size prefilled and runtime address fields blank. |")
    print("| `docs/assets/overlay-family-plan.md` / `docs/assets/overlay-family-plan.csv` | Repeated-shape representatives to decompile before siblings. |")
    print("| `docs/assets/overlay-entry-hints.md` / `docs/assets/overlay-entry-hints.csv` | Static target-relative code/prologue/JR offsets used to narrow runtime entry tracing. |")
    print("| `docs/assets/overlay-static-function-hints.md` / `docs/assets/overlay-static-function-hints.csv` | Static prologue/JR/call counts and offset lists for planning decompilation after trace-backed configs exist. |")
    print("| `docs/assets/overlay-static-function-queue.md` / `docs/assets/overlay-static-function-queue.csv` | Smallest-first static function queue for picking first C match targets after trace-backed overlay configs exist. |")
    print("| `docs/assets/overlay-function-boundary-audit.md` / `docs/assets/overlay-function-boundary-audit.csv` | Per-overlay split-risk audit for static function boundaries before committing traced configs. |")
    print("| `docs/assets/overlay-undefined-tail-audit.md` / `docs/assets/overlay-undefined-tail-audit.csv` | Bounded configured-function audit for direct jumps into `undefined_extra` shared tails before C matching. |")
    print("| `docs/assets/overlay-splat-subsegment-hints.md` / `docs/assets/overlay-splat-subsegment-hints.csv` | Tentative splat subsegment starts derived from static function hints; use only after trace-backed configs. |")
    print("| `docs/assets/overlay-hinted-config-audit.md` / `docs/assets/overlay-hinted-config-audit.csv` | One-row-per-overlay audit of hinted config generation, target hashes, and smoke proof coverage. |")
    print("| `docs/assets/overlay-debugger-plan.md` / `docs/assets/overlay-debugger-plan.csv` | Breakpoint checklist generated from trace actions and static entry hints. |")
    print("| `docs/assets/overlay-function-status.md` / `docs/assets/overlay-function-status.csv` | Build-backed C/ASM symbol status for configured overlays. |")
    print("| `docs/assets/overlay-function-hypotheses.md` / `docs/assets/overlay-function-hypotheses.csv` | Working function names, confidence, evidence, and next decompilation actions for configured overlays. |")


def emit_loader_map_markdown(candidates: list[OverlayCandidate], config_dir: Path) -> None:
    configured = read_overlay_configs(config_dir, candidates)
    statuses = Counter(candidate.match_status for candidate in candidates)
    room_section3 = [
        candidate
        for candidate in candidates
        if candidate.recommended_target_slice == "room-section3"
    ]
    non_room = [
        candidate
        for candidate in candidates
        if candidate.recommended_target_slice == "scanner" and candidate.default_vram is None
    ]
    tiny = [candidate for candidate in non_room if candidate.match_status == "review-tiny-or-false-positive"]

    print("# Overlay Loader Evidence Map")
    print()
    print(
        "Generated from `tools/scripts/catalog_pe_overlays.py --format loader-map-md`. "
        "This report separates static loader evidence from runtime facts still needed "
        "before creating FF8-style split/build/check overlay configs."
    )
    print()
    print("## Summary")
    print()
    print(f"- Cataloged code-overlay candidates: {len(candidates)}")
    print(f"- Configured trace-backed overlays: {len(configured)}")
    print(f"- Room-section3 targets with static room owner/source sector: {len(room_section3)}")
    print(f"- Scanner-only non-room targets needing runtime owner/source confirmation: {len(non_room)}")
    print(f"- Tiny scanner hits requiring execution confirmation: {len(tiny)}")
    print()

    print("## Loader Sites")
    print()
    print("| Loader/site | Static evidence | Destination evidence | Entry evidence | Overlay impact |")
    print("|---|---|---|---|---|")
    print(
        "| `Scene_LoadRoom` room section 3 | `g_RoomDataLbaTbl` + "
        "`g_RoomDataOffsetTbl` give the PE.IMG room source sector; "
        "`code-overlays.csv` correlates 162 targets to `room-section3`. | "
        "Destination is loaded through a `g_GameState` pointer field in "
        "`Scene_LoadRoom`; the absolute value is runtime state and must be traced. | "
        "First executed PC is not stored in the static room table; trace execution "
        "after the section load. | 162 owner-known targets can become configs after "
        "one destination/entry trace each. |"
    )
    print(
        "| `Overlay_LoadInitialImage` intro path | Runtime trace for `ovl_009` proves "
        "PE sector `10994` and scanner slice SHA. | `0x8018EFF0` is trace-backed "
        "and committed in `configs/USA/overlays/ovl_009.yaml`. | `0x801909B4` "
        "is trace-backed; canonical base is `0x801909A0`. | Known-good overlay "
        "split/build/check template. |"
    )
    print(
        "| Non-room scanner hits | Scanner sectors are known, but most are not tied to "
        "a named loader table yet. | Destination must be captured from "
        "`CdRom_ReadSectors`/`CdRom_ReadSectorsFromLba` call state. | Entry PC must "
        "be captured from execution after the matching read. | 27 regular targets "
        "plus 3 tiny/review targets remain runtime-trace first. |"
    )
    print()

    print("## Match Readiness by Loader Class")
    print()
    print("| Class | Count | Static source proof | Missing proof before config | Next command |")
    print("|---|---:|---|---|---|")
    print(
        f"| Configured trace-backed | {statuses['match-ready']} | PE sector, size, "
        "destination, canonical base, and entry PC are known. | none | "
        "`make overlay-check-all` |"
    )
    print(
        f"| Room-owned section3 | {statuses['owner-known-needs-vram']} | Room owner, "
        "`room-section3` PE sector, sector count, and SHA are cataloged. | Runtime "
        "destination pointer, first executed PC, and committed config. | "
        "`make overlay-trace-sheet-all-status OVERLAY_TRACE_BATCH=all-room` |"
    )
    print(
        f"| Non-room scanner | {statuses['needs-runtime-trace']} | Scanner sector, "
        "sector count, SHA, calls, and domain profile are cataloged. | Runtime "
        "read caller/source confirmation, destination, first executed PC, and "
        "committed config. | `make overlay-trace-sheet-all-status "
        "OVERLAY_TRACE_BATCH=all-non-room` |"
    )
    print(
        f"| Tiny/review scanner | {statuses['review-tiny-or-false-positive']} | "
        "Scanner sector and SHA are cataloged. | Execution confirmation before "
        "treating the hit as a real overlay. | `make overlay-trace-sheet-all-status "
        "OVERLAY_TRACE_BATCH=all-tiny-review` |"
    )
    print()

    print("## Practical Breakpoints")
    print()
    print("| Goal | Breakpoint/function | Capture | Applies to |")
    print("|---|---|---|---|")
    print(
        "| Confirm source read | `CdRom_ReadSectors` / `CdRom_ReadSectorsFromLba` | "
        "`caller_pc,lba,pe_sector,dst,size` | all pending overlays |"
    )
    print(
        "| Confirm room section destination | `Scene_LoadRoom` calls into "
        "`CdRom_ReadSectorsFromLba` | resolved destination pointer for the "
        "`room-section3` read | room-owned overlays |"
    )
    print(
        "| Confirm executable entry | first PC executed inside the loaded destination "
        "range | `exec_pc`, then derive canonical base | all pending overlays |"
    )
    print()
    print(
        "The resulting rows go into `docs/assets/overlay-trace-sheet-all.csv`; once "
        "filled, use `make overlay-init-from-full-sheet DRY_RUN=1` before committing "
        "new overlay configs."
    )


def emit_overlay_detail_markdown(candidate: OverlayCandidate, config_dir: Path) -> None:
    configured = read_overlay_configs(config_dir, [candidate]).get(candidate.overlay_id)
    target_sector, target_sector_count, target_size, target_sha, target_sha_full = target_slice_info(
        candidate, candidate.recommended_target_slice
    )

    print(f"# {candidate.overlay_id} Overlay Detail")
    print()
    print("| Field | Value |")
    print("|---|---|")
    print(f"| Match status | `{candidate.match_status}` |")
    print(f"| Configured | {'yes' if configured else 'no'} |")
    print(f"| Role hypothesis | {candidate.role_hypothesis} |")
    print(f"| Role confidence | `{candidate.role_confidence}` |")
    print(f"| Recommended slice | `{candidate.recommended_target_slice}` |")
    print(f"| Target sector | `{target_sector}` |")
    print(f"| Target sector count | `{target_sector_count}` |")
    print(f"| Target size | `{target_size // 1024} KiB` |")
    print(f"| Target SHA-1/12 | `{target_sha}` |")
    print(f"| Target SHA-1 | `{target_sha_full}` |")
    print(f"| Scanner sector range | `{sector_range(candidate)}` |")
    print(f"| Scanner size | `{candidate.size_bytes // 1024} KiB` |")
    print(f"| Scanner SHA-1/12 | `{candidate.sha1_12}` |")
    print(f"| Room | `{room_label(candidate)}` |" if room_label(candidate) else "| Room |  |")
    if candidate.room_id is not None:
        print(f"| Room section-3 sector | `{candidate.room_section3_sector}` |")
        print(f"| Room section-3 size | `{candidate.room_section3_size_bytes // 1024} KiB` |")
        print(f"| Room section-3 SHA-1/12 | `{candidate.room_section3_sha1_12}` |")
    print(f"| Shape | `{candidate.shape}` |")
    print(f"| Duplicate group | `{candidate.duplicate_group}` |" if candidate.duplicate_group else "| Duplicate group |  |")
    print(f"| Tags | {', '.join(candidate.tags)} |")
    print(f"| Default VRAM | `0x{candidate.default_vram:08X}` |" if candidate.default_vram is not None else "| Default VRAM |  |")
    print(f"| Load VRAM | `0x{candidate.load_vram:08X}` |" if candidate.load_vram is not None else "| Load VRAM |  |")
    print(f"| Canonical VRAM | `0x{candidate.canonical_vram:08X}` |" if candidate.canonical_vram is not None else "| Canonical VRAM |  |")
    print(f"| Entry VRAM | `0x{candidate.entry_vram:08X}` |" if candidate.entry_vram is not None else "| Entry VRAM |  |")
    print()

    print("## Calls")
    print()
    print(f"- Top calls: {candidate.top_calls or '(none named)'}")
    print(f"- Call domains: {candidate.call_domains or '(none classified)'}")
    print()

    print("## Next Action")
    print()
    if configured:
        print(f"- Check: `make overlay-check OVERLAY={candidate.overlay_id}`")
    else:
        print(f"- Init after trace: `{trace_command(candidate)}`")
    print(f"- Match plan CSV: `docs/assets/overlay-match-plan.csv`")


def parse_overlay_id(value: str) -> int:
    if value.startswith("ovl_"):
        value = value[4:]
    try:
        index = int(value)
    except ValueError as exc:
        raise argparse.ArgumentTypeError("overlay must be like ovl_009 or 9") from exc
    if index < 1:
        raise argparse.ArgumentTypeError("overlay index must be positive")
    return index


def target_slice_info(candidate: OverlayCandidate, target_slice: str) -> tuple[int, int, int, str, str]:
    target_sector = candidate.sector_start
    target_sector_count = candidate.sector_count
    target_size = candidate.size_bytes
    target_sha = candidate.sha1_12
    target_sha_full = candidate.sha1
    if target_slice == "room-section3":
        if candidate.room_id is None or candidate.room_size3 is None:
            raise ValueError("room-section3 target requested for non-room candidate")
        target_sector = candidate.room_section3_sector or candidate.sector_start
        target_sector_count = candidate.room_size3
        target_size = target_sector_count * SECTOR_SIZE
        target_sha = candidate.room_section3_sha1_12 or candidate.sha1_12
        target_sha_full = candidate.room_section3_sha1 or candidate.sha1
    return target_sector, target_sector_count, target_size, target_sha, target_sha_full


ENTRY_HINT_FIELDS = [
    "overlay_id",
    "match_status",
    "target_slice",
    "target_sector",
    "target_sector_count",
    "scanner_sector_start",
    "scanner_delta_sectors",
    "code_offset_in_target",
    "code_file_offset",
    "first_prologue_offset_in_code",
    "first_prologue_offset_in_target",
    "first_jrra_offset_in_code",
    "first_jrra_offset_in_target",
    "entry_probe",
    "trace_or_check_command",
    "shape",
    "role_hypothesis",
    "call_domains",
]


def fmt_hex(value: int | None) -> str:
    return f"0x{value:X}" if value is not None else ""


def entry_hint_rows(
    candidates: list[OverlayCandidate], config_dir: Path
) -> list[dict[str, str | int]]:
    configured = read_overlay_configs(config_dir, candidates)
    rows: list[dict[str, str | int]] = []
    for candidate in sorted(candidates, key=lambda candidate: priority_score(candidate), reverse=True):
        target_sector, target_sector_count, _target_size, _target_sha, _target_sha_full = (
            target_slice_info(candidate, candidate.recommended_target_slice)
        )
        code_offset = candidate.offset - (target_sector * SECTOR_SIZE)
        first_prologue_target = (
            code_offset + candidate.first_prologue_offset
            if candidate.first_prologue_offset is not None
            else None
        )
        first_jrra_target = (
            code_offset + candidate.first_jrra_offset
            if candidate.first_jrra_offset is not None
            else None
        )
        config = configured.get(candidate.overlay_id)
        rows.append(
            {
                "overlay_id": candidate.overlay_id,
                "match_status": candidate.match_status,
                "target_slice": candidate.recommended_target_slice,
                "target_sector": target_sector,
                "target_sector_count": target_sector_count,
                "scanner_sector_start": candidate.sector_start,
                "scanner_delta_sectors": candidate.sector_start - target_sector,
                "code_offset_in_target": fmt_hex(code_offset),
                "code_file_offset": fmt_hex(candidate.offset),
                "first_prologue_offset_in_code": fmt_hex(candidate.first_prologue_offset),
                "first_prologue_offset_in_target": fmt_hex(first_prologue_target),
                "first_jrra_offset_in_code": fmt_hex(candidate.first_jrra_offset),
                "first_jrra_offset_in_target": fmt_hex(first_jrra_target),
                "entry_probe": loader_trace_fields(candidate)["entry_probe"],
                "trace_or_check_command": (
                    f"make overlay-check OVERLAY={candidate.overlay_id}"
                    if config is not None
                    else trace_command(candidate)
                ),
                "shape": candidate.shape,
                "role_hypothesis": candidate.role_hypothesis,
                "call_domains": candidate.call_domains,
            }
        )
    return rows


def emit_entry_hints_csv(candidates: list[OverlayCandidate], config_dir: Path) -> None:
    writer = csv.DictWriter(sys.stdout, fieldnames=ENTRY_HINT_FIELDS)
    writer.writeheader()
    writer.writerows(entry_hint_rows(candidates, config_dir))


def emit_entry_hints_markdown(candidates: list[OverlayCandidate], config_dir: Path) -> None:
    rows = entry_hint_rows(candidates, config_dir)
    print("# Overlay Entry Hints")
    print()
    print(
        "Generated from cataloged code-like words in each target slice. These are "
        "static offsets only: use them to narrow debugger `exec_pc` tracing, then "
        "trust the runtime first-executed PC before committing a config."
    )
    print()
    print(f"- Hint rows: {len(rows)}")
    print("- CSV source: `docs/assets/overlay-entry-hints.csv`")
    print("- Trace actions: `docs/assets/overlay-trace-actions.md`")
    print("- Debugger plan: `docs/assets/overlay-debugger-plan.md`")
    print()
    print(
        "| Overlay | Status | Slice | Target sector | Scanner delta | Code offset | "
        "First prologue | First JR RA | Entry probe | Command |"
    )
    print("|---|---|---|---:|---:|---:|---:|---:|---|---|")
    for row in rows:
        print(
            f"| `{row['overlay_id']}` | `{row['match_status']}` | `{row['target_slice']}` | "
            f"{row['target_sector']} | {row['scanner_delta_sectors']} | "
            f"`{row['code_offset_in_target']}` | "
            f"`{row['first_prologue_offset_in_target']}` | "
            f"`{row['first_jrra_offset_in_target']}` | "
            f"{row['entry_probe']} | `{row['trace_or_check_command']}` |"
        )


STATIC_FUNCTION_HINT_FIELDS = [
    "overlay_id",
    "match_status",
    "target_slice",
    "target_sector",
    "target_sector_count",
    "code_offset_in_target",
    "prologue_count",
    "jrra_count",
    "text_call_count",
    "first_prologue_offset_in_target",
    "first_jrra_offset_in_target",
    "prologue_offsets_in_target",
    "jrra_offsets_in_target",
    "entry_probe",
    "trace_or_check_command",
    "shape",
]

STATIC_FUNCTION_QUEUE_FIELDS = [
    "rank",
    "overlay_id",
    "match_status",
    "target_slice",
    "target_sector",
    "target_size_bytes",
    "function_index",
    "function_offset",
    "next_function_offset",
    "estimated_size_bytes",
    "suggested_symbol",
    "configured",
    "trace_or_check_command",
    "notes",
]

FUNCTION_BOUNDARY_AUDIT_FIELDS = [
    "overlay_id",
    "match_status",
    "target_slice",
    "target_sector",
    "target_size_bytes",
    "function_hint_count",
    "jrra_count",
    "header_size_bytes",
    "largest_static_span_bytes",
    "largest_static_span_offset",
    "tail_span_bytes",
    "risk_level",
    "risk_flags",
    "trace_or_check_command",
]


def static_function_hint_rows(
    pe_img: Path,
    candidates: list[OverlayCandidate],
    config_dir: Path,
) -> list[dict[str, str | int]]:
    image = pe_img.read_bytes()
    configured = read_overlay_configs(config_dir, candidates)
    rows: list[dict[str, str | int]] = []
    for candidate in sorted(candidates, key=lambda candidate: priority_score(candidate), reverse=True):
        target_sector, target_sector_count, _target_size, _target_sha, _target_sha_full = (
            target_slice_info(candidate, candidate.recommended_target_slice)
        )
        target_start = target_sector * SECTOR_SIZE
        target_end = target_start + target_sector_count * SECTOR_SIZE
        target = image[target_start:target_end]
        code_offset = candidate.offset - target_start
        prologue_offsets = word_offsets(target, lambda word: (word & 0xFFFF8000) == 0x27BD8000)
        jrra_offsets = word_offsets(target, lambda word: word == 0x03E00008)
        text_call_offsets = word_offsets(target, lambda word: (word & 0xFFF80000) == 0x0C000000)
        config = configured.get(candidate.overlay_id)
        rows.append(
            {
                "overlay_id": candidate.overlay_id,
                "match_status": candidate.match_status,
                "target_slice": candidate.recommended_target_slice,
                "target_sector": target_sector,
                "target_sector_count": target_sector_count,
                "code_offset_in_target": fmt_hex(code_offset),
                "prologue_count": len(prologue_offsets),
                "jrra_count": len(jrra_offsets),
                "text_call_count": len(text_call_offsets),
                "first_prologue_offset_in_target": fmt_hex(
                    prologue_offsets[0] if prologue_offsets else None
                ),
                "first_jrra_offset_in_target": fmt_hex(jrra_offsets[0] if jrra_offsets else None),
                "prologue_offsets_in_target": format_offset_list(prologue_offsets),
                "jrra_offsets_in_target": format_offset_list(jrra_offsets),
                "entry_probe": loader_trace_fields(candidate)["entry_probe"],
                "trace_or_check_command": (
                    f"make overlay-check OVERLAY={candidate.overlay_id}"
                    if config is not None
                    else trace_command(candidate)
                ),
                "shape": candidate.shape,
            }
        )
    return rows


def static_function_queue_rows(
    pe_img: Path,
    candidates: list[OverlayCandidate],
    config_dir: Path,
) -> list[dict[str, str | int]]:
    image = pe_img.read_bytes()
    configured = read_overlay_configs(config_dir, candidates)
    rows: list[dict[str, str | int]] = []
    for candidate in sorted(candidates, key=lambda candidate: candidate.overlay_id):
        target_sector, _target_sector_count, target_size, _target_sha, _target_sha_full = (
            target_slice_info(candidate, candidate.recommended_target_slice)
        )
        target_start = target_sector * SECTOR_SIZE
        target = image[target_start : target_start + target_size]
        function_offsets = word_offsets(target, lambda word: (word & 0xFFFF8000) == 0x27BD8000)
        config = configured.get(candidate.overlay_id)
        command = (
            f"make overlay-check OVERLAY={candidate.overlay_id}"
            if config is not None
            else trace_command(candidate)
        )
        for index, offset in enumerate(function_offsets, start=1):
            next_offset = (
                function_offsets[index]
                if index < len(function_offsets)
                else target_size
            )
            rows.append(
                {
                    "rank": 0,
                    "overlay_id": candidate.overlay_id,
                    "match_status": candidate.match_status,
                    "target_slice": candidate.recommended_target_slice,
                    "target_sector": target_sector,
                    "target_size_bytes": target_size,
                    "function_index": index,
                    "function_offset": fmt_hex(offset),
                    "next_function_offset": fmt_hex(next_offset),
                    "estimated_size_bytes": next_offset - offset,
                    "suggested_symbol": f"func_{candidate.overlay_id}_{offset:06X}",
                    "configured": "yes" if config is not None else "no",
                    "trace_or_check_command": command,
                    "notes": "static prologue span; verify after trace-backed split",
                }
            )
    rows.sort(
        key=lambda row: (
            int(row["estimated_size_bytes"]),
            str(row["configured"]) != "yes",
            str(row["overlay_id"]),
            int(row["function_index"]),
        )
    )
    for rank, row in enumerate(rows, start=1):
        row["rank"] = rank
    return rows


def function_boundary_risk(
    function_offsets: list[int],
    jrra_count: int,
    target_size: int,
    match_status: str,
) -> tuple[str, str]:
    flags: list[str] = []
    if not function_offsets:
        flags.append("no-prologue-hints")
    else:
        header_size = function_offsets[0]
        spans = [
            (function_offsets[index + 1] if index + 1 < len(function_offsets) else target_size)
            - offset
            for index, offset in enumerate(function_offsets)
        ]
        if header_size >= 0x800:
            flags.append("large-header")
        elif header_size >= 0x200:
            flags.append("medium-header")
        if max(spans) >= 0x10000:
            flags.append("very-large-span")
        elif max(spans) >= 0x4000:
            flags.append("large-span")
        if target_size - function_offsets[-1] >= 0x10000:
            flags.append("large-tail-span")
    if abs(jrra_count - len(function_offsets)) >= 16:
        flags.append("jrra-prologue-divergence")
    if match_status == "review-tiny-or-false-positive":
        flags.append("tiny-review")
    if not flags:
        flags.append("straightforward-static-boundaries")

    high_flags = {
        "no-prologue-hints",
        "large-header",
        "very-large-span",
        "large-tail-span",
        "tiny-review",
    }
    medium_flags = {"medium-header", "large-span", "jrra-prologue-divergence"}
    if any(flag in high_flags for flag in flags):
        risk_level = "high"
    elif any(flag in medium_flags for flag in flags):
        risk_level = "medium"
    else:
        risk_level = "low"
    return risk_level, ";".join(flags)


def function_boundary_audit_rows(
    pe_img: Path,
    candidates: list[OverlayCandidate],
    config_dir: Path,
) -> list[dict[str, str | int]]:
    image = pe_img.read_bytes()
    configured = read_overlay_configs(config_dir, candidates)
    rows: list[dict[str, str | int]] = []
    for candidate in sorted(candidates, key=lambda candidate: candidate.overlay_id):
        target_sector, _target_sector_count, target_size, _target_sha, _target_sha_full = (
            target_slice_info(candidate, candidate.recommended_target_slice)
        )
        target_start = target_sector * SECTOR_SIZE
        target = image[target_start : target_start + target_size]
        function_offsets = word_offsets(target, lambda word: (word & 0xFFFF8000) == 0x27BD8000)
        jrra_offsets = word_offsets(target, lambda word: word == 0x03E00008)
        spans = [
            (
                offset,
                (function_offsets[index + 1] if index + 1 < len(function_offsets) else target_size)
                - offset,
            )
            for index, offset in enumerate(function_offsets)
        ]
        largest_span_offset, largest_span = max(spans, key=lambda item: item[1]) if spans else (0, 0)
        header_size = function_offsets[0] if function_offsets else target_size
        tail_span = target_size - function_offsets[-1] if function_offsets else target_size
        risk_level, risk_flags = function_boundary_risk(
            function_offsets,
            len(jrra_offsets),
            target_size,
            candidate.match_status,
        )
        config = configured.get(candidate.overlay_id)
        rows.append(
            {
                "overlay_id": candidate.overlay_id,
                "match_status": candidate.match_status,
                "target_slice": candidate.recommended_target_slice,
                "target_sector": target_sector,
                "target_size_bytes": target_size,
                "function_hint_count": len(function_offsets),
                "jrra_count": len(jrra_offsets),
                "header_size_bytes": header_size,
                "largest_static_span_bytes": largest_span,
                "largest_static_span_offset": fmt_hex(largest_span_offset),
                "tail_span_bytes": tail_span,
                "risk_level": risk_level,
                "risk_flags": risk_flags,
                "trace_or_check_command": (
                    f"make overlay-check OVERLAY={candidate.overlay_id}"
                    if config is not None
                    else trace_command(candidate)
                ),
            }
        )
    return rows


def emit_static_function_hints_csv(
    pe_img: Path,
    candidates: list[OverlayCandidate],
    config_dir: Path,
) -> None:
    writer = csv.DictWriter(sys.stdout, fieldnames=STATIC_FUNCTION_HINT_FIELDS)
    writer.writeheader()
    writer.writerows(static_function_hint_rows(pe_img, candidates, config_dir))


def emit_static_function_queue_csv(
    pe_img: Path,
    candidates: list[OverlayCandidate],
    config_dir: Path,
) -> None:
    writer = csv.DictWriter(sys.stdout, fieldnames=STATIC_FUNCTION_QUEUE_FIELDS)
    writer.writeheader()
    writer.writerows(static_function_queue_rows(pe_img, candidates, config_dir))


def emit_function_boundary_audit_csv(
    pe_img: Path,
    candidates: list[OverlayCandidate],
    config_dir: Path,
) -> None:
    writer = csv.DictWriter(sys.stdout, fieldnames=FUNCTION_BOUNDARY_AUDIT_FIELDS)
    writer.writeheader()
    writer.writerows(function_boundary_audit_rows(pe_img, candidates, config_dir))


def emit_function_boundary_audit_markdown(
    pe_img: Path,
    candidates: list[OverlayCandidate],
    config_dir: Path,
) -> None:
    rows = function_boundary_audit_rows(pe_img, candidates, config_dir)
    risk_counts = Counter(str(row["risk_level"]) for row in rows)
    print("# Overlay Function Boundary Audit")
    print()
    print(
        "Generated from static prologue/JR patterns in each recommended target slice. "
        "This flags split-risk before turning a traced overlay into C work; runtime "
        "trace and SHA checks remain the source of truth."
    )
    print()
    print(f"- Audit rows: {len(rows)}")
    print("- CSV source: `docs/assets/overlay-function-boundary-audit.csv`")
    print("- Static function queue: `docs/assets/overlay-static-function-queue.md`")
    print("- Splat subsegment hints: `docs/assets/overlay-splat-subsegment-hints.md`")
    print("- Risk counts:")
    for risk, count in sorted(risk_counts.items()):
        print(f"  - `{risk}`: {count}")
    print()
    print(
        "| Overlay | Risk | Flags | Slice | Functions | JR RA | Header | "
        "Largest span | Span offset | Tail span | Command |"
    )
    print("|---|---|---|---|---:|---:|---:|---:|---:|---:|---|")
    for row in rows:
        print(
            f"| `{row['overlay_id']}` | `{row['risk_level']}` | `{row['risk_flags']}` | "
            f"`{row['target_slice']}` | {row['function_hint_count']} | {row['jrra_count']} | "
            f"{row['header_size_bytes']} | {row['largest_static_span_bytes']} | "
            f"`{row['largest_static_span_offset']}` | {row['tail_span_bytes']} | "
            f"`{row['trace_or_check_command']}` |"
        )


def emit_static_function_queue_markdown(
    pe_img: Path,
    candidates: list[OverlayCandidate],
    config_dir: Path,
) -> None:
    rows = static_function_queue_rows(pe_img, candidates, config_dir)
    configured_rows = [row for row in rows if row["configured"] == "yes"]
    print("# Overlay Static Function Queue")
    print()
    print(
        "Generated from static prologue spans in each recommended target slice. "
        "This is a decompilation queue for trace-backed overlays: use it after a "
        "config exists, then verify real boundaries with split/link/SHA checks."
    )
    print()
    print(f"- Function rows: {len(rows)}")
    print(f"- Configured rows: {len(configured_rows)}")
    print("- CSV source: `docs/assets/overlay-static-function-queue.csv`")
    print("- Static function hints: `docs/assets/overlay-static-function-hints.md`")
    print("- Hinted config audit: `docs/assets/overlay-hinted-config-audit.md`")
    print("- Markdown table: smallest 300 static spans")
    print()
    print(
        "| Rank | Overlay | Status | Slice | Function | Offset | Next | Est. bytes | "
        "Configured | Command |"
    )
    print("|---:|---|---|---|---|---:|---:|---:|---|---|")
    for row in rows[:300]:
        print(
            f"| {row['rank']} | `{row['overlay_id']}` | `{row['match_status']}` | "
            f"`{row['target_slice']}` | `{row['suggested_symbol']}` | "
            f"`{row['function_offset']}` | `{row['next_function_offset']}` | "
            f"{row['estimated_size_bytes']} | `{row['configured']}` | "
            f"`{row['trace_or_check_command']}` |"
        )


def emit_static_function_hints_markdown(
    pe_img: Path,
    candidates: list[OverlayCandidate],
    config_dir: Path,
) -> None:
    rows = static_function_hint_rows(pe_img, candidates, config_dir)
    print("# Overlay Static Function Hints")
    print()
    print(
        "Generated from target-slice MIPS word patterns. Counts and offsets are "
        "static hints for planning decompilation after runtime trace/config; they "
        "are not committed function boundaries."
    )
    print()
    print(f"- Hint rows: {len(rows)}")
    print("- CSV source: `docs/assets/overlay-static-function-hints.csv`")
    print("- Entry hints: `docs/assets/overlay-entry-hints.md`")
    print("- Debugger plan: `docs/assets/overlay-debugger-plan.md`")
    print()
    print(
        "| Overlay | Status | Slice | Sector | Prologues | JR RA | Calls | "
        "First prologue | First JR RA | Prologue offsets | Command |"
    )
    print("|---|---|---|---:|---:|---:|---:|---:|---:|---|---|")
    for row in rows:
        print(
            f"| `{row['overlay_id']}` | `{row['match_status']}` | `{row['target_slice']}` | "
            f"{row['target_sector']} | {row['prologue_count']} | {row['jrra_count']} | "
            f"{row['text_call_count']} | `{row['first_prologue_offset_in_target']}` | "
            f"`{row['first_jrra_offset_in_target']}` | "
            f"`{row['prologue_offsets_in_target']}` | `{row['trace_or_check_command']}` |"
        )


SPLAT_SUBSEGMENT_HINT_FIELDS = [
    "overlay_id",
    "match_status",
    "target_slice",
    "target_sector",
    "target_sector_count",
    "target_size_bytes",
    "code_offset_in_target",
    "function_hint_count",
    "header_size",
    "first_function_offset",
    "last_function_offset",
    "data_island_count",
    "data_island_starts",
    "suggested_subsegment_starts",
    "trace_or_check_command",
    "notes",
]


def suggested_subsegments(overlay_id: str, function_offsets: list[int], limit: int = 12) -> str:
    entries: list[str] = []
    if not function_offsets or function_offsets[0] != 0:
        entries.append(f"0x0:asm:{overlay_id}_header")
    for offset in function_offsets[:limit]:
        entries.append(f"{fmt_hex(offset)}:asm:func_{overlay_id}_{offset:06X}")
    if len(function_offsets) > limit:
        entries.append(f"+{len(function_offsets) - limit} more function starts")
    return ";".join(entries)


def read_data_island_vrams(
    audit_csv: Path = FUNCTION_KIND_AUDIT_CSV,
) -> dict[str, list[int]]:
    if not audit_csv.exists():
        return {}
    islands: dict[str, list[int]] = {}
    with audit_csv.open(newline="") as handle:
        for row in csv.DictReader(handle):
            if row.get("match_class") != "data-island":
                continue
            try:
                vram = int(row["vram"], 16)
            except (KeyError, TypeError, ValueError):
                continue
            islands.setdefault(row["overlay_id"], []).append(vram)
    return {overlay_id: sorted(set(vrams)) for overlay_id, vrams in islands.items()}


def data_island_offsets(
    candidate: OverlayCandidate,
    target_size: int,
    island_vrams: list[int],
) -> list[int]:
    if not island_vrams:
        return []
    bases = [
        candidate.load_vram,
        candidate.canonical_vram,
        candidate.default_vram,
        0x80100000,
    ]
    seen: set[int] = set()
    for base in bases:
        if base is None or base in seen:
            continue
        seen.add(base)
        offsets = [vram - base for vram in island_vrams]
        if all(0 <= offset < target_size for offset in offsets):
            return sorted(set(offsets))
    return []


def suggested_data_islands(overlay_id: str, offsets: list[int]) -> str:
    return ";".join(
        f"{fmt_hex(offset)}:asm:{overlay_id}_data_{offset:06X}" for offset in offsets
    )


def function_hint_offsets(pe_img: Path, candidate: OverlayCandidate, target_slice: str) -> list[int]:
    target_sector, _target_sector_count, target_size, _target_sha, _target_sha_full = target_slice_info(
        candidate, target_slice
    )
    target_start = target_sector * SECTOR_SIZE
    target = pe_img.read_bytes()[target_start : target_start + target_size]
    return word_offsets(target, lambda word: (word & 0xFFFF8000) == 0x27BD8000)


def splat_subsegment_hint_rows(
    pe_img: Path,
    candidates: list[OverlayCandidate],
    config_dir: Path,
) -> list[dict[str, str | int]]:
    image = pe_img.read_bytes()
    configured = read_overlay_configs(config_dir, candidates)
    data_island_vrams = read_data_island_vrams()
    rows: list[dict[str, str | int]] = []
    for candidate in sorted(candidates, key=lambda candidate: priority_score(candidate), reverse=True):
        target_sector, target_sector_count, target_size, _target_sha, _target_sha_full = (
            target_slice_info(candidate, candidate.recommended_target_slice)
        )
        target_start = target_sector * SECTOR_SIZE
        target_end = target_start + target_size
        target = image[target_start:target_end]
        function_offsets = word_offsets(target, lambda word: (word & 0xFFFF8000) == 0x27BD8000)
        island_offsets = data_island_offsets(
            candidate,
            target_size,
            data_island_vrams.get(candidate.overlay_id, []),
        )
        code_offset = candidate.offset - target_start
        config = configured.get(candidate.overlay_id)
        if config is not None:
            notes = "configured; compare against committed YAML before changing subsegments"
        elif function_offsets:
            notes = "static starts only; commit YAML only after runtime trace and SHA check"
        else:
            notes = "no prologue starts found; keep asm-only until split evidence improves"
        if island_offsets:
            notes += "; data-island starts from overlay-function-kind-audit"
        rows.append(
            {
                "overlay_id": candidate.overlay_id,
                "match_status": (
                    "match-ready" if config is not None else candidate.match_status
                ),
                "target_slice": candidate.recommended_target_slice,
                "target_sector": target_sector,
                "target_sector_count": target_sector_count,
                "target_size_bytes": target_size,
                "code_offset_in_target": fmt_hex(code_offset),
                "function_hint_count": len(function_offsets),
                "header_size": fmt_hex(function_offsets[0] if function_offsets else target_size),
                "first_function_offset": fmt_hex(function_offsets[0] if function_offsets else None),
                "last_function_offset": fmt_hex(function_offsets[-1] if function_offsets else None),
                "data_island_count": len(island_offsets),
                "data_island_starts": suggested_data_islands(
                    candidate.overlay_id,
                    island_offsets,
                ),
                "suggested_subsegment_starts": suggested_subsegments(
                    candidate.overlay_id,
                    function_offsets,
                ),
                "trace_or_check_command": (
                    f"make overlay-check OVERLAY={candidate.overlay_id}"
                    if config is not None
                    else trace_command(candidate)
                ),
                "notes": notes,
            }
        )
    return rows


def emit_splat_subsegment_hints_csv(
    pe_img: Path,
    candidates: list[OverlayCandidate],
    config_dir: Path,
) -> None:
    writer = csv.DictWriter(sys.stdout, fieldnames=SPLAT_SUBSEGMENT_HINT_FIELDS)
    writer.writeheader()
    writer.writerows(splat_subsegment_hint_rows(pe_img, candidates, config_dir))


def emit_splat_subsegment_hints_markdown(
    pe_img: Path,
    candidates: list[OverlayCandidate],
    config_dir: Path,
) -> None:
    rows = splat_subsegment_hint_rows(pe_img, candidates, config_dir)
    print("# Overlay Splat Subsegment Hints")
    print()
    print(
        "Generated from static prologue starts in each recommended target slice. "
        "These rows are planning aids for future splat YAML edits; they are not "
        "trace evidence and should not be committed as configs without runtime "
        "VRAM/entry proof and a passing SHA check."
    )
    print()
    print(f"- Hint rows: {len(rows)}")
    print("- CSV source: `docs/assets/overlay-splat-subsegment-hints.csv`")
    print("- Static function hints: `docs/assets/overlay-static-function-hints.md`")
    print("- Config backlog: `docs/assets/overlay-config-backlog.md`")
    print()
    print(
        "| Overlay | Status | Slice | Sector | Size | Function starts | Header | "
        "First | Last | Data islands | Data starts | Suggested starts | Command |"
    )
    print("|---|---|---|---:|---:|---:|---:|---:|---:|---:|---|---|---|")
    for row in rows:
        print(
            f"| `{row['overlay_id']}` | `{row['match_status']}` | `{row['target_slice']}` | "
            f"{row['target_sector']} | {row['target_size_bytes']} | "
            f"{row['function_hint_count']} | `{row['header_size']}` | "
            f"`{row['first_function_offset']}` | `{row['last_function_offset']}` | "
            f"{row['data_island_count']} | `{row['data_island_starts']}` | "
            f"`{row['suggested_subsegment_starts']}` | `{row['trace_or_check_command']}` |"
        )


HINTED_CONFIG_AUDIT_FIELDS = [
    "overlay_id",
    "match_status",
    "target_slice",
    "target_sector",
    "target_sector_count",
    "target_size_bytes",
    "target_sha1",
    "function_hint_count",
    "hinted_subsegment_count",
    "coverage_status",
    "proof_command",
    "next_action",
]


HINTED_SMOKE_PROOFS = {
    "ovl_002": ("title/intro non-room smoke-proven", "make overlay-trace-hinted-smoke"),
    "ovl_024": ("room-section3 smoke-proven", "make overlay-trace-room-hinted-smoke"),
    "ovl_189": ("non-room effect smoke-proven", "make overlay-trace-effect-hinted-smoke"),
    "ovl_006": ("tiny/review smoke-proven", "make overlay-trace-tiny-hinted-smoke"),
}


def hinted_subsegment_count(function_offsets: list[int]) -> int:
    if not function_offsets:
        return 1
    return len(function_offsets) + (0 if function_offsets[0] == 0 else 1)


def configured_subsegment_counts(
    config_dir: Path,
    candidates: list[OverlayCandidate],
) -> dict[str, int]:
    candidate_ids = {candidate.overlay_id for candidate in candidates}
    counts: dict[str, int] = {}
    for path in sorted(config_dir.glob("*.yaml")):
        if path.stem not in candidate_ids:
            continue
        subsegments, _end = read_overlay_subsegments(path.read_text())
        counts[path.stem] = len(subsegments)
    return counts


def hinted_config_audit_rows(
    pe_img: Path,
    candidates: list[OverlayCandidate],
    config_dir: Path,
) -> list[dict[str, str | int]]:
    image = pe_img.read_bytes()
    configured = read_overlay_configs(config_dir, candidates)
    configured_counts = configured_subsegment_counts(config_dir, candidates)
    rows: list[dict[str, str | int]] = []
    for candidate in sorted(candidates, key=lambda candidate: candidate.overlay_id):
        target_sector, target_sector_count, target_size, _target_sha, target_sha1 = (
            target_slice_info(candidate, candidate.recommended_target_slice)
        )
        target_start = target_sector * SECTOR_SIZE
        target = image[target_start : target_start + target_size]
        function_offsets = word_offsets(target, lambda word: (word & 0xFFFF8000) == 0x27BD8000)
        smoke = HINTED_SMOKE_PROOFS.get(candidate.overlay_id)
        if candidate.overlay_id in configured:
            coverage_status = "committed-match-target"
            proof_command = f"make overlay-check OVERLAY={candidate.overlay_id}"
            next_action = "replace asm with C incrementally"
        elif smoke is not None:
            coverage_status, proof_command = smoke
            next_action = "capture real runtime trace before committing a config"
        else:
            coverage_status = "hinted-config-generatable"
            proof_command = trace_command(candidate)
            next_action = "capture runtime trace, then run overlay-check-from-trace with HINTED_SUBSEGMENTS=1"
        rows.append(
            {
                "overlay_id": candidate.overlay_id,
                "match_status": (
                    "match-ready"
                    if candidate.overlay_id in configured
                    else candidate.match_status
                ),
                "target_slice": candidate.recommended_target_slice,
                "target_sector": target_sector,
                "target_sector_count": target_sector_count,
                "target_size_bytes": target_size,
                "target_sha1": target_sha1,
                "function_hint_count": len(function_offsets),
                "hinted_subsegment_count": configured_counts.get(
                    candidate.overlay_id,
                    hinted_subsegment_count(function_offsets),
                ),
                "coverage_status": coverage_status,
                "proof_command": proof_command,
                "next_action": next_action,
            }
        )
    return rows


def emit_hinted_config_audit_csv(
    pe_img: Path,
    candidates: list[OverlayCandidate],
    config_dir: Path,
) -> None:
    writer = csv.DictWriter(sys.stdout, fieldnames=HINTED_CONFIG_AUDIT_FIELDS)
    writer.writeheader()
    writer.writerows(hinted_config_audit_rows(pe_img, candidates, config_dir))


def emit_hinted_config_audit_markdown(
    pe_img: Path,
    candidates: list[OverlayCandidate],
    config_dir: Path,
) -> None:
    rows = hinted_config_audit_rows(pe_img, candidates, config_dir)
    status_counts = Counter(str(row["coverage_status"]) for row in rows)
    print("# Overlay Hinted Config Audit")
    print()
    print(
        "Generated from static prologue hints and catalog target slices. This is a "
        "metadata-only audit of whether each candidate can produce an asm-only "
        "hinted splat config after runtime trace evidence is captured."
    )
    print()
    print(f"- Audit rows: {len(rows)}")
    print("- CSV source: `docs/assets/overlay-hinted-config-audit.csv`")
    print("- Hinted mode: `HINTED_SUBSEGMENTS=1`")
    print("- Required final proof: `make overlay-check-from-trace ... HINTED_SUBSEGMENTS=1`")
    print("- Coverage statuses:")
    for status, count in sorted(status_counts.items()):
        print(f"  - `{status}`: {count}")
    print()
    print(
        "| Overlay | Status | Slice | Sector | Size | Function starts | "
        "Subsegments | Coverage | Proof / next command |"
    )
    print("|---|---|---|---:|---:|---:|---:|---|---|")
    for row in rows:
        print(
            f"| `{row['overlay_id']}` | `{row['match_status']}` | `{row['target_slice']}` | "
            f"{row['target_sector']} | {row['target_size_bytes']} | "
            f"{row['function_hint_count']} | {row['hinted_subsegment_count']} | "
            f"`{row['coverage_status']}` | `{row['proof_command']}` |"
        )


def emit_splat_yaml(
    candidate: OverlayCandidate,
    vram: int,
    target_slice: str,
    pe_img: Path | None = None,
    hinted_subsegments: bool = False,
) -> None:
    """Emit a first-pass splat config skeleton for one local overlay target."""
    basename = candidate.overlay_id
    target_sector, target_sector_count, target_size, _target_sha, target_sha_full = target_slice_info(
        candidate, target_slice
    )
    function_offsets: list[int] = []
    if hinted_subsegments:
        if pe_img is None:
            raise ValueError("hinted subsegment generation requires PE.IMG")
        function_offsets = function_hint_offsets(pe_img, candidate, target_slice)

    print(f"name: {basename}.bin")
    print(f"sha1: {target_sha_full}")
    print("options:")
    print(f"  basename: {basename}")
    print(f"  target_path: original/USA/overlays/{basename}.bin")
    print("  base_path: ../../..")
    print("  platform: psx")
    print("  compiler: GCC")
    print(f"  asm_path: asm/USA/overlays/{basename}")
    print(f"  src_path: src/overlays/{basename}")
    print(f"  build_path: build/USA/overlays/{basename}")
    print(f"  ld_script_path: linkers/USA/overlays/{basename}.ld")
    print(f"  undefined_funcs_auto_path: linkers/USA/overlays/undefined_funcs_auto.{basename}.txt")
    print(f"  undefined_syms_auto_path: linkers/USA/overlays/undefined_syms_auto.{basename}.txt")
    print("  find_file_boundaries: True")
    print("  disassemble_all: True")
    print("  asm_function_macro: glabel")
    print("  asm_jtbl_label_macro: jlabel")
    print("  asm_data_macro: dlabel")
    print("  generate_asm_macros_files: False")
    print('  section_order: [".rodata", ".text", ".data", ".sdata", ".sbss", ".bss"]')
    print("  subalign: 4")
    print("  string_encoding: ASCII")
    print("  data_string_encoding: ASCII")
    print("  rodata_string_guesser_level: 2")
    print("  data_string_guesser_level: 2")
    print("segments:")
    print(f"  - name: {basename}")
    print("    type: code")
    print("    start: 0x0")
    print(f"    vram: 0x{vram:08X}")
    print("    align: 4")
    print("    subalign: 4")
    print("    bss_size: 0x0")
    print("    subsegments:")
    if hinted_subsegments and function_offsets:
        print("      # generated from static prologue hints; verify with runtime trace and SHA check")
        if function_offsets[0] != 0:
            print(f"      - [0x0, asm, {basename}_header]")
        for offset in function_offsets:
            print(f"      - [{fmt_hex(offset)}, asm, func_{basename}_{offset:06X}]")
    else:
        print(f"      - [0x0, asm, {basename}]")
    print(f"  - [0x{target_size:X}]")
    if candidate.room_id is not None:
        print(
            "# room: "
            f"m{candidate.room_id:03d}, section3 sector "
            f"{candidate.room_section3_sector}, scanner delta {candidate.room_delta}"
        )
    print(f"# source slice: {target_slice}")
    print(
        "# extract locally with: "
        f"dd if=disc/extracted/PE.IMG of=original/USA/overlays/{basename}.bin "
        f"bs=2048 skip={target_sector} count={target_sector_count}"
    )


def extract_overlay(
    pe_img: Path,
    candidate: OverlayCandidate,
    target_slice: str,
    out_dir: Path,
) -> Path:
    target_sector, target_sector_count, target_size, target_sha, _target_sha_full = target_slice_info(
        candidate, target_slice
    )
    out_dir.mkdir(parents=True, exist_ok=True)
    out_path = out_dir / f"{candidate.overlay_id}.bin"
    with pe_img.open("rb") as src:
        src.seek(target_sector * SECTOR_SIZE)
        data = src.read(target_size)
    if len(data) != target_size:
        raise ValueError(
            f"short read for {candidate.overlay_id}: got {len(data)}, expected {target_size}"
        )
    actual = hashlib.sha1(data).hexdigest()[:12]
    if actual != target_sha:
        raise ValueError(f"sha mismatch for {candidate.overlay_id}: {actual} != {target_sha}")
    out_path.write_bytes(data)
    return out_path


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("pe_img", type=Path, help="Path to extracted PE.IMG")
    parser.add_argument(
        "main_exe",
        type=Path,
        nargs="?",
        help="Path to built or extracted main.exe for room-table correlation",
    )
    parser.add_argument(
        "--sym-file",
        type=Path,
        help="Optional sym.main.txt file used to name top external call targets",
    )
    parser.add_argument(
        "--config-yaml",
        type=Path,
        help="Optional splat main.yaml used to name top external call targets",
    )
    parser.add_argument(
        "--format",
        choices=(
            "summary",
            "csv",
            "md",
            "json",
            "priority-md",
            "config-md",
            "status-md",
            "plan-csv",
            "plan-md",
            "evidence-csv",
            "evidence-md",
            "scanner-audit-csv",
            "scanner-audit-md",
            "call-map-csv",
            "call-map-md",
            "domain-plan-csv",
            "domain-plan-md",
            "family-csv",
            "family-md",
            "family-match-worklist-csv",
            "family-match-worklist-md",
            "entry-hints-csv",
            "entry-hints-md",
            "static-function-hints-csv",
            "static-function-hints-md",
            "static-function-queue-csv",
            "static-function-queue-md",
            "function-boundary-audit-csv",
            "function-boundary-audit-md",
            "splat-subsegment-hints-csv",
            "splat-subsegment-hints-md",
            "hinted-config-audit-csv",
            "hinted-config-audit-md",
            "room-map-csv",
            "room-map-md",
            "non-room-plan-csv",
            "non-room-plan-md",
            "trace-batches-md",
            "trace-sheet-csv",
            "trace-sheet-all-csv",
            "trace-actions-csv",
            "trace-actions-md",
            "match-targets-csv",
            "match-targets-md",
            "match-worklist-csv",
            "match-worklist-md",
            "loader-map-md",
            "readiness-md",
            "decomp-status-md",
            "decomp-status-csv",
            "detail-md",
            "splat-yaml",
            "extract",
        ),
        default="summary",
        help="Output format",
    )
    parser.add_argument(
        "--overlay",
        type=parse_overlay_id,
        help="Overlay to use with --format splat-yaml, e.g. ovl_009",
    )
    parser.add_argument(
        "--vram",
        type=lambda value: int(value, 0),
        help="Runtime load address for --format splat-yaml",
    )
    parser.add_argument(
        "--target-slice",
        choices=("scanner", "room-section3"),
        default="scanner",
        help="Source slice for --format splat-yaml",
    )
    parser.add_argument(
        "--out-dir",
        type=Path,
        default=Path("original/USA/overlays"),
        help="Output directory for --format extract",
    )
    parser.add_argument(
        "--overlay-config-dir",
        type=Path,
        default=Path("configs/USA/overlays"),
        help="Committed overlay config directory for --format config-md",
    )
    args = parser.parse_args()

    image_sectors, candidates = build_catalog(
        args.pe_img,
        args.main_exe,
        args.sym_file,
        args.config_yaml,
    )
    if args.format == "summary":
        emit_summary(image_sectors, candidates)
    elif args.format == "csv":
        emit_csv(candidates)
    elif args.format == "md":
        emit_markdown(candidates)
    elif args.format == "json":
        emit_json(candidates)
    elif args.format == "priority-md":
        emit_priority_markdown(candidates)
    elif args.format == "config-md":
        emit_config_markdown(candidates, args.overlay_config_dir)
    elif args.format == "status-md":
        emit_status_markdown(candidates, args.overlay_config_dir)
    elif args.format == "plan-csv":
        emit_plan_csv(candidates, args.overlay_config_dir)
    elif args.format == "plan-md":
        emit_plan_markdown(candidates, args.overlay_config_dir)
    elif args.format == "evidence-csv":
        emit_evidence_csv(candidates, args.overlay_config_dir)
    elif args.format == "evidence-md":
        emit_evidence_markdown(candidates, args.overlay_config_dir)
    elif args.format == "scanner-audit-csv":
        emit_scanner_audit_csv(args.pe_img, candidates)
    elif args.format == "scanner-audit-md":
        emit_scanner_audit_markdown(args.pe_img, candidates)
    elif args.format == "call-map-csv":
        emit_call_map_csv(args.pe_img, candidates, args.sym_file, args.config_yaml)
    elif args.format == "call-map-md":
        emit_call_map_markdown(args.pe_img, candidates, args.sym_file, args.config_yaml)
    elif args.format == "domain-plan-csv":
        emit_domain_plan_csv(candidates, args.overlay_config_dir)
    elif args.format == "domain-plan-md":
        emit_domain_plan_markdown(candidates, args.overlay_config_dir)
    elif args.format == "family-csv":
        emit_family_csv(candidates)
    elif args.format == "family-md":
        emit_family_markdown(candidates)
    elif args.format == "family-match-worklist-csv":
        emit_family_match_worklist_csv(args.pe_img, candidates, args.overlay_config_dir)
    elif args.format == "family-match-worklist-md":
        emit_family_match_worklist_markdown(args.pe_img, candidates, args.overlay_config_dir)
    elif args.format == "entry-hints-csv":
        emit_entry_hints_csv(candidates, args.overlay_config_dir)
    elif args.format == "entry-hints-md":
        emit_entry_hints_markdown(candidates, args.overlay_config_dir)
    elif args.format == "static-function-hints-csv":
        emit_static_function_hints_csv(args.pe_img, candidates, args.overlay_config_dir)
    elif args.format == "static-function-hints-md":
        emit_static_function_hints_markdown(args.pe_img, candidates, args.overlay_config_dir)
    elif args.format == "static-function-queue-csv":
        emit_static_function_queue_csv(args.pe_img, candidates, args.overlay_config_dir)
    elif args.format == "static-function-queue-md":
        emit_static_function_queue_markdown(args.pe_img, candidates, args.overlay_config_dir)
    elif args.format == "function-boundary-audit-csv":
        emit_function_boundary_audit_csv(args.pe_img, candidates, args.overlay_config_dir)
    elif args.format == "function-boundary-audit-md":
        emit_function_boundary_audit_markdown(args.pe_img, candidates, args.overlay_config_dir)
    elif args.format == "splat-subsegment-hints-csv":
        emit_splat_subsegment_hints_csv(args.pe_img, candidates, args.overlay_config_dir)
    elif args.format == "splat-subsegment-hints-md":
        emit_splat_subsegment_hints_markdown(args.pe_img, candidates, args.overlay_config_dir)
    elif args.format == "hinted-config-audit-csv":
        emit_hinted_config_audit_csv(args.pe_img, candidates, args.overlay_config_dir)
    elif args.format == "hinted-config-audit-md":
        emit_hinted_config_audit_markdown(args.pe_img, candidates, args.overlay_config_dir)
    elif args.format == "room-map-csv":
        emit_room_map_csv(candidates, args.overlay_config_dir)
    elif args.format == "room-map-md":
        emit_room_map_markdown(candidates, args.overlay_config_dir)
    elif args.format == "non-room-plan-csv":
        emit_non_room_plan_csv(candidates, args.overlay_config_dir)
    elif args.format == "non-room-plan-md":
        emit_non_room_plan_markdown(candidates, args.overlay_config_dir)
    elif args.format == "trace-batches-md":
        emit_trace_batches_markdown(candidates, args.overlay_config_dir)
    elif args.format == "trace-sheet-csv":
        emit_trace_sheet_csv(candidates, args.overlay_config_dir)
    elif args.format == "trace-sheet-all-csv":
        emit_trace_sheet_all_csv(candidates, args.overlay_config_dir)
    elif args.format == "trace-actions-csv":
        emit_trace_actions_csv(candidates, args.overlay_config_dir)
    elif args.format == "trace-actions-md":
        emit_trace_actions_markdown(candidates, args.overlay_config_dir)
    elif args.format == "match-targets-csv":
        emit_match_targets_csv(candidates, args.overlay_config_dir)
    elif args.format == "match-targets-md":
        emit_match_targets_markdown(candidates, args.overlay_config_dir)
    elif args.format == "match-worklist-csv":
        emit_match_worklist_csv(args.pe_img, candidates, args.overlay_config_dir)
    elif args.format == "match-worklist-md":
        emit_match_worklist_markdown(args.pe_img, candidates, args.overlay_config_dir)
    elif args.format == "loader-map-md":
        emit_loader_map_markdown(candidates, args.overlay_config_dir)
    elif args.format == "readiness-md":
        emit_match_readiness_markdown(candidates, args.overlay_config_dir)
    elif args.format == "decomp-status-md":
        emit_decomp_status_markdown(candidates, args.overlay_config_dir)
    elif args.format == "decomp-status-csv":
        emit_decomp_status_csv(candidates, args.overlay_config_dir)
    elif args.format == "detail-md":
        if args.overlay is None:
            parser.error("--format detail-md requires --overlay")
        if args.overlay > len(candidates):
            parser.error(f"overlay index out of range: {args.overlay}")
        emit_overlay_detail_markdown(candidates[args.overlay - 1], args.overlay_config_dir)
    elif args.format == "splat-yaml":
        if args.overlay is None:
            parser.error("--format splat-yaml requires --overlay")
        if args.overlay > len(candidates):
            parser.error(f"overlay index out of range: {args.overlay}")
        candidate = candidates[args.overlay - 1]
        vram = args.vram if args.vram is not None else default_vram(candidate)
        if vram is None:
            parser.error("--format splat-yaml requires --vram for this overlay")
        try:
            emit_splat_yaml(candidate, vram, args.target_slice)
        except ValueError as exc:
            parser.error(str(exc))
    else:
        if args.overlay is None:
            parser.error("--format extract requires --overlay")
        if args.overlay > len(candidates):
            parser.error(f"overlay index out of range: {args.overlay}")
        candidate = candidates[args.overlay - 1]
        try:
            out_path = extract_overlay(args.pe_img, candidate, args.target_slice, args.out_dir)
        except ValueError as exc:
            parser.error(str(exc))
        print(out_path)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
