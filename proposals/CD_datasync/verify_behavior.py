#!/usr/bin/env python3
"""Finite CD_datasync differential checks against SHA-verified retail MIPS.

VSync, diagnostic calls and CD_flush are modeled external calls. A simulated
DMA word replaces MMIO. This checks function behavior, not hardware timing.
Requires unicorn, pyelftools, and mipsel-none-elf-ld.
"""
import argparse
import hashlib
import re
from pathlib import Path
import struct
import subprocess
import tempfile

from elftools.elf.elffile import ELFFile
from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN, UC_HOOK_CODE, UC_HOOK_MEM_READ
from unicorn.mips_const import UC_MIPS_REG_A0, UC_MIPS_REG_A1, UC_MIPS_REG_A2, UC_MIPS_REG_A3, UC_MIPS_REG_SP, UC_MIPS_REG_RA, UC_MIPS_REG_PC, UC_MIPS_REG_V0

ENTRY, RETURN, STACK = 0x8007BDDC, 0x8000F000, 0x801FF000
SHA1 = '452fb033f2eaa4b18aa20a5bca60b8125af3a37b'
CALLS = {'VSync': 0x80073A44, 'puts': 0x80073C5C, 'printf': 0x80071A74, 'CD_flush': 0x8007B9EC}



def cases():
    for mode in [0, 1, -1, 0x7fffffff, -0x80000000]:
        for busy in [[], [1], [1, 1, 1]]:
            yield f'mode={mode},busy={len(busy)}', mode, [100] * 8, busy, None
        for elapsed in [959, 960, 961]:
            yield f'deadline={elapsed},mode={mode}', mode, [100, 100 + elapsed, 1061], [1], None
        for counter in [0x3bffff, 0x3c0000, 0x3c0001]:
            yield f'counter={counter:x},mode={mode}', mode, [100] * 8, [1, 1, 1], counter


def execute(exe, patches, mode, times, busy, counter, entries=None):
    cpu = Uc(UC_ARCH_MIPS, UC_MODE_MIPS32 | UC_MODE_LITTLE_ENDIAN)
    cpu.mem_map(0, 0x200000)
    def write(address, value):
        cpu.mem_write(address & 0x1fffffff, struct.pack('<I', value & 0xffffffff))
    def read(address):
        return struct.unpack('<I', cpu.mem_read(address & 0x1fffffff, 4))[0]
    address, size = struct.unpack_from('<II', exe, 0x18)
    cpu.mem_write(address & 0x1fffffff, exe[0x800:0x800 + size])
    for address, data in patches:
        cpu.mem_write(address & 0x1fffffff, data)
    write(0x8009B2C0, 0x80130000)
    cpu.mem_write(0x9B294, bytes([2, 3]))
    cpu.mem_write(0x9AFD5, bytes([4]))
    for i in range(16):
        write(0x8009AFDC + 4*i, 0x80140000 + 16*i)
        write(0x8009B05C + 4*i, 0x80150000 + 16*i)
    trace, ticks, reads = [], 0, 0
    def dma(cpu, access, address, size, value, data):
        nonlocal reads
        value = 0x1000000 if reads < len(busy) and busy[reads] else 0
        write(0x80130000, value)
        trace.append(('dma', value))
        reads += 1
    cpu.hook_add(UC_HOOK_MEM_READ, dma, begin=0x130000, end=0x130003)
    def call(cpu, address, size, name):
        nonlocal ticks
        args = [cpu.reg_read(r) for r in [UC_MIPS_REG_A0, UC_MIPS_REG_A1, UC_MIPS_REG_A2, UC_MIPS_REG_A3]]
        if name == 'VSync':
            trace.append((name, args[0]))
            if ticks == 1 and counter is not None:
                write(0x800A347C, counter)
            result = times[min(ticks, len(times)-1)]
            ticks += 1
        elif name == 'printf':
            trace.append((name, *args, read(cpu.reg_read(UC_MIPS_REG_SP) + 16)))
            result = 17
        elif name == 'puts':
            trace.append((name, args[0]))
            result = 0
        else:
            trace.append((name,))
            result = 0
        cpu.reg_write(UC_MIPS_REG_V0, result & 0xffffffff)
        cpu.reg_write(UC_MIPS_REG_PC, cpu.reg_read(UC_MIPS_REG_RA))
    for name, address in CALLS.items():
        if entries is not None:
            address = entries.get(name, address)
        cpu.hook_add(UC_HOOK_CODE, call, user_data=name, begin=address, end=address)
    cpu.reg_write(UC_MIPS_REG_A0, mode & 0xffffffff)
    cpu.reg_write(UC_MIPS_REG_SP, STACK)
    cpu.reg_write(UC_MIPS_REG_RA, RETURN)
    cpu.emu_start(entries["CD_datasync"] if entries is not None else ENTRY, RETURN, count=10000)
    if cpu.reg_read(UC_MIPS_REG_PC) != RETURN:
        raise RuntimeError('instruction budget exhausted')
    return cpu.reg_read(UC_MIPS_REG_V0), trace, bytes(cpu.mem_read(0xA3478, 12))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('object', type=Path)
    parser.add_argument('--exe', type=Path, default=Path('build/USA/main.exe'))
    args = parser.parse_args()
    exe = args.exe.read_bytes()
    if hashlib.sha1(exe).hexdigest() != SHA1:
        parser.error('reference EXE does not match retail SHA-1')
    with tempfile.TemporaryDirectory(prefix='cd-datasync-') as temp:
        script, linked = Path(temp)/'link.ld', Path(temp)/'candidate.elf'
        symbols = dict(re.findall(r'^(\w+) = (0x[0-9A-Fa-f]+);', Path('configs/USA/sym.main.txt').read_text(), re.M))
        symbols.update(re.findall(r'^(\w+) = (0x[0-9A-Fa-f]+);', Path('linkers/USA/undefined_syms_manual.txt').read_text(), re.M))
        undefined = subprocess.check_output(['mipsel-none-elf-nm', '-u', str(args.object)], text=True).split()[1::2]
        definitions = [f'{n} = ' + ('0x' + n[2:] if n.startswith('D_') else symbols[n]) + ';' for n in undefined]
        script.write_text('\n'.join(definitions) + '''
SECTIONS {
 .text 0x80180000 : { *(.text) }
 .rodata 0x80160000 : { *(.rodata) }
 /DISCARD/ : { *(.reginfo) *(.MIPS.abiflags) *(.pdr) *(.comment) *(.gnu.attributes) }
}
''')
        subprocess.run(['mipsel-none-elf-ld', '-T', str(script), str(args.object), '-o', str(linked)], check=True)
        with linked.open('rb') as stream:
            elf = ELFFile(stream)
            entries = {s.name: s['st_value'] for s in elf.get_section_by_name('.symtab').iter_symbols()}
            patches = [(s['sh_addr'], s.data()) for s in elf.iter_sections() if s['sh_flags'] & 2 and s['sh_size']]
        for count, (name, *case) in enumerate(cases(), 1):
            expected, actual = execute(exe, [], *case), execute(exe, patches, *case, entries=entries)
            if actual != expected:
                raise SystemExit(f'FAIL {name}\nretail={expected!r}\ncandidate={actual!r}')
            if not any(event[0] == 'dma' for event in expected[1]) and expected[0] != 0xffffffff:
                raise RuntimeError('DMA hook was not exercised')
        print(f'PASS: {count} cases agree on return values, external call/DMA traces and timeout state')


if __name__ == '__main__':
    main()
