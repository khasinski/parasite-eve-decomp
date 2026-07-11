#!/usr/bin/env python3
"""Print the PE.IMG target slice used by an overlay splat config."""

from __future__ import annotations

import argparse
import re
from pathlib import Path

from catalog_pe_overlays import RENAMED_OVERLAY_IDS, build_catalog, target_slice_info


BASENAME_RE = re.compile(r"^\s*basename:\s*(\S+)\s*$")
SHA1_RE = re.compile(r"^\s*sha1:\s*([0-9a-fA-F]{40})\s*$")


def config_basename_and_sha1(path: Path) -> tuple[str, str]:
    basename: str | None = None
    sha1: str | None = None
    for line in path.read_text().splitlines():
        if basename is None and (match := BASENAME_RE.match(line)):
            basename = match.group(1)
        if sha1 is None and (match := SHA1_RE.match(line)):
            sha1 = match.group(1).lower()
        if basename is not None and sha1 is not None:
            break
    if basename is None:
        raise SystemExit(f"{path}: missing options.basename")
    if sha1 is None:
        raise SystemExit(f"{path}: missing sha1")
    return basename, sha1


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("overlay_config", type=Path)
    parser.add_argument("--pe-img", type=Path, default=Path("disc/extracted/PE.IMG"))
    parser.add_argument("--main-exe", type=Path, default=Path("assets/USA/main.exe"))
    parser.add_argument("--sym-file", type=Path, default=Path("configs/USA/sym.main.txt"))
    parser.add_argument("--config-yaml", type=Path, default=Path("configs/USA/main.yaml"))
    parser.add_argument("--fallback", default="scanner")
    args = parser.parse_args()

    basename, config_sha1 = config_basename_and_sha1(args.overlay_config)
    if basename == "fx_common":
        basename = "fx_block_1792"
    elif basename in RENAMED_OVERLAY_IDS:
        basename = f"ovl_{RENAMED_OVERLAY_IDS[basename]:03d}"
    _image_sectors, candidates = build_catalog(
        args.pe_img,
        args.main_exe,
        args.sym_file,
        args.config_yaml,
    )
    candidate = next(
        (candidate for candidate in candidates if candidate.overlay_id == basename),
        None,
    )
    if candidate is None:
        raise SystemExit(f"{args.overlay_config}: unknown overlay basename {basename}")

    for target_slice in ("scanner", "room-section3"):
        try:
            _sector, _count, _size, _sha12, target_sha1 = target_slice_info(
                candidate,
                target_slice,
            )
        except ValueError:
            continue
        if target_sha1 == config_sha1:
            print(target_slice)
            return 0

    print(args.fallback)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
