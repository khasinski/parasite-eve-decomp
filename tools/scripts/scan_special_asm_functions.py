#!/usr/bin/env python3
"""Scan generated asm for functions that should be asm-backed C islands.

This intentionally only inspects `glabel func_*` bodies. Data labels and jump
tables can contain raw words that look like instructions, but they are not C
function debt.
"""

from __future__ import annotations

import argparse
import re
from collections import Counter
from dataclasses import dataclass
from pathlib import Path


GLABEL_RE = re.compile(r"^\s*glabel\s+([A-Za-z_.$][A-Za-z0-9_.$]*)\b")
ENDLABEL_RE = re.compile(r"^\s*endlabel\s+([A-Za-z_.$][A-Za-z0-9_.$]*)\b")
INSN_RE = re.compile(
    r"/\*\s+([0-9A-Fa-f]+)\s+([0-9A-Fa-f]+)\s+([0-9A-Fa-f]{8})\s+\*/\s+"
    r"([A-Za-z0-9_.]+)\s*(.*)$"
)

SPECIAL_PATTERNS: dict[str, re.Pattern[str]] = {
    "raw_invalid_word": re.compile(r"\.word\b.*invalid instruction"),
    "raw_word_in_func": re.compile(r"/\*.*\*/\s+\.word\b"),
    "cop0": re.compile(r"\b(?:mfc0|mtc0|cfc0|ctc0)\b"),
    "cop2_raw": re.compile(r"\b(?:mfc2|mtc2|cfc2|ctc2|cop2)\b"),
    "bios_t2_thunk": re.compile(
        r"\b(?:addiu|li)\s+\$t2,\s*(?:\$zero,\s*)?0xA0\b|\bjr\s+\$t2\b"
    ),
    "syscall": re.compile(r"\bsyscall\b"),
}


@dataclass
class Function:
    name: str
    path: Path
    start_offset: int | None
    start_vram: int | None
    lines: list[str]

    @property
    def text(self) -> str:
        return "\n".join(self.lines)

    @property
    def categories(self) -> list[str]:
        return [name for name, pattern in SPECIAL_PATTERNS.items() if pattern.search(self.text)]


def parse_functions(path: Path) -> list[Function]:
    funcs: list[Function] = []
    current_name: str | None = None
    current_lines: list[str] = []

    def flush() -> None:
        nonlocal current_name, current_lines
        if current_name is None or not current_name.startswith("func_"):
            current_name = None
            current_lines = []
            return

        start_offset: int | None = None
        start_vram: int | None = None
        for line in current_lines:
            match = INSN_RE.search(line)
            if match:
                start_offset = int(match.group(1), 16)
                start_vram = int(match.group(2), 16)
                break
        funcs.append(Function(current_name, path, start_offset, start_vram, current_lines))
        current_name = None
        current_lines = []

    for line in path.read_text(errors="replace").splitlines():
        match = GLABEL_RE.match(line)
        if match:
            flush()
            current_name = match.group(1)
            current_lines = []
            continue

        if current_name is None:
            continue

        end_match = ENDLABEL_RE.match(line)
        if end_match and end_match.group(1) == current_name:
            flush()
            continue

        current_lines.append(line)

    flush()
    return funcs


def iter_asm_files(paths: list[Path]) -> list[Path]:
    files: list[Path] = []
    for path in paths:
        if path.is_dir():
            files.extend(path.rglob("*.s"))
        elif path.suffix == ".s":
            files.append(path)
    return sorted(set(files))


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "paths",
        nargs="*",
        type=Path,
        default=[Path("asm/USA/main/main")],
        help="asm files or directories to scan",
    )
    parser.add_argument("--fail-on-special", action="store_true")
    args = parser.parse_args()

    funcs: list[Function] = []
    for path in iter_asm_files(args.paths):
        funcs.extend(parse_functions(path))

    special = [func for func in funcs if func.categories]
    category_counts = Counter(category for func in special for category in func.categories)

    print(f"asm_function_count\t{len(funcs)}")
    print(f"special_function_count\t{len(special)}")
    for category, count in sorted(category_counts.items()):
        print(f"category\t{category}\t{count}")

    if special:
        print("function\tvram\tcategories\tfile")
        for func in special:
            vram = f"0x{func.start_vram:08X}" if func.start_vram is not None else ""
            print(f"{func.name}\t{vram}\t{','.join(func.categories)}\t{func.path}")

    if args.fail_on_special and special:
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
