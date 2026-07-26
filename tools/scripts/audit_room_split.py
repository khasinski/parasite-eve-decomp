#!/usr/bin/env python3
"""Audit room overlay split consistency.

This is a structural audit of the room YAMLs against the currently generated
asm. It intentionally checks only the exact asm file named by each current YAML
subsegment; stale generated files can remain in asm directories after split
changes and should not be treated as authoritative.
"""
from __future__ import annotations

import argparse
import collections
import json
import re
import struct
from pathlib import Path
from typing import Any

import yaml


ROOT = Path(__file__).resolve().parents[2]
CONFIG_DIR = ROOT / "configs/USA/overlays"
ASM_DIR = ROOT / "asm/USA/overlays"
ORIGINAL_DIR = ROOT / "original/USA/overlays"


def as_int(value: Any) -> int:
    if isinstance(value, int):
        return value
    if isinstance(value, str):
        return int(value, 0)
    raise TypeError(f"cannot parse integer from {value!r}")


def read_yaml(path: Path) -> dict[str, Any]:
    with path.open() as f:
        return yaml.safe_load(f)


def segment_end(room: str, cfg: dict[str, Any]) -> int:
    segments = cfg["segments"]
    if len(segments) > 1 and isinstance(segments[1], list):
        return as_int(segments[1][0])
    return (ORIGINAL_DIR / f"{room}.bin").stat().st_size


def subsegments(cfg: dict[str, Any]) -> list[tuple[int, str, str]]:
    rows: list[tuple[int, str, str]] = []
    for ss in cfg["segments"][0]["subsegments"]:
        if not isinstance(ss, list):
            continue
        off = as_int(ss[0])
        typ = str(ss[1])
        name = str(ss[2]) if len(ss) > 2 else ""
        rows.append((off, typ, name))
    rows.sort()
    return rows


def parse_nonmatching_lines(text: str) -> list[tuple[str, int | None]]:
    rows: list[tuple[str, int | None]] = []
    for line in text.splitlines():
        line = line.strip()
        if not line.startswith("nonmatching "):
            continue
        rest = line[len("nonmatching ") :].strip()
        parts = [p.strip() for p in rest.split(",", 1)]
        name = parts[0]
        size = None
        if len(parts) > 1 and parts[1]:
            size = int(parts[1], 0)
        rows.append((name, size))
    return rows


def asm_features(text: str) -> set[str]:
    features: set[str] = set()

    if re.search(r"\b(?:ctc2|lwc2|swc2|mvmva)\b", text):
        features.add("gte_or_scratchpad_handler")
    if "jtbl_" in text or re.search(r"\bjr\s+\$v0\b", text):
        features.add("switch_or_jump_table")
    if re.search(r"\bjal\b", text):
        features.add("calls_other_functions")
    if not re.search(r"\bjr\s+\$ra\b", text):
        features.add("no_local_return")
    if not features:
        features.add("plain_local_function")

    return features


def exact_asm_checks(
    room: str, subs: list[tuple[int, str, str]], end: int
) -> tuple[
    list[dict[str, Any]],
    list[dict[str, Any]],
    list[dict[str, Any]],
    list[dict[str, Any]],
    collections.Counter[str],
    collections.Counter[str],
    collections.Counter[str],
]:
    missing: list[dict[str, Any]] = []
    ambiguous: list[dict[str, Any]] = []
    mismatches: list[dict[str, Any]] = []
    historical_names: list[dict[str, Any]] = []
    families: collections.Counter[str] = collections.Counter()
    feature_totals: collections.Counter[str] = collections.Counter()
    feature_counts: collections.Counter[str] = collections.Counter()
    used_names = collections.Counter(name for _off, _typ, name in subs if name)

    for i, (off, typ, name) in enumerate(subs):
        next_off = subs[i + 1][0] if i + 1 < len(subs) else end
        size = next_off - off
        if typ != "asm":
            continue

        asm_file = ASM_DIR / room / f"{name}.s"
        if not asm_file.exists():
            missing.append({"room": room, "offset": off, "name": name, "size": size})
            continue

        text = asm_file.read_text(errors="ignore")
        nonmatching = parse_nonmatching_lines(text)
        glabels = re.findall(r"^glabel\s+(\S+)", text, flags=re.M)
        if len(nonmatching) != 1 or len(glabels) != 1:
            ambiguous.append(
                {
                    "room": room,
                    "offset": off,
                    "name": name,
                    "size": size,
                    "nonmatching_count": len(nonmatching),
                    "glabel_count": len(glabels),
                    "file": str(asm_file.relative_to(ROOT)),
                }
            )
            continue

        func_name, declared_size = nonmatching[0]
        families[func_name] += size
        for feature in asm_features(text):
            feature_totals[feature] += size
            feature_counts[feature] += 1
        if name.startswith("room_") and name != glabels[0] and not used_names[glabels[0]]:
            historical_names.append(
                {
                    "room": room,
                    "offset": off,
                    "name": name,
                    "glabel": glabels[0],
                }
            )
        if declared_size is not None and declared_size != size:
            mismatches.append(
                {
                    "room": room,
                    "offset": off,
                    "name": name,
                    "glabel": glabels[0],
                    "yaml_size": size,
                    "declared_size": declared_size,
                }
            )

    return (
        missing,
        ambiguous,
        mismatches,
        historical_names,
        families,
        feature_totals,
        feature_counts,
    )


