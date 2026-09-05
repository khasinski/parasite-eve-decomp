#!/usr/bin/env python3
"""Classify C sources for honest semantic-decompilation reporting."""
from __future__ import annotations

import ast
import pathlib
import re


C_STRING = r'"(?:[^"\\]|\\.)*"'
TEMPLATE_PART = C_STRING + r'|[A-Za-z_]\w*\s*\([^()]*\)|#?[A-Za-z_]\w*'
ASM_CALL = re.compile(
    r'\b(?:__asm__|__asm|asm)\s*(?:(?:volatile|__volatile__)\s*)?\(\s*'
    r'(?P<template>(?:(?:' + TEMPLATE_PART + r')\s*)+)',
    re.S,
)
COMMENTS_AND_LITERALS = re.compile(
    C_STRING + r"|'(?:[^'\\]|\\.)*'|/\*.*?\*/|//[^\n]*", re.S
)
INC_INCLUDE = re.compile(r'^\s*#\s*include\s+"([^"]+\.inc)"', re.MULTILINE)
ASM_INCLUDE = re.compile(r'^\s*\.include\s+"', re.MULTILINE)
TEXT_SECTION = re.compile(r'\bsection\s*\(\s*"\.text"\s*\)')
FUNCTION_DEF = re.compile(
    r"^\s*(?!if\b|for\b|while\b|switch\b)[A-Za-z_][\w\s*]*\b[A-Za-z_]\w*"
    r"\s*\([^;{}]*\)\s*\{",
    re.MULTILINE,
)
COP2_OP = re.compile(r'\b(?:cfc2|ctc2|lwc2|swc2|mfc2|mtc2)\b')
# Audited CPU-ASM helpers hidden in headers. Quarantine their callers until the
# CPU windows are C; this is not a substitute for full macro-expansion review.
CPU_ASM_HELPERS = re.compile(
    r'\b(?:gte_(?:ldv0_short3|load_packed_short3|store_ir123_packed_short3|'
    r'stir123_matrix_column|stsz3_s16|'
    r'store_mac12_byte2|store_mac123_byte3|store_third_output|store_flag_bound)|'
    r'ROOMLIB_(?:LOAD_S16|LOAD_PTR|LOAD_U16|DIV_V0_A0_CHECKED))\s*\('
)


def strip_comments(text: str) -> str:
    # C splices continued lines before interpreting comments and strings.
    text = re.sub(r"\\\r?\n", "", text)
    return COMMENTS_AND_LITERALS.sub(
        lambda match: " " if match.group().startswith(("/*", "//")) else match.group(),
        text,
    )


def included_inc_text(path: pathlib.Path, text: str) -> str:
    """Read directly included C templates; missing/generated includes add nothing."""
    parts = []
    for name in INC_INCLUDE.findall(text):
        include = path.parent / name
        if include.exists():
            parts.append(include.read_text(errors="ignore"))
    return "\n".join(parts)


def has_instruction_asm(text: str) -> bool:
    """Whether inline asm emits instructions or assembler directives.

    Register pins, symbol aliases, and empty compiler barriers do not emit
    bytes and remain semantic C. They are tracked separately as crutch debt.
    """
    text = strip_comments(text)
    if ASM_INCLUDE.search(text):
        return True
    for match in ASM_CALL.finditer(text):
        literals = re.findall(C_STRING, match.group("template"), re.S)
        try:
            body = "".join(ast.literal_eval(literal) for literal in literals).strip()
        except (SyntaxError, ValueError):
            return True
        statement = text[max(text.rfind(";", 0, match.start()),
                             text.rfind("{", 0, match.start())) + 1:match.start()]
        opaque = re.sub(C_STRING, "", match.group("template"), flags=re.S).strip()
        if not body and not opaque:
            continue
        declaration = re.search(
            r'(?:\b(?:extern|register)\b[^=]*|\}\s*\w+|'
            r'\b\w+(?:\s|\*)+\w+(?:\s*\[[^\]]*\])+|'
            r'\b\w+(?:\s|\*)+\w+\s*\([^{};]*\))\s*$', statement, re.S
        )
        if (declaration and not opaque
                and re.fullmatch(r"[A-Za-z_$][\w.$]*|0x[0-9A-Fa-f]+", body)):
            continue
        # Only fully visible transfer windows qualify automatically. Mixed CPU
        # bridges and opaque macro fragments require separate manual review.
        instructions = [part.strip() for part in re.split(r"[;\n]", body)
                        if part.strip()]
        if (not opaque and COP2_OP.search(body) and instructions
                and all(re.fullmatch(r"(?:cfc2|ctc2|lwc2|swc2|mfc2|mtc2)\s+[^:#]+|nop",
                                     instruction) for instruction in instructions)):
            continue
        # Nonempty templates are not safe just because their first token is
        # a label or an opcode absent from a hand-maintained instruction list.
        return True
    return False


def classify(path: pathlib.Path) -> str:
    text = path.read_text(errors="ignore")
    expanded = text + "\n" + included_inc_text(path, text)
    if "PSYQ_BIOS_TRAMPOLINE" in expanded:
        return "original_asm"
    if TEXT_SECTION.search(expanded) and not FUNCTION_DEF.search(expanded):
        return "text_data"
    code = re.sub(C_STRING, '""', strip_comments(expanded))
    if CPU_ASM_HELPERS.search(code):
        return "asm_constrained"
    if has_instruction_asm(expanded):
        return "asm_constrained"
    return "semantic_c"
