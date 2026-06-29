#!/usr/bin/env python3
"""Apply proposed function names from docs/function_names.md to the codebase.

Operations per function:
  1. git mv src/main/MODULE/func_8XXXXXX.c  src/main/MODULE/NewName.c
  2. Replace func_8XXXXXX -> NewName in ALL .c files (definitions + callers)
  3. Replace path in configs/USA/main.yaml
Then: make split
"""
import os
import re
import subprocess
import sys
from concurrent.futures import ThreadPoolExecutor, as_completed
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent
DOC  = ROOT / "docs" / "function_names.md"
SRC  = ROOT / "src" / "main"
YAML = ROOT / "configs" / "USA" / "main.yaml"

# ── Parse naming document ────────────────────────────────────────────────────

def parse_proposals():
    proposals = []
    current_module = None
    row_re = re.compile(r'^\| (0x[0-9A-Fa-f]+) \| (\S+) \| (high|medium|low) \|')
    sec_re = re.compile(r'^## (\w+)$')
    with open(DOC) as f:
        for line in f:
            m = sec_re.match(line.rstrip())
            if m:
                current_module = m.group(1)
                continue
            m = row_re.match(line)
            if m and current_module and current_module != "Summary":
                addr_hex = m.group(1)[2:].upper()   # strip 0x, uppercase → 8001ABCD
                name     = m.group(2)
                proposals.append({
                    "module":    current_module,
                    "addr_hex":  addr_hex,
                    "old_name":  f"func_{addr_hex}",
                    "new_name":  name,
                    "old_file":  SRC / current_module / f"func_{addr_hex}.c",
                    "new_file":  SRC / current_module / f"{name}.c",
                    "old_yaml":  f"{current_module}/func_{addr_hex}",
                    "new_yaml":  f"{current_module}/{name}",
                })
    return proposals

# ── Validation ───────────────────────────────────────────────────────────────

def validate(proposals):
    """Mark conflicting proposals as skipped; return list of conflict messages."""
    conflicts = []
    seen_new = {}
    for p in proposals:
        p["skip"] = False
        if not p["old_file"].exists():
            continue
        if p["new_file"].exists() and p["new_file"] != p["old_file"]:
            msg = f"SKIP (target exists): {p['old_name']} → {p['new_name']} in {p['module']}"
            conflicts.append(msg)
            p["skip"] = True
            continue
        key = str(p["new_file"])
        if key in seen_new:
            msg = f"SKIP (duplicate target): {p['old_name']} → {p['new_name']} clashes with {seen_new[key]}"
            conflicts.append(msg)
            p["skip"] = True
            continue
        seen_new[key] = p["old_name"]
    return conflicts

# ── git mv ───────────────────────────────────────────────────────────────────

def do_git_mv(proposals):
    to_rename = [p for p in proposals if p["old_file"].exists() and not p.get("skip")]
    print(f"git mv: {len(to_rename)} files to rename …")
    errors = []
    for p in to_rename:
        r = subprocess.run(
            ["git", "mv",
             str(p["old_file"].relative_to(ROOT)),
             str(p["new_file"].relative_to(ROOT))],
            cwd=ROOT, capture_output=True, text=True
        )
        if r.returncode != 0:
            errors.append(f"git mv FAILED: {p['old_name']} → {p['new_name']}: {r.stderr.strip()}")
    if errors:
        for e in errors:
            print(f"  ERROR: {e}", file=sys.stderr)
    print(f"  done ({len(to_rename) - len(errors)} renamed, {len(errors)} errors)")
    return errors

# ── Replace identifiers in source files ─────────────────────────────────────

def build_substitution_re(proposals):
    """Return a compiled regex + replacement dict for all old→new name swaps."""
    # Only include proposals where the old file DID exist (we actually renamed it)
    mapping = {}
    for p in proposals:
        # Include even if old_file is gone now (we just moved it)
        mapping[p["old_name"]] = p["new_name"]
    # Sort by length descending to avoid partial matches (func_80012345 before func_8001234)
    names_sorted = sorted(mapping.keys(), key=len, reverse=True)
    pattern = re.compile(r'\b(' + '|'.join(re.escape(n) for n in names_sorted) + r')\b')
    return pattern, mapping

def replace_in_file(path, pattern, mapping):
    try:
        text = path.read_text(encoding="utf-8", errors="replace")
    except Exception:
        return False
    new_text, n = pattern.subn(lambda m: mapping.get(m.group(1), m.group(1)), text)
    if n == 0:
        return False
    path.write_text(new_text, encoding="utf-8")
    return True

def replace_in_all_sources(proposals):
    pattern, mapping = build_substitution_re(proposals)
    c_files = list(SRC.rglob("*.c"))
    print(f"Replacing identifiers in {len(c_files)} source files …")
    changed = 0
    with ThreadPoolExecutor(max_workers=16) as ex:
        futs = {ex.submit(replace_in_file, f, pattern, mapping): f for f in c_files}
        for fut in as_completed(futs):
            if fut.result():
                changed += 1
    print(f"  {changed} files updated")

# ── Update yaml ──────────────────────────────────────────────────────────────

def update_yaml(proposals):
    print("Updating configs/USA/main.yaml …")
    text = YAML.read_text()
    count = 0
    for p in proposals:
        if p["old_yaml"] in text:
            text = text.replace(p["old_yaml"], p["new_yaml"])
            count += 1
    YAML.write_text(text)
    print(f"  {count} paths updated in yaml")

# ── make split ───────────────────────────────────────────────────────────────

def run_make_split():
    print("Running make split …")
    r = subprocess.run(["make", "split"], cwd=ROOT, capture_output=True, text=True)
    if r.returncode != 0:
        print("make split FAILED:", file=sys.stderr)
        print(r.stderr[-2000:], file=sys.stderr)
    else:
        print("  make split OK")
    return r.returncode == 0

# ── Main ─────────────────────────────────────────────────────────────────────

def main():
    proposals = parse_proposals()
    print(f"Parsed {len(proposals)} proposals from naming doc")

    conflicts = validate(proposals)
    if conflicts:
        print(f"{len(conflicts)} proposals skipped due to name conflicts:")
        for c in conflicts:
            print(f"  {c}")

    active = [p for p in proposals if p["old_file"].exists() and not p.get("skip")]
    print(f"  {len(active)} files exist and will be renamed")
    print(f"  {len(proposals) - len(active)} already renamed or not found (skipped)")

    do_git_mv(proposals)
    replace_in_all_sources(proposals)
    update_yaml(proposals)
    run_make_split()
    print("\nDone. Run 'make check' to verify the build.")

if __name__ == "__main__":
    main()
