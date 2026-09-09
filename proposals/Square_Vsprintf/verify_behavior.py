#!/usr/bin/env python3
"""Compare a formatter object with verified USA retail MIPS code in Unicorn.

Requires unicorn, pyelftools, and mipsel-none-elf-ld. This finite behavioral
check supplements, and does not replace, instruction/relocation and SHA checks.
"""
import argparse
import hashlib
from pathlib import Path
import struct
import subprocess
import tempfile

from elftools.elf.elffile import ELFFile
from unicorn import Uc, UcError, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN, UC_HOOK_CODE
from unicorn.mips_const import (
    UC_MIPS_REG_A0, UC_MIPS_REG_A1, UC_MIPS_REG_A2, UC_MIPS_REG_A3,
    UC_MIPS_REG_T1, UC_MIPS_REG_SP, UC_MIPS_REG_RA, UC_MIPS_REG_PC, UC_MIPS_REG_V0,
)

RETAIL_SHA1 = '452fb033f2eaa4b18aa20a5bca60b8125af3a37b'
ENTRY, RETURN, STACK = 0x80071A84, 0x8000F000, 0x801FF000
LINK_SCRIPT = '''
D_80094528 = 0x80094528;
D_8001161C = 0x8001161C;
D_80011630 = 0x80011630;
strlen = 0x80072314;
memchr = 0x80072324;
memmove = 0x80072334;
SECTIONS {
 .text 0x80071A84 : SUBALIGN(4) { *(.text) }
 .rodata 0x80011644 : SUBALIGN(4) { *(.rodata) }
 /DISCARD/ : { *(.reginfo) *(.MIPS.abiflags) *(.pdr) *(.comment) *(.gnu.attributes) }
}
'''


def candidate_layout(object_path):
    """Keep legacy external-data permutations and data-owning TUs testable."""
    with object_path.open('rb') as stream:
        elf = ELFFile(stream)
        defined = {s.name for s in elf.get_section_by_name('.symtab').iter_symbols()
                   if s['st_shndx'] != 'SHN_UNDEF'}
    owned = {name for name in ('D_80094528', 'D_8001161C', 'D_80011630')
             if name in defined}
    tables = owned & {'D_8001161C', 'D_80011630'}
    if tables and len(tables) != 2:
        raise ValueError('candidate must own both digit tables or neither')
    script = LINK_SCRIPT
    for name in owned:
        address = int(name[2:], 16)
        script = script.replace(f'{name} = 0x{address:08X};\n', '')
    if tables:
        script = script.replace('.rodata 0x80011644', '.rodata 0x8001161C')
    script = script.replace(' /DISCARD/', ' .data 0x80094528 : SUBALIGN(4) { *(.data) }\n /DISCARD/')
    return script, owned


def cases():
    yield b'plain text', []
    yield b'', []
    yield b'%% / %c / %c', [65, 0]
    for conversion in 'diuoxXp':
        for flags in ['', '#', '0', '+', ' ', '-', '#0', '+0']:
            for width_precision in ['', '8', '.0', '.5', '10.3']:
                for value in [0, 1, 0x1234, 0xFFFFFFFF, 0x80000000]:
                    yield ('before:%' + flags + width_precision + conversion + ':after').encode(), [value]
        for modifier in ['h', 'l', 'L']:
            yield ('%' + modifier + conversion).encode(), [0xFFFF8001]
        for width, precision in [(8, 3), (-8, 3), (0, 0), (8, -1)]:
            yield ('%*.*' + conversion).encode(), [width, precision, 0x1234]
    for fmt in [b'%s', b'%.0s', b'%.3s', b'%8s', b'%-8s', b'%8.3s']:
        for value in [b'\0', b'abcdef\0']:
            yield fmt, [value]
    for fmt in [b'%#s', b'%#.3s', b'%#8s']:
        yield fmt, [b'\x06abcdef']
    for modifier in ['', 'h', 'l']:
        yield ('abc%' + modifier + 'n:%x').encode(), [b'\xA5' * 8, 0xBEEF]
    yield b'%d/%s/%X/%u', [0xFFFFFFFF, b'hello\0', 0xCAFE, 42]


