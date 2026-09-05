#!/usr/bin/env python3
"""Classify C sources for honest semantic-decompilation reporting."""
from __future__ import annotations

import pathlib
import re


ASM_CALL = re.compile(
    r'\b(?:__asm__|asm)\s*(?:(?:volatile|__volatile__)\s*)?\(\s*"([^"\\]*(?:\\.[^"\\]*)*)"'
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
MIPS_OPS = {
    "add", "addi", "addiu", "addu", "and", "andi", "b", "beq", "beqz",
    "bgez", "bgtz", "blez", "bltz", "bne", "bnez", "break", "div",
    "divu", "j", "jal", "jalr", "jr", "lb", "lbu", "lh", "lhu", "lui",
    "lw", "lwl", "lwr", "mfhi", "mflo", "move", "mthi", "mtlo", "mult",
    "multu", "negu", "nop", "nor", "or", "ori", "sb", "sh", "sll",
    "sllv", "slt", "slti", "sltiu", "sltu", "sra", "srav", "srl",
    "srlv", "sub", "subu", "sw", "swl", "swr", "syscall", "xor", "xori",
}


def strip_comments(text: str) -> str:
    return re.sub(r"//.*", "", re.sub(r"/\*.*?\*/", "", text, flags=re.S))


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
        body = match.group(1)
        body = bytes(body, "utf-8").decode("unicode_escape").strip()
        statement = text[max(text.rfind(";", 0, match.start()),
                             text.rfind("{", 0, match.start())) + 1:match.start()]
        if not body or re.search(r"\b(?:extern|register)\b", statement):
            continue
        # One surgical asm expression may include address setup around COP2
        # loads/stores. Treat the whole expression as sanctioned GTE asm.
        tail = text[match.start():match.start() + 1200]
        stops = [i for i in (tail.find(";"), tail.find("#endif")) if i >= 0]
        end = min(stops) if stops else len(tail)
        if COP2_OP.search(tail[:end]):
            continue
        first = re.split(r"[\s\\]", body, maxsplit=1)[0]
        if first.startswith(".") or first.rstrip(":") in MIPS_OPS:
            return True
    return False


def classify(path: pathlib.Path) -> str:
    text = path.read_text(errors="ignore")
    expanded = text + "\n" + included_inc_text(path, text)
    if "PSYQ_BIOS_TRAMPOLINE" in expanded:
        return "original_asm"
    if TEXT_SECTION.search(expanded) and not FUNCTION_DEF.search(expanded):
        return "text_data"
    if has_instruction_asm(expanded):
        return "asm_constrained"
    return "semantic_c"
