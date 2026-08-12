#!/usr/bin/env python3
"""Compare compiled C segment text with its exact retail YAML slice.

Relocation payloads differ before linking, so only the relocated bit fields are
ignored. Instruction opcodes/registers, local control flow, and segment sizes
still have to match.
"""

from __future__ import annotations

import argparse
import re
import struct
import subprocess
import tempfile
from dataclasses import dataclass
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
SUBSEGMENT_RE = re.compile(
    r"^\s*-\s*\[(0x[0-9A-Fa-f]+),\s*([^,\]]+),\s*([^\]]+)\]"
)
RELOC_RE = re.compile(r"^([0-9A-Fa-f]+)\s+R_MIPS_([A-Z0-9_]+)\s+")


@dataclass(frozen=True)
class Segment:
    offset: int
    end: int
    source: Path
    target: Path


def yaml_value(text: str, key: str) -> str:
    match = re.search(rf"^\s*{re.escape(key)}:\s*(\S+)\s*$", text, re.MULTILINE)
    if match is None:
        raise ValueError(f"missing {key}")
    return match.group(1)


def config_segments(config: Path) -> list[Segment]:
    text = config.read_text()
    base = (config.parent / yaml_value(text, "base_path")).resolve()
    target = (base / yaml_value(text, "target_path")).resolve()
    src_root = (base / yaml_value(text, "src_path")).resolve()
    rows: list[tuple[int, str, str]] = []
    for line in text.splitlines():
        match = SUBSEGMENT_RE.match(line)
        if match:
            rows.append((int(match.group(1), 16), match.group(2).strip(), match.group(3).strip()))

    result: list[Segment] = []
    for index, (offset, kind, name) in enumerate(rows):
        if kind != "c":
            continue
        end = rows[index + 1][0] if index + 1 < len(rows) else target.stat().st_size
        result.append(Segment(offset, end, src_root / f"{name}.c", target))
    return result


def all_segments() -> dict[Path, Segment]:
    configs = [ROOT / "configs/USA/main.yaml"]
    configs.extend(sorted((ROOT / "configs/USA/overlays").glob("*.yaml")))
    result: dict[Path, Segment] = {}
    for config in configs:
        for segment in config_segments(config):
            result[segment.source.resolve()] = segment
    return result


def run(args: list[str], **kwargs) -> subprocess.CompletedProcess:
    return subprocess.run(args, cwd=ROOT, check=True, **kwargs)


def section_bytes(obj: Path, out: Path) -> bytes:
    run(["mipsel-none-elf-objcopy", "-O", "binary", "-j", ".text", str(obj), str(out)])
    return out.read_bytes()


def relocations(obj: Path) -> dict[int, str]:
    output = run(
        ["mipsel-none-elf-objdump", "-r", str(obj)],
        text=True,
        stdout=subprocess.PIPE,
    ).stdout
    result: dict[int, str] = {}
    in_text = False
    for line in output.splitlines():
        if line.startswith("RELOCATION RECORDS FOR ["):
            in_text = line == "RELOCATION RECORDS FOR [.text]:"
            continue
        if not in_text:
            continue
        match = RELOC_RE.match(line.strip())
        if match:
            result[int(match.group(1), 16)] = match.group(2)
    return result


def relocation_mask(kind: str) -> int:
    if kind == "26":
        return 0xFC000000
    if kind in {"HI16", "LO16", "GPREL16", "GOT16", "CALL16"}:
        return 0xFFFF0000
    return 0xFFFFFFFF


def compare(segment: Segment, obj: Path, text_path: Path) -> tuple[int, int, int, list[int]]:
    actual = section_bytes(obj, text_path)
    expected = segment.target.read_bytes()[segment.offset : segment.end]
    relocs = relocations(obj)
    word_count = min(len(actual), len(expected)) // 4
    mismatches: list[int] = []
    for index in range(word_count):
        offset = index * 4
        left = struct.unpack_from("<I", actual, offset)[0]
        right = struct.unpack_from("<I", expected, offset)[0]
        mask = relocation_mask(relocs.get(offset, ""))
        if left & mask != right & mask:
            mismatches.append(offset)
    if len(actual) != len(expected):
        mismatches.extend(range(word_count * 4, max(len(actual), len(expected)), 4))
    return len(actual), len(expected), len(mismatches), mismatches[:8]


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("sources", nargs="+", type=Path)
    args = parser.parse_args()
    segments = all_segments()
    failures = 0
    with tempfile.TemporaryDirectory(prefix="pe1-c-segment-audit-") as tmp_name:
        tmp = Path(tmp_name)
        for index, source_arg in enumerate(args.sources):
            source = (ROOT / source_arg).resolve() if not source_arg.is_absolute() else source_arg.resolve()
            segment = segments.get(source)
            if segment is None:
                print(f"NO_SEGMENT {source_arg}")
                failures += 1
                continue
            obj = tmp / f"{index}.o"
            try:
                run(
                    [str(ROOT / "tools/scripts/cc.sh"), str(source), str(obj)],
                    stdout=subprocess.DEVNULL,
                    stderr=subprocess.DEVNULL,
                )
                actual, expected, count, first = compare(segment, obj, tmp / f"{index}.text")
            except subprocess.CalledProcessError:
                print(f"BUILD_FAIL {source_arg}")
                failures += 1
                continue
            status = "MATCH" if count == 0 else "DIFF"
            details = "" if not first else " first=" + ",".join(f"0x{x:X}" for x in first)
            print(
                f"{status} {source_arg} actual=0x{actual:X} expected=0x{expected:X} "
                f"mismatched_words={count}{details}"
            )
            failures += count != 0
    return 1 if failures else 0


if __name__ == "__main__":
    raise SystemExit(main())
