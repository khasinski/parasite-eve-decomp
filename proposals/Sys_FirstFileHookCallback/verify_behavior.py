#!/usr/bin/env python3
"""Compare ordinary-C Sys_FirstFileHookCallback against retail, modeling external APIs."""
import hashlib
import itertools
from pathlib import Path
import re
import struct
import subprocess
import sys
import tempfile
from elftools.elf.elffile import ELFFile
from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN, UC_HOOK_CODE, UC_HOOK_MEM_READ, UC_HOOK_MEM_WRITE, UC_MEM_WRITE
from unicorn.mips_const import UC_MIPS_REG_A0, UC_MIPS_REG_A1, UC_MIPS_REG_A2, UC_MIPS_REG_A3, UC_MIPS_REG_V0, UC_MIPS_REG_SP, UC_MIPS_REG_RA, UC_MIPS_REG_PC


exe = Path('build/USA/main.exe').read_bytes()
assert hashlib.sha1(exe).hexdigest() == '452fb033f2eaa4b18aa20a5bca60b8125af3a37b'
with tempfile.TemporaryDirectory(prefix='first-file-hook-') as work:
    script, linked = Path(work) / 'candidate.ld', Path(work) / 'candidate.elf'
    undefined = subprocess.check_output(['mipsel-none-elf-nm', '-u', sys.argv[1]], text=True).split()[1::2]
    symbols = dict(re.findall(r'^(\w+) = (0x[0-9A-Fa-f]+);', Path('configs/USA/sym.main.txt').read_text(), re.M))
    symbols.update(re.findall(r'^(\w+) = (0x[0-9A-Fa-f]+);', Path('linkers/USA/undefined_syms_manual.txt').read_text(), re.M))
    definitions = [f'{name} = ' + ('0x' + name[2:] if name.startswith('D_') else symbols[name]) + ';' for name in undefined]
    script.write_text('\n'.join(definitions) + '\nSECTIONS { .text 0x80150000 : { *(.text) } .rodata 0x80140000 : { *(.rodata*) } /DISCARD/ : { *(.reginfo) *(.MIPS.abiflags) *(.pdr) *(.comment) *(.gnu.attributes) } }')
    subprocess.run(['mipsel-none-elf-ld', '-T', str(script), sys.argv[1], '-o', str(linked)], check=True)
    with linked.open('rb') as stream:
        elf = ELFFile(stream)
        entries = {symbol.name: symbol['st_value'] for symbol in elf.get_section_by_name('.symtab').iter_symbols()}
        sections = [(s['sh_addr'] & 0x1FFFFFFF, s.data()) for s in elf.iter_sections() if s.name in ('.text', '.rodata')]

