#!/usr/bin/env python3
"""Reject objdiff reports that credit code outside verified semantic C."""
from __future__ import annotations

import json
import pathlib
import sys


ROOT = pathlib.Path(__file__).resolve().parents[2]


def number(measures, name):
    return int(measures.get(name, 0))


def audit(config, report):
    errors = []
    config_units = {unit["name"]: unit for unit in config["units"]}
    report_units = {unit["name"]: unit for unit in report["units"]}
    if len(config_units) != len(config["units"]):
        errors.append("objdiff config contains duplicate unit names")
    if len(report_units) != len(report["units"]):
        errors.append("objdiff report contains duplicate unit names")
    missing = sorted(config_units.keys() - report_units.keys())
    extra = sorted(report_units.keys() - config_units.keys())
    if missing:
        errors.append("report omitted %d configured units" % len(missing))
    if extra:
        errors.append("report added %d unknown units" % len(extra))

    semantic_code = 0
    semantic_functions = 0
    for name in config_units.keys() & report_units.keys():
        configured = config_units[name]
        measured = report_units[name]["measures"]
        kind = configured.get("metadata", {}).get("source_kind")
        matched_code = number(measured, "matched_code")
        matched_functions = number(measured, "matched_functions")
        if kind == "semantic_c":
            semantic_code += matched_code
            semantic_functions += matched_functions
        elif matched_code or matched_functions:
            errors.append(
                "%s: %s received %d code bytes and %d functions" %
                (name, kind or "non-source", matched_code, matched_functions)
            )

    totals = report["measures"]
    if number(totals, "matched_code") != semantic_code:
        errors.append("report matched-code total does not equal semantic-C sum")
    if number(totals, "matched_functions") != semantic_functions:
        errors.append("report matched-function total does not equal semantic-C sum")
    return errors, semantic_code, semantic_functions


def main():
    config = json.loads((ROOT / "objdiff.json").read_text())
    report = json.loads((ROOT / "build/USA/report.json").read_text())
    errors, code, functions = audit(config, report)
    if errors:
        print("Progress report audit failed:")
        for error in errors[:100]:
            print("  " + error)
        if len(errors) > 100:
            print("  ... and %d more" % (len(errors) - 100))
        return 1
    print("OK: report credits only semantic C (%d bytes, %d functions)." %
          (code, functions))
    return 0


if __name__ == "__main__":
    sys.exit(main())
