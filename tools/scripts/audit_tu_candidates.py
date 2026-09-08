#!/usr/bin/env python3
"""List contiguous manifest C ranges with a direct cross-segment call edge.

This is evidence discovery only: every candidate still needs a byte-identical
overlay or main build and a type/layout review before it can be merged.
"""
from __future__ import annotations

import pathlib
import re


ROOT = pathlib.Path(__file__).resolve().parents[2]
ENTRY = re.compile(r"\s*- \[(0x[0-9A-Fa-f]+), ([^,\]]+), ([^\]]+)\]")
FUNCTION = re.compile(r"\b([A-Za-z_][A-Za-z0-9_]*)\s*\(")
FUNCTION_DEFINITION = re.compile(
    r"^[ \t]*(?:static[ \t]+)?(?:[A-Za-z_]\w*[ \t]+)+"
    r"([A-Za-z_][A-Za-z0-9_]*)\s*\([^;{}]*\)\s*\{",
    re.M,
)
CONTROL_KEYWORDS = {"if", "for", "switch", "while"}


def entries(config: pathlib.Path):
    result = []
    for line in config.read_text().splitlines():
        match = ENTRY.fullmatch(line)
        if match:
            result.append((int(match.group(1), 16), match.group(2).strip(),
                           match.group(3)))
    return result


def source_for(config: pathlib.Path, name: str) -> pathlib.Path | None:
    stem = config.stem
    if "/overlays/" in config.as_posix():
        path = ROOT / "src" / "overlays" / stem / (name + ".c")
    elif config.name == "main.yaml":
        path = ROOT / "src" / "main" / (name + ".c")
    else:
        return None
    return path if path.exists() else None


def defined_functions(text: str) -> set[str]:
    return {
        match.group(1) for match in FUNCTION_DEFINITION.finditer(text)
        if match.group(1) not in CONTROL_KEYWORDS
    }


def main() -> int:
    candidates = []
    configs = [ROOT / "configs" / "USA" / "main.yaml"]
    configs.extend(sorted((ROOT / "configs" / "USA" / "overlays").glob("*.yaml")))
    for config in configs:
        ranges = entries(config)
        for (address, first_type, first), (next_address, second_type, second) in zip(ranges, ranges[1:]):
            if first_type != "c" or second_type != "c":
                continue
            left, right = source_for(config, first), source_for(config, second)
            if not left or not right:
                continue
            left_text, right_text = left.read_text(), right.read_text()
            referenced = set(FUNCTION.findall(right_text))
            shared = sorted(defined_functions(left_text) & referenced)
            if shared:
                candidates.append((config.stem, address, next_address, first, second, shared))
    for overlay, address, next_address, first, second, shared in candidates:
        print(f"{overlay} {address:#06x}-{next_address:#06x}: {first} -> {second} ({', '.join(shared)})")
    print(f"{len(candidates)} direct contiguous call candidates")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
