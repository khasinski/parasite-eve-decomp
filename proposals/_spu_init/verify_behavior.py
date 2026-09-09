#!/usr/bin/env python3
"""Compare _spu_init C against retail with modeled SPU status and external calls."""
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
with tempfile.TemporaryDirectory(prefix='cd-init-') as work:
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

def run(hot, pending, fill, redirect, candidate):
    cpu = Uc(UC_ARCH_MIPS, UC_MODE_MIPS32 | UC_MODE_LITTLE_ENDIAN)
    cpu.mem_map(0, 0x200000)
    start, size = struct.unpack_from('<II', exe, 0x18)
    cpu.mem_write(start & 0x1FFFFFFF, exe[0x800:0x800 + size])
    if candidate:
        for address, data in sections:
            cpu.mem_write(address, data)
    def word(address, value):
        cpu.mem_write(address, struct.pack('<I', value & 0xFFFFFFFF))
    word(0x9B3FC, 0x80130000)
    word(0x9B40C, 0x80131000)
    for address, size in ((0x130000, 0x400), (0x131000, 4), (0x9B414, 0x28), (0xB68FE, 24)):
        cpu.mem_write(address, bytes([fill]) * size)
    trace, reads, waits = [], 0, 0
    def access(u, kind, address, size, value, user):
        nonlocal reads
        address &= 0x1FFFFFFF
        if not (0x130000 <= address < 0x130400 or address == 0x131000):
            return
        assert size == (4 if address == 0x131000 else 2)
        if kind != UC_MEM_WRITE:
            if address in (0x1301AE, 0x1303AE):
                value = 0x7FF if reads < pending else 0
                reads += 1
                u.mem_write(address, struct.pack('<H', value))
            value = int.from_bytes(u.mem_read(address, size), 'little')
        trace.append(('write' if kind == UC_MEM_WRITE else 'read', address, size, value))
    cpu.hook_add(UC_HOOK_MEM_READ | UC_HOOK_MEM_WRITE, access)
    api = {int(symbols[name], 16) + 4: name for name in ('_spu_Fw1ts', '_spu_FwriteByIO', 'printf')}
    trapped = []
    def trap(u, address, size, user):
        trapped.append(address)
        u.emu_stop()
    for address in api:
        # Modeled calls must not run their original prologues when a hook stops.
        cpu.mem_write((address - 4) & 0x1FFFFFFF, bytes(8))
        cpu.hook_add(UC_HOOK_CODE, trap, begin=address, end=address)
    cpu.reg_write(UC_MIPS_REG_SP, 0x801F0000)
    cpu.reg_write(UC_MIPS_REG_RA, 0x801E0000)
    cpu.reg_write(UC_MIPS_REG_A0, hot & 0xFFFFFFFF)
    pc = entries['_spu_init'] if candidate else 0x8007D1D4
    for step in range(32):
        cpu.emu_start(pc, 0x801E0000, count=100000)
        if not trapped:
            break
        name = api[trapped.pop()]
        if name == '_spu_Fw1ts':
            waits += 1
            trace.append((name,))
            if redirect and waits == 1:
                word(0x9B3FC, 0x80130200)
        else:
            args = tuple(cpu.reg_read(r) for r in (UC_MIPS_REG_A0, UC_MIPS_REG_A1))
            assert args == ((0x8009B43C, 16) if name == '_spu_FwriteByIO' else (0x80011C4C, 0x80011C5C))
            trace.append((name, args))
        cpu.reg_write(UC_MIPS_REG_V0, 0x1234)
        pc = cpu.reg_read(UC_MIPS_REG_RA)
    assert cpu.reg_read(UC_MIPS_REG_PC) == 0x801E0000
    assert cpu.reg_read(UC_MIPS_REG_SP) == 0x801F0000
    assert cpu.reg_read(UC_MIPS_REG_V0) == 0
    assert waits == (9 if hot == 0 else 1)
    assert reads == min(pending + 1, 3841)
    assert sum(t[0] == 'printf' for t in trace) == int(pending >= 3841)
    assert bytes(cpu.mem_read(0xB6900, 20)) == bytes(20)
    assert bytes(cpu.mem_read(0xB68FE, 2)) == bytes([fill]) * 2
    assert bytes(cpu.mem_read(0xB6914, 2)) == bytes([fill]) * 2
    return trace, tuple(bytes(cpu.mem_read(a, n)) for a, n in ((0x130000, 0x400), (0x131000, 4), (0x9B414, 0x28)))

for count, case in enumerate(itertools.product((0, 1, -1), (0, 1, 3840, 3841), (0, 0xA5), (False, True)), 1):
    assert run(*case, False) == run(*case, True), case
print(f'PASS {count} cases: hot/cold initialization, timeout boundary, SPU pointer reload, register traces, software state and stack')
