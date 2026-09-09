#!/usr/bin/env python3
"""Compare SPU FIFO transfer C with retail under finite status traces."""
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

def run(length, pending, fill, redirect, candidate):
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
    cpu.mem_write(0x9B414, struct.pack('<H', 0x2345))
    cpu.mem_write(0x130000, bytes([fill]) * 0x400)
    cpu.mem_write(0x120000, bytes(range(256)))
    trace, waits, fifo, source_reads = [], 0, [], []
    phase, polls, chunks = 'initial', 0, 0
    initial = 0xA221 if fill else 0
    def access(u, kind, address, size, value, user):
        nonlocal phase, polls, chunks
        address &= 0x1FFFFFFF
        if 0x120000 <= address < 0x120100:
            assert kind != UC_MEM_WRITE and size == 2
            source_reads.append(address)
            return
        if not 0x130000 <= address < 0x130400:
            return
        offset = (address - 0x130000) & 0x1FF
        assert size == 2
        if kind != UC_MEM_WRITE:
            if offset == 0x1AE:
                if phase == 'initial':
                    value = initial
                elif phase == 'transfer':
                    value = 0x400 if polls < pending else 0
                    polls += 1
                else:
                    value = (initial & 0x7FF) ^ int(polls < pending)
                    polls += 1
                u.mem_write(address, struct.pack('<H', value))
            value = int.from_bytes(u.mem_read(address, size), 'little')
        elif offset == 0x1AA:
            phase = 'transfer' if value & 0x30 == 0x10 else 'restore'
            polls = 0
            if phase == 'transfer': chunks += 1
        elif offset == 0x1A8:
            fifo.append(value)
        trace.append(('write' if kind == UC_MEM_WRITE else 'read', address, value))
    cpu.hook_add(UC_HOOK_MEM_READ | UC_HOOK_MEM_WRITE, access)
    api = {(entries.get(name, int(symbols[name], 16)) if candidate else int(symbols[name], 16)) + 4: name for name in ('_spu_Fw1ts', 'printf')}
    trapped = []
    def trap(u, address, size, user):
        trapped.append(address)
        u.emu_stop()
    for address in api:
        cpu.mem_write((address - 4) & 0x1FFFFFFF, bytes(8))
        cpu.hook_add(UC_HOOK_CODE, trap, begin=address, end=address)
    cpu.reg_write(UC_MIPS_REG_SP, 0x801F0000)
    cpu.reg_write(UC_MIPS_REG_RA, 0x801E0000)
    cpu.reg_write(UC_MIPS_REG_A0, 0x80120000)
    cpu.reg_write(UC_MIPS_REG_A1, length)
    pc = entries['_spu_FwriteByIO'] if candidate else 0x8007D454
    for step in range(64):
        cpu.emu_start(pc, 0x801E0000, count=100000)
        if not trapped: break
        name = api[trapped.pop()]
        if name == '_spu_Fw1ts':
            waits += 1
            trace.append((name,))
            if redirect and waits == 1: word(0x9B3FC, 0x80130200)
        else:
            args = tuple(cpu.reg_read(r) for r in (UC_MIPS_REG_A0, UC_MIPS_REG_A1))
            assert args == (0x80011C4C, 0x80011C6C if phase == 'transfer' else 0x80011C80)
            trace.append((name, args))
        cpu.reg_write(UC_MIPS_REG_V0, 0x1234)
        pc = cpu.reg_read(UC_MIPS_REG_RA)
    assert cpu.reg_read(UC_MIPS_REG_PC) == 0x801E0000
    assert cpu.reg_read(UC_MIPS_REG_SP) == 0x801F0000
    assert chunks == (length + 63) // 64
    assert waits == 1 + 3 * chunks
    assert source_reads == list(range(0x120000, 0x120000 + length, 2))
    assert fifo == [i | ((i + 1) << 8) for i in range(0, length, 2)]
    assert sum(t[0] == 'printf' for t in trace) == (chunks + 1 if pending >= 3841 else 0)
    assert polls == min(pending + 1, 3841)
    return trace, source_reads, bytes(cpu.mem_read(0x130000, 0x400))

for count, case in enumerate(itertools.product((0, 1, 2, 63, 64, 65, 128, 129), (0, 1, 3840, 3841), (0, 0xA5), (False, True)), 1):
    assert run(*case, False) == run(*case, True), case
print(f'PASS {count} cases: FIFO words, chunk boundaries, odd lengths, both timeouts, pointer reload and stack')