def execute(exe, fmt, arguments, patches):
    uc = Uc(UC_ARCH_MIPS, UC_MODE_MIPS32 | UC_MODE_LITTLE_ENDIAN)
    uc.mem_map(0, 0x200000)

    def bios_call(cpu, address, size, data):
        service = cpu.reg_read(UC_MIPS_REG_T1)
        source = cpu.reg_read(UC_MIPS_REG_A0)
        if service == 0x1B:  # BIOS strlen, reached through the retail thunk.
            result = 0
            while cpu.mem_read((source + result) & 0x1FFFFFFF, 1)[0]:
                result += 1
                if result > 0x1000:
                    raise RuntimeError('unterminated string passed to BIOS strlen')
        elif service == 0x2E:  # BIOS memchr.
            byte = cpu.reg_read(UC_MIPS_REG_A1) & 0xFF
            count = cpu.reg_read(UC_MIPS_REG_A2)
            if count > 0x1000:
                raise RuntimeError('unexpected BIOS memchr length')
            found = bytes(cpu.mem_read(source & 0x1FFFFFFF, count)).find(bytes([byte])) if count else -1
            result = source + found if found >= 0 else 0
        else:
            raise RuntimeError(f'unexpected BIOS A0 service {service:x}')
        cpu.reg_write(UC_MIPS_REG_V0, result)
        cpu.reg_write(UC_MIPS_REG_PC, cpu.reg_read(UC_MIPS_REG_RA))

    uc.hook_add(UC_HOOK_CODE, bios_call, begin=0xA0, end=0xA0)
    address, size = struct.unpack_from('<II', exe, 0x18)
    uc.mem_write(address & 0x1FFFFFFF, exe[0x800:0x800 + size])
    for address, data in patches:
        uc.mem_write(address & 0x1FFFFFFF, data)
    uc.mem_write(0x100000, fmt + b'\0')
    uc.mem_write(0x110000, b'\xCC' * 0x800)
    values = []
    for i, value in enumerate(arguments):
        if isinstance(value, bytes):
            address = 0x120000 + i * 0x100
            uc.mem_write(address, value)
            value = address | 0x80000000
        values.append(value & 0xFFFFFFFF)
    uc.reg_write(UC_MIPS_REG_A0, 0x80110010)
    uc.reg_write(UC_MIPS_REG_A1, 0x80100000)
    for register, value in zip([UC_MIPS_REG_A2, UC_MIPS_REG_A3], values):
        uc.reg_write(register, value)
    for i, value in enumerate(values[2:]):
        uc.mem_write((STACK & 0x1FFFFFFF) + 16 + i * 4, struct.pack('<I', value))
    uc.reg_write(UC_MIPS_REG_SP, STACK)
    uc.reg_write(UC_MIPS_REG_RA, RETURN)
    try:
        uc.emu_start(ENTRY, RETURN, count=200000)
    except UcError as exc:
        raise RuntimeError(f"CPU error at {uc.reg_read(UC_MIPS_REG_PC):08x} for {fmt!r}, {arguments!r}") from exc
    if uc.reg_read(UC_MIPS_REG_PC) != RETURN:
        raise RuntimeError('formatter did not return within instruction budget')
    return (uc.reg_read(UC_MIPS_REG_V0), bytes(uc.mem_read(0x110000, 0x800)),
            bytes(uc.mem_read(0x120000, 0x1000)))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('object', type=Path)
    parser.add_argument('--exe', type=Path, default=Path('build/USA/main.exe'))
    args = parser.parse_args()
    exe = args.exe.read_bytes()
    if hashlib.sha1(exe).hexdigest() != RETAIL_SHA1:
        parser.error('reference EXE does not match the USA retail SHA-1')
    with tempfile.TemporaryDirectory(prefix='sprintf-behavior-') as temp:
        script, linked = Path(temp) / 'link.ld', Path(temp) / 'candidate.elf'
        layout, owned = candidate_layout(args.object)
        script.write_text(layout)
        subprocess.run(['mipsel-none-elf-ld', '-T', str(script), str(args.object),
                        '-o', str(linked)], check=True)
        with linked.open('rb') as stream:
            elf = ELFFile(stream)
            # Match the production linker's SUBALIGN(4), including objects
            # whose assembler declares a larger input-section alignment.
            entry = elf.get_section_by_name('.symtab').get_symbol_by_name('Square_Vsprintf')
            if not entry or entry[0]['st_value'] != ENTRY:
                raise ValueError('candidate entry was not linked at the retail address')
            for name in owned:
                symbol = elf.get_section_by_name('.symtab').get_symbol_by_name(name)[0]
                address = int(name[2:], 16)
                if symbol['st_value'] != address:
                    raise ValueError(f'{name} was not linked at its retail address')
                section = elf.get_section(symbol['st_shndx'])
                offset = address - section['sh_addr']
                size = 12 if name == 'D_80094528' else 17
                actual_data = section.data()[offset:offset + size]
                retail_offset = address - 0x8000F800
                if actual_data != exe[retail_offset:retail_offset + size]:
                    raise ValueError(f'{name} initializer differs from retail')
            patches = [(s['sh_addr'], s.data()) for s in elf.iter_sections()
                       if s['sh_flags'] & 2 and s['sh_size']]
        for count, (fmt, arguments) in enumerate(cases(), 1):
            expected = execute(exe, fmt, arguments, [])
            actual = execute(exe, fmt, arguments, patches)
            if actual != expected:
                print(f'FAIL case {count}: {fmt!r}, arguments={arguments!r}')
                print(f'retail return/output: {expected[0]}, {expected[1][16:80]!r}')
                print(f'candidate return/output: {actual[0]}, {actual[1][16:80]!r}')
                raise SystemExit(1)
        print(f'PASS: {count} cases agree with retail return values, output buffers and argument memory')


if __name__ == '__main__':
    main()
