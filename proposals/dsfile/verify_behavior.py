#!/usr/bin/env python3
"""Compare the three dsfile functions with retail MIPS using synthetic sectors."""
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

RETAIL_SHA1 = '452fb033f2eaa4b18aa20a5bca60b8125af3a37b'
EXE = Path('build/USA/main.exe').read_bytes()
assert hashlib.sha1(EXE).hexdigest() == RETAIL_SHA1
SYMS = {name: int(address, 16) for name, address in re.findall(
    r'^(\w+) = (0x[0-9A-Fa-f]+);', Path('configs/USA/sym.main.txt').read_text(), re.M)}
OBJECT = Path(sys.argv[1]).resolve()
WORK = tempfile.TemporaryDirectory(prefix='dsfile-behavior-')
undefined = subprocess.check_output(['mipsel-none-elf-nm', '-u', str(OBJECT)], text=True).split()[1::2]
def symbol(name):
    return int(name[2:], 16) if name.startswith('D_') else SYMS[name]
script = '\n'.join(f'{name} = {symbol(name):#x};' for name in undefined)
script += '\nSECTIONS { .text 0x80081414 : { *(.text) } /DISCARD/ : { *(.reginfo) *(.MIPS.abiflags) *(.pdr) *(.comment) *(.gnu.attributes) } }'
link_script = Path(WORK.name) / 'candidate.ld'
linked = Path(WORK.name) / 'candidate.elf'
link_script.write_text(script)
subprocess.run(['mipsel-none-elf-ld', '-T', str(link_script), str(OBJECT), '-o', str(linked)], check=True)
with linked.open('rb') as stream:
    elf = ELFFile(stream)
    PATCHES = [(s['sh_addr'], s.data()) for s in elf.iter_sections() if s['sh_flags'] & 2 and s['sh_size']]
    ENTRIES = {s.name: s['st_value'] for s in elf.get_section_by_name('.symtab').iter_symbols()}


def path_table(count):
    result = bytearray()
    for index in range(count):
        name = b'\0' if index == 0 else f'D{index}'.encode()
        result += bytes([len(name), 0]) + struct.pack('<IH', 200 + index, 1 + index % 7) + name
        if len(name) & 1:
            result += b'\0'
    return bytes(result[:2048]).ljust(2048, b'\0')


def directory_sector(count):
    if count == -1:
        # Deliberately tiny overlapping records exercise the 64-entry cap.
        return b'\x01' * 2048
    result = bytearray()
    for index in range(count):
        name = bytes([index]) if index < 2 else f'FILE{index};1'.encode()
        length = 33 + len(name) + (not (len(name) & 1))
        record = bytearray(length)
        record[0] = length
        struct.pack_into('<I', record, 2, 300 + index)
        struct.pack_into('<I', record, 10, 2048 + index)
        record[32] = len(name)
        record[33:33 + len(name)] = name
        if len(result) + len(record) > 2048:
            break
        result += record
    return bytes(result).ljust(2048, b'\0')


