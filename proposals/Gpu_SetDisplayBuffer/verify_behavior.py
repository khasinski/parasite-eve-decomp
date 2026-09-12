#!/usr/bin/env python3
"""Compare queue draining with retail, including callbacks, DMA starts and wraparound."""
import argparse
import hashlib
from itertools import product
from pathlib import Path
import struct
import subprocess
import tempfile

from elftools.elf.elffile import ELFFile
from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN, UC_HOOK_CODE, UC_HOOK_MEM_READ
from unicorn.mips_const import UC_MIPS_REG_A0, UC_MIPS_REG_A1, UC_MIPS_REG_SP, UC_MIPS_REG_RA, UC_MIPS_REG_PC, UC_MIPS_REG_V0

ENTRY, RETURN = 0x80076EE4, 0x8000F000
MASK_CALL, DMA_CALL = 0x80073E10, 0x80073CF4
JOB, DONE = 0x80180000, 0x80180010
CHCR, GP1 = 0x80190000, 0x80190004
LINK = '''
D_80095860 = 0x80095860; SetIntrMask = 0x80073E10;
D_80095874 = 0x80095874; D_80095878 = 0x80095878;
D_80095880 = 0x80095880; D_8009574C = 0x8009574C;
DMACallback = 0x80073CF4; D_80095854 = 0x80095854;
D_800BD030 = 0x800BD030;
SECTIONS { .text 0x80076EE4 : SUBALIGN(4) { *(.text .text.*) }
/DISCARD/ : { *(.reginfo) *(.mdebug) } }
'''


def execute(exe, code, case):
    tail, length, callback, pending, busy, stop_after, delay = case
    cpu = Uc(UC_ARCH_MIPS, UC_MODE_MIPS32 | UC_MODE_LITTLE_ENDIAN)
    cpu.mem_map(0, 0x200000)
    address, size = struct.unpack_from('<II', exe, 0x18)
    cpu.mem_write(address & 0x1FFFFFFF, exe[0x800:0x800 + size])
    if code is not None:
        cpu.mem_write(ENTRY & 0x1FFFFFFF, code)

    def put(address, value):
        cpu.mem_write(address & 0x1FFFFFFF, struct.pack('<I', value))

    def get(address):
        return struct.unpack('<I', cpu.mem_read(address & 0x1FFFFFFF, 4))[0]

    for address, value in [(0x80095860, CHCR), (0x80095854, GP1),
                           (CHCR, busy << 24), (GP1, 0 if delay else 0x04000000),
                           (0x80095874, (tail + length) & 63), (0x80095878, tail),
                           (0x80095754, pending), (0x80095758, DONE if callback else 0)]:
        put(address, value)
    for index in range(64):
        for offset, value in [(0, JOB), (4, 0xA0000000 + index), (8, 0xB0000000 + index)]:
            put(0x800BD030 + index * 96 + offset, value)
    trace = []
    state = {'mask': 0x55, 'jobs': 0, 'reads': 0}

    def hook(cpu, address, size, _):
        if address == MASK_CALL:
            value = cpu.reg_read(UC_MIPS_REG_A0)
            trace.append(('mask', value))
            cpu.reg_write(UC_MIPS_REG_V0, state['mask'])
            state['mask'] = value
        elif address == DMA_CALL:
            trace.append(('dma', cpu.reg_read(UC_MIPS_REG_A0), cpu.reg_read(UC_MIPS_REG_A1)))
        elif address == JOB:
            trace.append(('job', cpu.reg_read(UC_MIPS_REG_A0), cpu.reg_read(UC_MIPS_REG_A1)))
            state['jobs'] += 1
            if state['jobs'] == stop_after:
                put(CHCR, 0x01000000)
        elif address == DONE:
            trace.append(('done', get(0x80095754), state['mask']))
        else:
            return
        cpu.reg_write(UC_MIPS_REG_PC, cpu.reg_read(UC_MIPS_REG_RA))

    def read(cpu, access, address, size, value, _):
        if address & 0x1FFFFFFF == GP1 & 0x1FFFFFFF:
            state['reads'] += 1
            if state['reads'] >= delay:
                put(GP1, 0x04000000)

    cpu.hook_add(UC_HOOK_CODE, hook)
    cpu.hook_add(UC_HOOK_MEM_READ, read)
    cpu.reg_write(UC_MIPS_REG_SP, 0x801FF000)
    cpu.reg_write(UC_MIPS_REG_RA, RETURN)
    cpu.emu_start(ENTRY, RETURN, count=100000)
    assert cpu.reg_read(UC_MIPS_REG_PC) == RETURN, ('did not return', case)
    assert cpu.reg_read(UC_MIPS_REG_SP) == 0x801FF000
    consumed = 0 if busy else min(length, stop_after) if stop_after else length
    assert state['jobs'] == consumed
    assert get(0x80095878) == (tail + consumed) & 63
    assert state['mask'] == 0x55
    assert cpu.reg_read(UC_MIPS_REG_V0) == (1 if busy else length - consumed)
    expected_done = bool(not busy and callback and pending and consumed == length and
                         not (stop_after and consumed == stop_after))
    assert any(event[0] == 'done' for event in trace) == expected_done
    assert get(0x80095754) == (0 if expected_done else pending)
    return (cpu.reg_read(UC_MIPS_REG_V0), trace,
            bytes(cpu.mem_read(0x9574C, 16)), bytes(cpu.mem_read(0x95874, 16)),
            bytes(cpu.mem_read(0xBD030, 0x1800)))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('object', type=Path)
    parser.add_argument('--retail', type=Path, default=Path('assets/USA/main.exe'))
    args = parser.parse_args()
    exe = args.retail.read_bytes()
    assert hashlib.sha1(exe).hexdigest() == '452fb033f2eaa4b18aa20a5bca60b8125af3a37b'
    with tempfile.TemporaryDirectory() as directory:
        work = Path(directory)
        (work / 'link.ld').write_text(LINK)
        subprocess.run(['mipsel-none-elf-ld', '-T', str(work / 'link.ld'),
                        str(args.object), '-o', str(work / 'code.elf')], check=True)
        with (work / 'code.elf').open('rb') as stream:
            code = ELFFile(stream).get_section_by_name('.text').data()
    for count, case in enumerate(product([0, 1, 31, 63], [0, 1, 2, 31, 63],
                                         [0, 1], [0, 1], [0, 1], [0, 1, 2], [0, 3]), 1):
        assert execute(exe, None, case) == execute(exe, code, case), case
    print(f'PASS: {count} retail-oracle cases, callbacks, queue state, DMA and return values')


if __name__ == '__main__':
    main()