def typefunc_inside_data(room: str, cfg: dict[str, Any], subs: list[tuple[int, str, str]], end: int) -> list[dict[str, Any]]:
    ranges: list[tuple[int, int, str]] = []
    for i, (off, typ, name) in enumerate(subs):
        next_off = subs[i + 1][0] if i + 1 < len(subs) else end
        if typ == "data":
            ranges.append((off, next_off, name))

    sym = CONFIG_DIR / f"sym.{room}.txt"
    if not sym.exists():
        return []

    vram = as_int(cfg["segments"][0]["vram"])
    hits: list[dict[str, Any]] = []
    for line in sym.read_text(errors="ignore").splitlines():
        if "// type:func" not in line:
            continue
        m = re.match(r"([A-Za-z_]\w*)\s*=\s*0x([0-9A-Fa-f]+);", line)
        if not m:
            continue
        name = m.group(1)
        off = int(m.group(2), 16) - vram
        for start, stop, data_name in ranges:
            if start <= off < stop:
                hits.append(
                    {
                        "room": room,
                        "offset": off,
                        "symbol": name,
                        "data_start": start,
                        "data_end": stop,
                        "data_name": data_name,
                    }
                )
                break
    return hits


def base_rodata_jtbl_hits(room: str, rodata_offsets: set[int]) -> list[dict[str, Any]]:
    path = ASM_DIR / room / "data" / f"{room}.rodata.s"
    if not path.exists():
        return []

    lines = path.read_text(errors="ignore").splitlines()
    hits: list[dict[str, Any]] = []
    for i, line in enumerate(lines):
        m = re.match(r"\s*dlabel\s+(jtbl_\S+)", line)
        if not m:
            continue
        off = None
        for j in range(i + 1, min(i + 8, len(lines))):
            mm = re.search(r"/\*\s*([0-9A-Fa-f]+)\s+", lines[j])
            if mm:
                off = int(mm.group(1), 16)
                break
        if off is not None and off not in rodata_offsets:
            hits.append({"room": room, "offset": off, "label": m.group(1)})
    return hits


def exact_data_file(room: str, off: int, name: str) -> Path | None:
    data_dir = ASM_DIR / room / "data"
    if name:
        named = data_dir / f"{name}.data.s"
        if named.exists():
            return named

    by_offset = data_dir / f"{off:X}.data.s"
    if by_offset.exists():
        return by_offset

    by_offset_lower = data_dir / f"{off:x}.data.s"
    if by_offset_lower.exists():
        return by_offset_lower

    return None


def aligned_dlabels(path: Path) -> list[tuple[int, str]]:
    labels: list[tuple[int, str]] = []
    pending: str | None = None

    for line in path.read_text(errors="ignore").splitlines():
        m = re.match(r"dlabel\s+(\S+)", line)
        if m:
            pending = m.group(1)
            continue

        if pending is None:
            continue

        mm = re.search(r"/\*\s*([0-9A-Fa-f]+)\s+", line)
        if mm:
            off = int(mm.group(1), 16)
            if off % 4 == 0:
                labels.append((off, pending))
            pending = None

    return labels


