#!/usr/bin/env python3
"""Generate docs/progress.html and docs/badges/*.json from the progress metric
plus objdiff's report.json when present.

Badges are shields.io endpoint JSONs, usable straight from
raw.githubusercontent.com.
"""
from __future__ import annotations

import html
import json
import pathlib
import re
import sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "tools/scripts"))
from progress_report import configured_non_room_overlays, count_funcs, is_clean  # noqa: E402

DOCS = ROOT / "docs"
BADGES = DOCS / "badges"


def color(pct: float) -> str:
    return ("brightgreen" if pct >= 90 else "green" if pct >= 70
            else "yellowgreen" if pct >= 50 else "yellow" if pct >= 30
            else "orange" if pct >= 10 else "red")


def unit_rows(src_dir: pathlib.Path, prefix: str):
    rows = []
    for f in sorted(src_dir.rglob("*.c")) if src_dir.exists() else []:
        t = f.read_text(errors="ignore")
        clean = is_clean(t)
        rows.append((str(f.relative_to(ROOT)), count_funcs(t), clean))
    return rows


def main() -> None:
    binaries = [("SLUS_006.62 (main)", ROOT / "src/main")]
    for ovl_yaml in configured_non_room_overlays():
        binaries.append((ovl_yaml.stem, ROOT / "src/overlays" / ovl_yaml.stem))

    # headline numbers from the generated PROGRESS.md (single source of truth)
    prog = (DOCS / "PROGRESS.md").read_text()
    total = re.search(
        r"\| \*\*total\*\* \| \*\*(\d+)/(\d+)\*\* \| \*\*([\d.]+)%\*\* "
        r"\| \*\*(\d+)/(\d+)\*\* \| \*\*([\d.]+)%\*\* \|", prog)
    mf, nf, fpct, mb, tb, bpct = total.groups()

    BADGES.mkdir(parents=True, exist_ok=True)
    for name, label, msg, pct in [
        ("functions", "functions decompiled", f"{mf}/{nf} ({fpct}%)", float(fpct)),
        ("code", "code bytes decompiled", f"{bpct}%", float(bpct)),
    ]:
        (BADGES / f"{name}.json").write_text(json.dumps({
            "schemaVersion": 1, "label": label,
            "message": msg, "color": color(pct),
        }) + "\n")

    parts = [
        "<!doctype html><meta charset='utf-8'>",
        "<title>parasite-eve-decomp progress</title>",
        "<style>body{font:14px/1.5 -apple-system,sans-serif;max-width:60rem;"
        "margin:2rem auto;padding:0 1rem}table{border-collapse:collapse;width:100%}"
        "td,th{border:1px solid #ccc;padding:.25rem .5rem;text-align:left}"
        ".ok{color:#1a7f37}.no{color:#b35900}h2{margin-top:2rem}</style>",
        "<h1>parasite-eve-decomp</h1>",
        f"<p>Decompiled functions: <b>{mf}/{nf} ({fpct}%)</b> · "
        f"code bytes: <b>{bpct}%</b>. A unit counts when it has no INCLUDE_ASM "
        "and no inline/register assembly. Pure C is the target. "
        "The full build is byte-identical "
        "to retail.</p>",
    ]
    md_table = re.search(r"\| Binary \|.*?\n((?:\|.*\n)+)", prog)
    parts.append("<h2>Per binary</h2><table><tr><th>Binary</th>"
                 "<th>Functions</th><th>%</th><th>Code bytes</th><th>%</th></tr>")
    for line in md_table.group(1).strip().split("\n")[1:]:
        cells = [c.strip().strip("*").strip("`") for c in line.strip("|").split("|")]
        parts.append("<tr>" + "".join(f"<td>{html.escape(c)}</td>" for c in cells) + "</tr>")
    parts.append("</table>")

    for name, src_dir in binaries:
        rows = unit_rows(src_dir, name)
        if not rows:
            continue
        done = sum(1 for _, _, c in rows if c)
        parts.append(f"<h2>{html.escape(name)} — {done}/{len(rows)} decompiled files</h2>")
        parts.append("<table><tr><th>Translation unit</th><th>Functions</th>"
                     "<th>Status</th></tr>")
        for path, funcs, clean in rows:
            status = "<span class=ok>decompiled</span>" if clean else \
                     "<span class=no>not pure C</span>"
            parts.append(f"<tr><td>{html.escape(path)}</td><td>{funcs}</td>"
                         f"<td>{status}</td></tr>")
        parts.append("</table>")

    (DOCS / "progress.html").write_text("\n".join(parts) + "\n")
    print(f"wrote {DOCS/'progress.html'} and {BADGES}/*.json")


if __name__ == "__main__":
    main()
