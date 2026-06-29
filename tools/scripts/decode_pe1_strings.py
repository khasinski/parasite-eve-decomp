#!/usr/bin/env python3
"""Decode PE1 custom-encoded text streams from a binary blob.

PE1's game text uses a compact glyph encoding:

    byte 0x00..0x4D -> ASCII byte + 0x31
    byte 0x0F       -> space
    byte 0xFF       -> terminator

High bytes are not fully mapped yet, so this tool renders them as
``<XX>`` tokens and keeps the scan conservative enough for research output.
"""
from __future__ import annotations

import argparse
import csv
import sys
from dataclasses import dataclass
from pathlib import Path


TERMINATOR = 0xFF
SPACE = 0x0F
PRINTABLE_MAX = 0x4D


@dataclass(frozen=True)
class DecodedString:
    offset: int
    length: int
    text: str
    raw: bytes
    printable_count: int
    alpha_count: int


def decode_bytes(raw: bytes) -> tuple[str, int, int]:
    """Decode bytes up to, but not including, the terminator."""
    out: list[str] = []
    printable_count = 0
    alpha_count = 0

    for byte in raw:
        if byte == SPACE:
            ch = " "
            printable_count += 1
        elif byte <= PRINTABLE_MAX:
            ch = chr(byte + 0x31)
            printable_count += 1
        else:
            out.append(f"<{byte:02X}>")
            continue

        out.append(ch)
        if ch.isalpha():
            alpha_count += 1

    return "".join(out), printable_count, alpha_count


def decode_at(data: bytes, offset: int, max_length: int) -> DecodedString | None:
    end_limit = min(len(data), offset + max_length + 1)
    end = data.find(bytes([TERMINATOR]), offset, end_limit)
    if end < 0:
        return None

    raw = data[offset:end]
    if not raw:
        return None

    text, printable_count, alpha_count = decode_bytes(raw)
    return DecodedString(
        offset=offset,
        length=len(raw) + 1,
        text=text,
        raw=data[offset : end + 1],
        printable_count=printable_count,
        alpha_count=alpha_count,
    )


def is_candidate(
    decoded: DecodedString,
    *,
    min_chars: int,
    require_alpha: bool,
    min_printable_ratio: float,
) -> bool:
    first = decoded.raw[0]
    if first == 0x00 or first > PRINTABLE_MAX:
        return False
    if decoded.printable_count < min_chars:
        return False
    if require_alpha and decoded.alpha_count == 0:
        return False
    return decoded.printable_count / max(1, decoded.length - 1) >= min_printable_ratio


def scan_strings(
    data: bytes,
    *,
    min_chars: int = 4,
    max_length: int = 256,
    require_alpha: bool = True,
    min_printable_ratio: float = 0.70,
    include_all: bool = False,
    scan_mode: str = "all-offsets",
    base_offset: int = 0,
) -> list[DecodedString]:
    if scan_mode == "terminated-runs":
        return scan_terminated_runs(
            data,
            base_offset=base_offset,
            min_chars=min_chars,
            max_length=max_length,
            require_alpha=require_alpha,
            min_printable_ratio=min_printable_ratio,
            include_all=include_all,
        )
    if scan_mode != "all-offsets":
        raise ValueError(f"unknown scan mode: {scan_mode}")

    found: list[DecodedString] = []
    offset = 0
    while offset < len(data):
        decoded = decode_at(data, offset, max_length)
        if decoded is None:
            offset += 1
            continue
        if include_all or is_candidate(
            decoded,
            min_chars=min_chars,
            require_alpha=require_alpha,
            min_printable_ratio=min_printable_ratio,
        ):
            found.append(
                DecodedString(
                    offset=base_offset + decoded.offset,
                    length=decoded.length,
                    text=decoded.text,
                    raw=decoded.raw,
                    printable_count=decoded.printable_count,
                    alpha_count=decoded.alpha_count,
                )
            )
            offset += decoded.length
        else:
            offset += 1
    return found