def internal_data_label_candidates(
    room: str, subs: list[tuple[int, str, str]]
) -> list[dict[str, Any]]:
    subsegment_starts = {off for off, _typ, _name in subs}
    candidates: list[dict[str, Any]] = []

    for off, typ, name in subs:
        if typ != "data":
            continue

        data_file = exact_data_file(room, off, name)
        if data_file is None:
            continue

        for label_off, label in aligned_dlabels(data_file):
            if label_off in subsegment_starts:
                continue
            candidates.append(
                {
                    "room": room,
                    "offset": label_off,
                    "label": label,
                    "data_start": off,
                    "data_name": name,
                }
            )

    return candidates


def parse_psx_tim(data: bytes, pos: int) -> dict[str, Any] | None:
    if pos + 20 > len(data):
        return None
    if data[pos : pos + 4] != b"\x10\x00\x00\x00":
        return None

    flags = struct.unpack_from("<I", data, pos + 4)[0]
    bpp = flags & 0x7
    has_clut = bool(flags & 0x8)
    if bpp not in (0, 1, 2, 3):
        return None
    if flags & ~0xF:
        return None

    cursor = pos + 8
    clut: dict[str, int] | None = None
    if has_clut:
        if cursor + 12 > len(data):
            return None
        clut_size = struct.unpack_from("<I", data, cursor)[0]
        if clut_size < 12 or clut_size % 4:
            return None
        if cursor + clut_size > len(data):
            return None
        _x, _y, width, height = struct.unpack_from("<HHHH", data, cursor + 4)
        if width == 0 or height == 0 or width * height > 4096:
            return None
        clut = {"width": width, "height": height, "size": clut_size}
        cursor += clut_size

    if cursor + 12 > len(data):
        return None
    image_size = struct.unpack_from("<I", data, cursor)[0]
    if image_size < 12 or image_size % 4:
        return None
    if cursor + image_size > len(data):
        return None
    _x, _y, width, height = struct.unpack_from("<HHHH", data, cursor + 4)
    if width == 0 or height == 0:
        return None
    if width > 1024 or height > 1024 or width * height > 262144:
        return None

    size = cursor + image_size - pos
    if size < 20 or size > 262144:
        return None
    if not has_clut and width * height < 256:
        return None

    return {
        "flags": flags,
        "bpp": bpp,
        "has_clut": has_clut,
        "clut": clut,
        "width": width,
        "height": height,
        "size": size,
    }


def hidden_tim_candidates(
    room: str, subs: list[tuple[int, str, str]], end: int
) -> list[dict[str, Any]]:
    data = (ORIGINAL_DIR / f"{room}.bin").read_bytes()
    starts = {off for off, _typ, _name in subs}
    candidates: list[dict[str, Any]] = []

    for i, (off, typ, name) in enumerate(subs):
        next_off = subs[i + 1][0] if i + 1 < len(subs) else end
        if typ != "data":
            continue

        block = data[off:next_off]
        for rel in range(0, max(0, len(block) - 20), 4):
            tim = parse_psx_tim(block, rel)
            if tim is None:
                continue
            tim_off = off + rel
            if tim_off in starts:
                continue
            candidates.append(
                {
                    "room": room,
                    "offset": tim_off,
                    "data_start": off,
                    "data_name": name,
                    **tim,
                }
            )

    return candidates


def classify_data_block(size: int, name: str) -> str:
    if size >= 0x8000:
        return "large_custom_payload_or_asset_bank"
    if re.search(r"(Slot|Table|Pair|Rec|Dialog|Anchor)", name):
        return "named_room_control_table"
    if size <= 0x100:
        return "small_room_scalar_or_record"
    return "medium_room_data_record"