def run(state, byte_count, pattern, change_callback, candidate):
    cpu = Uc(UC_ARCH_MIPS, UC_MODE_MIPS32 | UC_MODE_LITTLE_ENDIAN)
    cpu.mem_map(0, 0x200000)
    start, size = struct.unpack_from('<II', exe, 0x18)
    cpu.mem_write(start & 0x1FFFFFFF, exe[0x800:0x800 + size])
    if candidate:
        for address, data in sections: cpu.mem_write(address, data)
    def word(a, v): cpu.mem_write(a & 0x1FFFFFFF, struct.pack('<I', v & 0xFFFFFFFF))
    def read(a): return int.from_bytes(cpu.mem_read(a & 0x1FFFFFFF, 4), 'little')
    def string(a):
        result = bytearray()
        while (b := cpu.mem_read(a & 0x1FFFFFFF, 1)) != b'\0':
            result.extend(b); a += 1
            assert len(result) < 100
        return bytes(result)
    table, state_address = 0x80180000, 0x80181000
    callback, replacement = 0x801C0000, 0x801C0020
    cb_global, match_name = int(symbols['D_800A32D0'], 16), int(symbols['D_800A32D8'], 16)
    word(0x150, table); word(0x154, byte_count)
    word(state_address, state); word(cb_global, callback)
    cpu.mem_write(match_name & 0x1FFFFFFF, b'cdrom\0')
    cpu.mem_write(0x182000, b'other\0cdrom\0')
    cpu.mem_write(table & 0x1FFFFFFF, bytes([0xA5]) * (80 * 4))
    names = [(0, 0x80182000, 0x80182000, 0),
             (0x80182006, 0x80182000, 0x80182006, 0),
             (0, 0x80182000, 0, 0x80182006)][pattern]
    for i, name in enumerate(names): word(table + i * 80, name)
    trace = []
    def store(u, kind, address, size, value, user):
        address &= 0x1FFFFFFF
        if address == (state_address & 0x1FFFFFFF) or 0x180000 <= address < 0x180140:
            assert size == 4
            trace.append(('write', address, value))
    cpu.hook_add(UC_HOOK_MEM_WRITE, store)
    strcmp_address = int(symbols['strcmp'], 16)
    apis = {strcmp_address + 4: 'strcmp', callback + 4: 'callback', replacement + 4: 'replacement'}
    trapped = []
    def trap(u, address, size, user): trapped.append(address); u.emu_stop()
    for a in apis:
        cpu.mem_write((a - 4) & 0x1FFFFFFF, bytes(8))
        cpu.hook_add(UC_HOOK_CODE, trap, begin=a, end=a)
    cpu.reg_write(UC_MIPS_REG_A0, state_address)
    cpu.reg_write(UC_MIPS_REG_A1, 0x12345678)
    cpu.reg_write(UC_MIPS_REG_A2, 0x87654321)
    cpu.reg_write(UC_MIPS_REG_SP, 0x801F0000)
    cpu.reg_write(UC_MIPS_REG_RA, 0x801E0000)
    pc = entries['Sys_FirstFileHookCallback'] if candidate else int(symbols['Sys_FirstFileHookCallback'], 16)
    compared = []
    for _ in range(8):
        cpu.emu_start(pc, 0x801E0000, count=1000)
        if not trapped: break
        name = apis[trapped.pop()]
        args = tuple(cpu.reg_read(r) for r in (UC_MIPS_REG_A0, UC_MIPS_REG_A1, UC_MIPS_REG_A2))
        if name == 'strcmp':
            left, right = string(args[0]), string(args[1])
            assert args[1] == match_name
            compared.append(args[0]); trace.append((name, left, right))
            result = 0 if left == right else 1
            if change_callback: word(cb_global, replacement)
        else:
            assert args == (state_address, 0x12345678, 0x87654321)
            assert name == ('replacement' if change_callback and compared else 'callback')
            trace.append((name, *args)); result = 0xF1234567
        cpu.reg_write(UC_MIPS_REG_V0, result)
        pc = cpu.reg_read(UC_MIPS_REG_RA)
    assert cpu.reg_read(UC_MIPS_REG_PC) == 0x801E0000
    assert cpu.reg_read(UC_MIPS_REG_SP) == 0x801F0000
    assert cpu.reg_read(UC_MIPS_REG_V0) == 0xF1234567
    expected_compared = []
    expected = bytearray([0xA5] * (80 * 4))
    for i, name in enumerate(names): struct.pack_into('<I', expected, i * 80, name)
    for i in range(byte_count // 80):
        if names[i]: expected_compared.append(names[i])
        if names[i] == 0x80182006:
            struct.pack_into('<I', expected, i * 80 + 0x34, callback)
            break
    assert compared == expected_compared
    assert cpu.mem_read(table & 0x1FFFFFFF, 80 * 4) == bytes(expected)
    assert read(state_address) == (state if state else 1)
    return trace

for count, case in enumerate(itertools.product((0, 1, 0xFFFFFFFF), (0, 79, 80, 159, 160, 320), range(3), (False, True)), 1):
    assert run(*case, False) == run(*case, True), case
print(f'PASS {count} cases: state initialization, table size truncation, null/nonmatching/duplicate names, first-match patch, callback snapshot/reload, argument forwarding and return')
