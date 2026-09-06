#!/usr/bin/env python3
"""Audit objdiff inputs so progress cannot exceed verified semantic C."""
from __future__ import annotations

import json
import pathlib
import sys

from elftools.elf.elffile import ELFFile


ROOT = pathlib.Path(__file__).resolve().parents[2]


def functions(path):
    with path.open("rb") as handle:
        elf = ELFFile(handle)
        table = elf.get_section_by_name(".symtab")
        if table is None:
            return set()
        return {
            (symbol["st_value"], symbol["st_size"])
            for symbol in table.iter_symbols()
            if symbol["st_info"]["type"] == "STT_FUNC"
            and symbol["st_shndx"] != "SHN_UNDEF"
        }


def main() -> int:
    config = json.loads((ROOT / "objdiff.json").read_text())
    errors = []
    semantic_units = 0
    semantic_functions = 0
    for unit in config["units"]:
        metadata = unit.get("metadata", {})
        kind = metadata.get("source_kind")
        base = unit.get("base_path")
        complete = metadata.get("complete")
        if complete and kind != "semantic_c":
            errors.append("%s: %s received objdiff complete override" %
                          (unit["name"], kind))
            continue
        if kind not in (None, "semantic_c", "data") and base is not None:
            errors.append("%s: %s received semantic progress metadata" %
                          (unit["name"], kind))
            continue
        if kind == "text_data":
            target_functions = functions(ROOT / unit["target_path"])
            if target_functions:
                errors.append("%s: text data has %d target functions" %
                              (unit["name"], len(target_functions)))
        if base is None:
            continue
        # Generated data objects intentionally have a base so objdiff can
        # measure data bytes. They are not source and cannot add code credit.
        if kind in (None, "data") and "source_path" not in metadata:
            if functions(ROOT / unit["target_path"]):
                errors.append("%s: generated data unit contains target functions" %
                              unit["name"])
            continue
        if kind != "semantic_c":
            errors.append("%s: base assigned to %s" % (unit["name"], kind))
            continue
        if complete is not True:
            errors.append("%s: base assigned without verified module SHA" % unit["name"])
            continue
        base_functions = functions(ROOT / base)
        target_functions = functions(ROOT / unit["target_path"])
        semantic_units += 1
        semantic_functions += len(target_functions)
        if base_functions != target_functions:
            errors.append(
                "%s: function boundaries differ (base %d, target %d)" %
                (unit["name"], len(base_functions), len(target_functions))
            )

    if errors:
        print("Progress input audit failed:")
        for error in errors[:100]:
            print("  " + error)
        if len(errors) > 100:
            print("  ... and %d more" % (len(errors) - 100))
        return 1
    print("OK: %d semantic-C units, %d consistent function boundaries."
          % (semantic_units, semantic_functions))
    return 0


if __name__ == "__main__":
    sys.exit(main())