def audit() -> dict[str, Any]:
    totals: collections.Counter[str] = collections.Counter()
    family_totals: collections.Counter[str] = collections.Counter()
    asm_feature_totals: collections.Counter[str] = collections.Counter()
    asm_feature_counts: collections.Counter[str] = collections.Counter()
    data_class_totals: collections.Counter[str] = collections.Counter()
    data_class_counts: collections.Counter[str] = collections.Counter()
    missing: list[dict[str, Any]] = []
    ambiguous: list[dict[str, Any]] = []
    size_mismatches: list[dict[str, Any]] = []
    historical_names: list[dict[str, Any]] = []
    data_label_candidates: list[dict[str, Any]] = []
    tim_candidates: list[dict[str, Any]] = []
    typefunc_data_hits: list[dict[str, Any]] = []
    base_jtbl_hits: list[dict[str, Any]] = []
    top_data_blocks: list[tuple[int, str, int, int, str]] = []

    room_count = 0
    for cfg_path in sorted(CONFIG_DIR.glob("room_m*.yaml")):
        room_count += 1
        room = cfg_path.stem
        cfg = read_yaml(cfg_path)
        end = segment_end(room, cfg)
        subs = subsegments(cfg)

        rodata_offsets = {off for off, typ, _name in subs if typ == "rodata"}
        base_jtbl_hits.extend(base_rodata_jtbl_hits(room, rodata_offsets))
        typefunc_data_hits.extend(typefunc_inside_data(room, cfg, subs, end))
        data_label_candidates.extend(internal_data_label_candidates(room, subs))
        tim_candidates.extend(hidden_tim_candidates(room, subs, end))

        m, a, sm, hn, fam, aft, afc = exact_asm_checks(room, subs, end)
        missing.extend(m)
        ambiguous.extend(a)
        size_mismatches.extend(sm)
        historical_names.extend(hn)
        family_totals.update(fam)
        asm_feature_totals.update(aft)
        asm_feature_counts.update(afc)

        for i, (off, typ, name) in enumerate(subs):
            next_off = subs[i + 1][0] if i + 1 < len(subs) else end
            size = next_off - off
            totals[typ] += size
            if typ == "data":
                top_data_blocks.append((size, room, off, next_off, name))
                data_class = classify_data_block(size, name)
                data_class_totals[data_class] += size
                data_class_counts[data_class] += 1

    failures = {
        "missing_exact_asm_files": missing,
        "ambiguous_exact_asm_files": ambiguous,
        "asm_size_mismatches": size_mismatches,
        "historical_asm_rename_candidates": historical_names,
        "internal_aligned_data_label_candidates": data_label_candidates,
        "hidden_tim_asset_candidates": tim_candidates,
        "typefunc_symbols_inside_data": typefunc_data_hits,
        "base_rodata_jtbl_labels": base_jtbl_hits,
    }

    return {
        "room_count": room_count,
        "bytes_by_type": dict(totals),
        "data_class_totals": dict(data_class_totals),
        "data_class_counts": dict(data_class_counts),
        "asm_family_totals": family_totals.most_common(30),
        "asm_feature_totals": dict(asm_feature_totals),
        "asm_feature_counts": dict(asm_feature_counts),
        "top_data_blocks": sorted(top_data_blocks, reverse=True)[:30],
        "failures": failures,
        "ok": not any(failures.values()),
    }


def print_text(report: dict[str, Any], limit: int) -> None:
    print(f"room configs: {report['room_count']}")
    print("bytes by YAML subsegment type:")
    for typ, size in sorted(report["bytes_by_type"].items(), key=lambda x: (-x[1], x[0])):
        print(f"  {typ:8s} {size:9d}")

    print("\nbytes by room data class:")
    data_counts = report["data_class_counts"]
    for cls, size in sorted(report["data_class_totals"].items(), key=lambda x: (-x[1], x[0])):
        print(f"  {cls:34s} {size:9d}  blocks {data_counts[cls]:4d}")

    print("\nbytes by remaining asm feature:")
    asm_counts = report["asm_feature_counts"]
    for feature, size in sorted(report["asm_feature_totals"].items(), key=lambda x: (-x[1], x[0])):
        print(f"  {feature:34s} {size:9d}  blocks {asm_counts[feature]:4d}")

    print("\nconsistency checks:")
    for key, rows in report["failures"].items():
        print(f"  {key:32s} {len(rows)}")
        for row in rows[:limit]:
            print(f"    {row}")

    print("\ntop remaining asm families:")
    for name, total in report["asm_family_totals"][:limit]:
        print(f"  {total:7d}  {name}")

    print("\ntop data blocks:")
    for size, room, start, stop, name in report["top_data_blocks"][:limit]:
        suffix = f" {name}" if name else ""
        print(f"  {size:7d}  {room}  0x{start:X}-0x{stop:X}{suffix}")

    print(f"\nstatus: {'OK' if report['ok'] else 'FAILED'}")


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--json", action="store_true", help="emit machine-readable JSON")
    parser.add_argument("--limit", type=int, default=20, help="rows to print per section")
    args = parser.parse_args()

    report = audit()
    if args.json:
        print(json.dumps(report, indent=2, sort_keys=True))
    else:
        print_text(report, args.limit)
    return 0 if report["ok"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
