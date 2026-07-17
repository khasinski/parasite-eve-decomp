#!/usr/bin/env python3
"""Extract an overlay target directly from an existing splat config comment.

Some hand-maintained overlay configs describe slices that are not represented as
single catalog overlay IDs. For those, use the generated "extract locally with"
comment as the source of truth.
"""

from __future__ import annotations

import argparse
import hashlib
import re
from pathlib import Path


SHA1_RE = re.compile(r"^sha1:\s*([0-9a-fA-F]{40})\s*$", re.MULTILINE)
TARGET_RE = re.compile(r"^\s*target_path:\s*(\S+)\s*$", re.MULTILINE)
DD_RE = re.compile(
    r"dd\s+if=\S+\s+of=\S+\s+bs=(?P<bs>\d+)\s+skip=(?P<skip>\d+)\s+count=(?P<count>\d+)"
)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("config", type=Path)
    parser.add_argument("--pe-img", type=Path, default=Path("disc/extracted/PE.IMG"))
    args = parser.parse_args()

    text = args.config.read_text()
    target_match = TARGET_RE.search(text)
    sha1_match = SHA1_RE.search(text)
    dd_match = DD_RE.search(text)
    if target_match is None or sha1_match is None or dd_match is None:
        return 2

    target_path = Path(target_match.group(1))
    expected_sha1 = sha1_match.group(1).lower()
    block_size = int(dd_match.group("bs"))
    skip = int(dd_match.group("skip"))
    count = int(dd_match.group("count"))
    size = block_size * count

    with args.pe_img.open("rb") as src:
        src.seek(block_size * skip)
        data = src.read(size)
    if len(data) != size:
        raise SystemExit(f"short read: got {len(data)}, expected {size}")

    actual_sha1 = hashlib.sha1(data).hexdigest()
    if actual_sha1 != expected_sha1:
        raise SystemExit(f"sha mismatch: {actual_sha1} != {expected_sha1}")

    target_path.parent.mkdir(parents=True, exist_ok=True)
    target_path.write_bytes(data)
    print(target_path)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
