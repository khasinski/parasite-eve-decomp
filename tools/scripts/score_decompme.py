#!/usr/bin/env python3
"""Score two relocatable objects with decomp.me's asm-differ configuration.

The weighted score is a search aid, not proof of a linked byte match.
Configuration: decompme/decomp.me backend/coreapp/diff_wrapper.py.
"""
import argparse
import json
from pathlib import Path
import subprocess

import diff as asm_differ


def create_config():
    arch = asm_differ.get_arch("mipsel")
    return asm_differ.Config(
        arch=arch, diff_obj=True, file="", ref_file=None, make=False,
        source_old_binutils=True, diff_section=".text", inlines=False,
        max_function_size_lines=25000, max_function_size_bytes=100000,
        formatter=asm_differ.PythonFormatter(arch_str=arch.name),
        diff_mode=asm_differ.DiffMode.NORMAL, base_shift=0, skip_lines=0,
        compress=None, show_rodata_refs=True, show_branches=True,
        show_line_numbers=False, show_source=False, stop_at_ret=False,
        ignore_large_imms=False, ignore_addr_diffs=True,
        algorithm="levenshtein", reg_categories={}, diff_function_symbols=False,
    )


def read_lines(path, config, objdump, symbol=None):
    command = [objdump, *config.arch.arch_flags, "-drz", "-j", ".text"]
    if symbol:
        command.append("--disassemble=" + symbol)
    dump = subprocess.check_output([*command, str(path)], text=True)
    dump = asm_differ.preprocess_objdump_out(None, path.read_bytes(), dump, config)
    return asm_differ.process(dump, config)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("target", type=Path)
    parser.add_argument("candidate", type=Path)
    parser.add_argument("--target-symbol")
    parser.add_argument("--candidate-symbol")
    parser.add_argument("--objdump", default="mipsel-none-elf-objdump")
    args = parser.parse_args()
    config = create_config()
    target = read_lines(args.target, config, args.objdump, args.target_symbol)
    candidate = read_lines(args.candidate, config, args.objdump, args.candidate_symbol)
    if not target or not candidate:
        parser.error("both objects must contain instructions in the selected function")
    result = asm_differ.do_diff(target, candidate, config)
    print(json.dumps({"scorer": "asm-differ", "algorithm": "levenshtein",
                      "score": result.score, "max_score": result.max_score,
                      "target_lines": len(target),
                      "candidate_lines": len(candidate)}))


if __name__ == "__main__":
    main()
