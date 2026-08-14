#!/usr/bin/env python3
"""Render the progress badges from the objdiff report.

The one source of truth for progress is build/USA/report.json - the same
file CI publishes to decomp.dev - so the badges can never disagree with the
site. Run `make report` first; `make progress` refuses to invent numbers
without it. The crutch-debt badge is written by crutch_debt.py, which owns
that ledger.
"""

import json
import pathlib
import sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
REPORT = ROOT / "build" / "USA" / "report.json"
BADGES = ROOT / "docs" / "badges"


def color(percent):
    if percent >= 100:
        return "brightgreen"
    if percent >= 90:
        return "green"
    if percent >= 60:
        return "yellow"
    if percent >= 30:
        return "orange"
    return "red"


def badge(label, message, percent):
    return json.dumps(
        {"schemaVersion": 1, "label": label, "message": message,
         "color": color(percent)}
    ) + "\n"


def main():
    if not REPORT.exists():
        raise SystemExit("%s missing - run `make report` first" % REPORT)
    report = json.loads(REPORT.read_text())

    measures = report["measures"]
    matched = int(measures.get("matched_functions", 0))
    total = int(measures.get("total_functions", 0))
    fn_percent = 100.0 * matched / total if total else 0.0
    code_percent = measures.get("matched_code_percent", 0.0)

    BADGES.mkdir(parents=True, exist_ok=True)
    (BADGES / "functions.json").write_text(
        badge("functions matched", "%d/%d (%.1f%%)" % (matched, total, fn_percent),
              fn_percent)
    )
    (BADGES / "code.json").write_text(
        badge("code matched", "%.2f%%" % code_percent, code_percent)
    )

    print("%-12s %9s %8s %8s" % ("category", "functions", "code", "data"))
    for category in report.get("categories", []):
        m = category["measures"]
        print("%-12s %4s/%-4s %7.2f%% %7.2f%%" % (
            category["id"],
            m.get("matched_functions", 0), m.get("total_functions", 0),
            m.get("matched_code_percent", 0.0),
            m.get("matched_data_percent", 0.0),
        ))
    print("%-12s %4d/%-4d %7.2f%% %7.2f%%" % (
        "total", matched, total, code_percent,
        measures.get("matched_data_percent", 0.0)))
    return 0


if __name__ == "__main__":
    sys.exit(main())
