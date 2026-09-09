#!/usr/bin/env python3
"""Compare ordinary-C func_8007A740 against retail, modeling external APIs."""
import hashlib
import itertools
from pathlib import Path
import re
import struct
import subprocess
import sys
import tempfile
from elftools.elf.elffile import ELFFile
from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN, UC_HOOK_CODE
from unicorn.mips_const import UC_MIPS_REG_A0, UC_MIPS_REG_A1, UC_MIPS_REG_A2, UC_MIPS_REG_A3, UC_MIPS_REG_V0, UC_MIPS_REG_SP, UC_MIPS_REG_RA, UC_MIPS_REG_PC


exe = Path('build/USA/main.exe').read_bytes()
assert hashlib.sha1(exe).hexdigest() == '452fb033f2eaa4b18aa20a5bca60b8125af3a37b'
with tempfile.TemporaryDirectory(prefix='cd-command-retry-') as work:
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

def run(command, parameter_present, lid, needs_location, outcome, sync_result, candidate):
    cpu = Uc(UC_ARCH_MIPS, UC_MODE_MIPS32 | UC_MODE_LITTLE_ENDIAN)
    cpu.mem_map(0, 0x200000)
    start, size = struct.unpack_from('<II', exe, 0x18)
    cpu.mem_write(start & 0x1FFFFFFF, exe[0x800:0x800 + size])
    if candidate:
        for address, data in sections: cpu.mem_write(address, data)
    def word(a, v): cpu.mem_write(a & 0x1FFFFFFF, struct.pack('<I', v & 0xFFFFFFFF))
    def read(a): return int.from_bytes(cpu.mem_read(a & 0x1FFFFFFF, 4), 'little')
    saved = 0x801C0000
    parameter, result = (0x80180000 if parameter_present else 0), 0x80180100
    cpu.mem_write(0x180000, bytes([0xA5]) * 0x120)
    word(0x9AFB4, saved); word(0x9AFC4, 0xA5A50000 | (0x10 if lid else 0))
    word(0x9AF2C + (command & 255) * 4, needs_location)
    trace = []
    # Unicorn 2.1.4 memory-write hooks fail on the retail SW in a branch delay
    # slot. Observe that SW at instruction entry instead; execute it unchanged.
    from unicorn.mips_const import UC_MIPS_REG_0
    def store_instruction(u, address, size, user):
        instruction = read(address)
        if instruction >> 26 == 0x2B:
            base = (instruction >> 21) & 31
            source = (instruction >> 16) & 31
            offset = instruction & 0xFFFF
            if offset & 0x8000: offset -= 0x10000
            target = (u.reg_read(UC_MIPS_REG_0 + base) + offset) & 0x1FFFFFFF
            if target == 0x9AFB4:
                trace.append(('callback', u.reg_read(UC_MIPS_REG_0 + source)))
    cpu.hook_add(UC_HOOK_CODE, store_instruction)
    apis = {int(symbols[name], 16) + 4: name for name in ('CD_cw', 'CD_sync')}
    trapped = []
    def trap(u, address, size, user): trapped.append(address); u.emu_stop()
    for a in apis:
        cpu.mem_write((a - 4) & 0x1FFFFFFF, bytes(8))
        cpu.hook_add(UC_HOOK_CODE, trap, begin=a, end=a)
    cpu.reg_write(UC_MIPS_REG_A0, command)
    cpu.reg_write(UC_MIPS_REG_A1, parameter)
    cpu.reg_write(UC_MIPS_REG_A2, result)
    cpu.reg_write(UC_MIPS_REG_SP, 0x801F0000)
    cpu.reg_write(UC_MIPS_REG_RA, 0x801E0000)
    pc = entries['func_8007A740'] if candidate else int(symbols['func_8007A740'], 16)
    calls = 0; syncs = 0
    for _ in range(20):
        cpu.emu_start(pc, 0x801E0000, count=1000)
        if not trapped: break
        name = apis[trapped.pop()]
        args = tuple(cpu.reg_read(r) for r in (UC_MIPS_REG_A0, UC_MIPS_REG_A1, UC_MIPS_REG_A2, UC_MIPS_REG_A3))
        if name == 'CD_cw':
            assert args[0] in (1, 2, command & 255) and args[3] == 0
            value = (0, 0xFFFFFFFF, calls & 1, int(calls < 3))[outcome]
            calls += 1
            trace.append((name, *args, read(0x9AFB4), value))
            if args[2]: cpu.mem_write(args[2] & 0x1FFFFFFF, bytes([calls]) * 8)
        else:
            assert args[:2] == (0, result) and read(0x9AFB4) == saved
            syncs += 1; value = sync_result
            trace.append((name, args[0], args[1], value))
            cpu.mem_write(result & 0x1FFFFFFF, bytes([sync_result]) * 8)
        cpu.reg_write(UC_MIPS_REG_V0, value)
        pc = cpu.reg_read(UC_MIPS_REG_RA)
    assert cpu.reg_read(UC_MIPS_REG_PC) == 0x801E0000
    assert cpu.reg_read(UC_MIPS_REG_SP) == 0x801F0000
    assert read(0x9AFB4) == saved and syncs <= 1 and calls <= 12
    returned = cpu.reg_read(UC_MIPS_REG_V0)
    assert returned == int(syncs == 1 and sync_result == 2)
    return trace, returned, bytes(cpu.mem_read(0x180000, 0x120))

for count, case in enumerate(itertools.product((1, 2, 6, 0x106), (False, True), (False, True), (0, 1), range(4), (0, 2, 5)), 1):
    assert run(*case, False) == run(*case, True), case
print(f'PASS {count} cases: low-byte commands, optional status/location commands, bounded retries, callback suppression/restoration, sync result and result-buffer effects')
