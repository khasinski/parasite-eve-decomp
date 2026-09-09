#!/usr/bin/env python3
"""Compare getintr C with retail MIPS under finite modeled CD FIFO traces."""
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
with tempfile.TemporaryDirectory(prefix='getintr-') as work:
    script, linked = Path(work) / 'candidate.ld', Path(work) / 'candidate.elf'
    undefined = subprocess.check_output(['mipsel-none-elf-nm', '-u', sys.argv[1]], text=True).split()[1::2]
    symbols = dict(re.findall(r'^(\w+) = (0x[0-9A-Fa-f]+);', Path('configs/USA/sym.main.txt').read_text(), re.M))
    definitions = [f'{name} = ' + ('0x' + name[2:] if name.startswith('D_') else symbols[name]) + ';' for name in undefined]
    script.write_text('\n'.join(definitions) + '\nSECTIONS { .text 0x8007AAB4 : { *(.text) } .rodata 0x80140000 : { *(.rodata*) } /DISCARD/ : { *(.reginfo) *(.MIPS.abiflags) *(.pdr) *(.comment) *(.gnu.attributes) } }')
    subprocess.run(['mipsel-none-elf-ld', '-T', str(script), sys.argv[1], '-o', str(linked)], check=True)
    with linked.open('rb') as stream:
        elf = ELFFile(stream)
        sections = [(s['sh_addr'] & 0x1FFFFFFF, s.data()) for s in elf.iter_sections() if s.name in ('.text', '.rodata')]


def run(event, response, old_status, flags, debug, unstable, candidate):
    cpu = Uc(UC_ARCH_MIPS, UC_MODE_MIPS32 | UC_MODE_LITTLE_ENDIAN)
    cpu.mem_map(0, 0x200000)
    start, size = struct.unpack_from('<II', exe, 0x18)
    cpu.mem_write(start & 0x1FFFFFFF, exe[0x800:0x800 + size])
    if candidate:
        for address, data in sections:
            cpu.mem_write(address, data)
    def word(address, value):
        cpu.mem_write(address, struct.pack('<I', value & 0xFFFFFFFF))
    for i, address in enumerate((0x9B27C, 0x9B280, 0x9B284, 0x9B288)):
        word(address, 0x80130000 + i)
    word(0x9AFC0, debug)
    word(0x9AFC4, old_status)
    word(0x9AFC8, 0x12345678)
    word(0x9AFCC, 10)
    cpu.mem_write(0x9AFD5, b'\x01')
    word(0x9B17C + 4, flags & 1)
    word(0x9B07C + 4, flags >> 1)
    cpu.mem_write(0x9B294, b'\x91\x92\x93')
    cpu.mem_write(0xA3460, bytes(range(24)))
    trace = []
    state = {'fifo': 0, 'interrupt_reads': 0}
    samples = [event, event ^ 1, event, event] if unstable and event else [event]
    def access(u, kind, address, size, value, user):
        address &= 0x1FFFFFFF
        if not 0x130000 <= address <= 0x130003:
            return
        assert size == 1
        port = address - 0x130000
        if kind == UC_MEM_WRITE:
            trace.append(('write', port, value))
            return
        if port == 0:
            value = 0x20 if state['fifo'] < len(response) else 0
        elif port == 1:
            assert state['fifo'] < len(response)
            value = response[state['fifo']]
            state['fifo'] += 1
        elif port == 3:
            value = samples[min(state['interrupt_reads'], len(samples) - 1)]
            state['interrupt_reads'] += 1
        else:
            raise AssertionError(port)
        u.mem_write(address, bytes([value]))
        trace.append(('read', port, value))
    cpu.hook_add(UC_HOOK_MEM_READ | UC_HOOK_MEM_WRITE, access)
    def call(u, address, size, user):
        args = [u.reg_read(r) for r in (UC_MIPS_REG_A0, UC_MIPS_REG_A1, UC_MIPS_REG_A2, UC_MIPS_REG_A3)]
        # Only initialized arguments belonging to this format are observable.
        argc = {0x80011B44: 1, 0x80011B50: 4, 0x80011B6C: 1, 0x80011B80: 2}[args[0]]
        trace.append(('call', address, tuple(args[:argc])))
        u.reg_write(UC_MIPS_REG_V0, 0)
        u.reg_write(UC_MIPS_REG_PC, u.reg_read(UC_MIPS_REG_RA))
    for address in (0x80071A74, 0x80073C5C):
        cpu.hook_add(UC_HOOK_CODE, call, begin=address, end=address)
    cpu.reg_write(UC_MIPS_REG_SP, 0x801F0000)
    cpu.reg_write(UC_MIPS_REG_RA, 0x801E0000)
    cpu.emu_start(0x8007AAB4, 0x801E0000, count=10000)
    assert cpu.reg_read(UC_MIPS_REG_PC) == 0x801E0000
    assert trace[:2] == [('write', 0, 1), ('read', 3, event)]
    assert state['fifo'] == (len(response) if event else 0)
    return cpu.reg_read(UC_MIPS_REG_V0), bytes(cpu.mem_read(0x9AFC4, 12)), bytes(cpu.mem_read(0x9B294, 3)), bytes(cpu.mem_read(0xA3460, 24)), trace

responses = [b''] + [bytes([status] + list(range(1, length))) for length in (1, 2, 8) for status in (0, 1, 4, 8, 0x10, 0x1D, 0xFF)]
cases = itertools.product(range(8), responses, (0, 0x10, 0x12340010), range(4), (0, 1), (False, True))
for count, case in enumerate(cases, 1):
    expected = run(*case, False)
    assert expected == run(*case, True), case
print(f'PASS {count} cases: return, status words, events, three response buffers, FIFO/MMIO and diagnostic traces')