def scan_terminated_runs(
    data: bytes,
    *,
    base_offset: int = 0,
    min_chars: int,
    max_length: int,
    require_alpha: bool,
    min_printable_ratio: float,
    include_all: bool,
) -> list[DecodedString]:
    found: list[DecodedString] = []
    start = 0
    while start < len(data):
        end = data.find(bytes([TERMINATOR]), start)
        if end < 0:
            break

        candidate_start = start
        while candidate_start < end and data[candidate_start] == 0x00:
            candidate_start += 1

        length = end - candidate_start
        if 0 < length <= max_length:
            raw = data[candidate_start:end]
            text, printable_count, alpha_count = decode_bytes(raw)
            decoded = DecodedString(
                offset=base_offset + candidate_start,
                length=length + 1,
                text=text,
                raw=data[candidate_start : end + 1],
                printable_count=printable_count,
                alpha_count=alpha_count,
            )
            if include_all or is_candidate(
                decoded,
                min_chars=min_chars,
                require_alpha=require_alpha,
                min_printable_ratio=min_printable_ratio,
            ):
                found.append(decoded)

        start = end + 1
    return found


def parse_int(value: str) -> int:
    return int(value, 0)


def build_arg_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("binary", type=Path, help="Binary file to scan")
    parser.add_argument(
        "--base-vram",
        type=parse_int,
        help="Optional base address added to file offsets for the vram column",
    )
    parser.add_argument("--min-chars", type=int, default=4)
    parser.add_argument("--max-length", type=int, default=256)
    parser.add_argument(
        "--start",
        type=parse_int,
        default=0,
        help="Start file offset to scan, inclusive",
    )
    parser.add_argument(
        "--end",
        type=parse_int,
        help="End file offset to scan, exclusive",
    )
    parser.add_argument(
        "--min-printable-ratio",
        type=float,
        default=0.70,
        help="Minimum printable glyph ratio for default candidate filtering",
    )
    parser.add_argument(
        "--no-require-alpha",
        action="store_true",
        help="Do not require at least one ASCII letter in default filtering",
    )
    parser.add_argument(
        "--all",
        action="store_true",
        help="Emit every terminated run under --max-length",
    )
    parser.add_argument(
        "--scan-mode",
        choices=("terminated-runs", "all-offsets"),
        default="terminated-runs",
        help="terminated-runs is fast for PE.IMG; all-offsets is exhaustive",
    )
    parser.add_argument("--limit", type=int, help="Maximum rows to emit")
    return parser


def main(argv: list[str] | None = None) -> int:
    args = build_arg_parser().parse_args(argv)
    data = args.binary.read_bytes()
    if args.start < 0:
        raise ValueError("--start must be non-negative")
    end = len(data) if args.end is None else args.end
    if end < args.start:
        raise ValueError("--end must be greater than or equal to --start")
    window = data[args.start : min(end, len(data))]
    rows = scan_strings(
        window,
        min_chars=args.min_chars,
        max_length=args.max_length,
        require_alpha=not args.no_require_alpha,
        min_printable_ratio=args.min_printable_ratio,
        include_all=args.all,
        scan_mode=args.scan_mode,
        base_offset=args.start,
    )
    if args.limit is not None:
        rows = rows[: args.limit]

    fieldnames = ["offset"]
    if args.base_vram is not None:
        fieldnames.append("vram")
    fieldnames.extend(["length", "text", "hex"])

    writer = csv.DictWriter(sys.stdout, fieldnames=fieldnames)
    writer.writeheader()
    for row in rows:
        out: dict[str, str | int] = {
            "offset": f"0x{row.offset:06X}",
            "length": row.length,
            "text": row.text,
            "hex": row.raw.hex(" ").upper(),
        }
        if args.base_vram is not None:
            out["vram"] = f"0x{args.base_vram + row.offset:08X}"
        writer.writerow(out)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