def execute(function, payload, arguments, debug, failure, cached, patches):
    cpu = Uc(UC_ARCH_MIPS, UC_MODE_MIPS32 | UC_MODE_LITTLE_ENDIAN)
    cpu.mem_map(0, 0x200000)
    start, size = struct.unpack_from('<II', EXE, 0x18)
    cpu.mem_write(start & 0x1FFFFFFF, EXE[0x800:0x800 + size])
    for address, data in patches:
        cpu.mem_write(address & 0x1FFFFFFF, data)
    cpu.mem_write(0xA36B0, b'\xA5' * (64 * 24))
    cpu.mem_write(0xA52B0, b'\xA5' * 2048)
    for index in range(128):
        name = b'' if index == 0 else f'D{index}'.encode()
        cpu.mem_write(0xA3CB0 + index * 44,
                      struct.pack('<III', index + 1, 1 + index % 7, 200 + index) + name.ljust(32, b'\0'))
    cpu.mem_write(0x9B6DC, struct.pack('<I', cached))
    cpu.mem_write(0x9AFC0, struct.pack('<I', debug))
    cpu.mem_write(0x100000, arguments[1] + b'\0' if len(arguments) > 1 else b'\0')
    calls = []
    read_count = 0

    def string(address, limit=256):
        result = bytearray()
        for _ in range(limit):
            value = cpu.mem_read(address & 0x1FFFFFFF, 1)[0]
            if not value:
                return bytes(result)
            result.append(value)
            address += 1
        raise AssertionError('unterminated string')

    def helper(current, address, size, function_name):
        nonlocal read_count
        args = [current.reg_read(reg) for reg in
                (UC_MIPS_REG_A0, UC_MIPS_REG_A1, UC_MIPS_REG_A2, UC_MIPS_REG_A3)]
        if function_name == 'ds_read':
            read_count += 1
            calls.append((function_name, args[0], args[1], args[2]))
            result = 0 if failure == read_count else 1
            if result:
                data = payload
                if function == 'DS_newmedia' and read_count == 1:
                    data = bytearray(2048)
                    data[0:7] = b'\x01BAD00\x01' if failure == 3 else b'\x01CD001\x01'
                    struct.pack_into('<I', data, 140, 1234)
                current.mem_write(args[2] & 0x1FFFFFFF, bytes(data))
        elif function_name == 'memcpy':
            assert args[2] <= 2048
            current.mem_write(args[0] & 0x1FFFFFFF, bytes(current.mem_read(args[1] & 0x1FFFFFFF, args[2])))
            result = args[0]
        elif function_name in ('strcmp', 'strncmp'):
            if function_name == 'strncmp':
                left = bytes(current.mem_read(args[0] & 0x1FFFFFFF, args[2])).split(b'\0')[0]
                right = bytes(current.mem_read(args[1] & 0x1FFFFFFF, args[2])).split(b'\0')[0]
            else:
                left, right = string(args[0]), string(args[1])
            result = (left > right) - (left < right)
        elif function_name == 'puts':
            calls.append((function_name, string(args[0])))
            result = 0
        else:
            fmt = string(args[0])
            values = args[1:] + list(struct.unpack('<II', current.mem_read((current.reg_read(UC_MIPS_REG_SP) + 16) & 0x1FFFFFFF, 8)))
            conversions = re.findall(rb'%(?:[-+ #0]*[0-9.]*)?([sdiuxX])', fmt)
            values = tuple(string(value) if kind == b's' else value for kind, value in zip(conversions, values))
            calls.append((function_name, fmt, values))
            result = 0
        current.reg_write(UC_MIPS_REG_V0, result & 0xFFFFFFFF)
        current.reg_write(UC_MIPS_REG_PC, current.reg_read(UC_MIPS_REG_RA))

    for name in ('ds_read', 'memcpy', 'strcmp', 'strncmp', 'printf', 'puts'):
        address = ENTRIES.get(name, SYMS[name]) if patches else SYMS[name]
        cpu.hook_add(UC_HOOK_CODE, helper, user_data=name, begin=address, end=address)
    cpu.reg_write(UC_MIPS_REG_A0, arguments[0] if arguments else 0)
    cpu.reg_write(UC_MIPS_REG_A1, 0x80100000)
    cpu.reg_write(UC_MIPS_REG_SP, 0x801FF000)
    cpu.reg_write(UC_MIPS_REG_RA, 0x8000F000)
    cpu.emu_start(ENTRIES[function] if patches else SYMS[function], 0x8000F000, count=500000)
    assert cpu.reg_read(UC_MIPS_REG_PC) == 0x8000F000, 'instruction limit reached'
    return (cpu.reg_read(UC_MIPS_REG_V0), bytes(cpu.mem_read(0xA36B0, 0x2400)),
            bytes(cpu.mem_read(0x9B6DC, 4)), calls)


def cases():
    for count, failure, debug in itertools.product((0, 1, 2, 127, 128, 180), range(4), range(3)):
        yield 'DS_newmedia', path_table(count), (), debug, failure, 42
    for count, directory, state, debug in itertools.product((0, 1, 2, 20, 64, -1), (1, 2, 128), range(3), range(3)):
        yield 'DS_cachefile', directory_sector(count), (directory,), debug, int(state == 1), directory if state == 2 else 0
    for parent, name in itertools.product((0, 1, 2, 7, 128), (b'', b'D1', b'D7', b'D127', b'MISSING')):
        yield 'DS_searchdir', bytes(2048), (parent, name), 0, 0, 0


for count, case in enumerate(cases(), 1):
    expected = execute(*case, [])
    actual = execute(*case, PATCHES)
    if actual != expected:
        print('FAIL', count, case[0], case[2:])
        print('return:', expected[0], actual[0], 'cached directory:', expected[2], actual[2])
        print('calls:', expected[3], actual[3])
        for index, (left, right) in enumerate(zip(expected[1], actual[1])):
            if left != right:
                print('first cache difference:', hex(0x800A36B0 + index), left, right)
                break
        raise SystemExit(1)
print(f'PASS {count} cases: return values, complete caches, sector buffer, cached directory and call traces')
