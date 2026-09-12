#!/usr/bin/env python3
"""Retail comparison of GPU submission, queue-full waits, packet copies and publication."""
import argparse
import hashlib
from itertools import product
from pathlib import Path
import struct
import subprocess
import tempfile

from elftools.elf.elffile import ELFFile
from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN, UC_HOOK_CODE, UC_HOOK_MEM_READ
from unicorn.mips_const import UC_MIPS_REG_A0, UC_MIPS_REG_A1, UC_MIPS_REG_A2, UC_MIPS_REG_A3, UC_MIPS_REG_SP, UC_MIPS_REG_RA, UC_MIPS_REG_PC, UC_MIPS_REG_V0

ENTRY, RETURN = 0x80076C34, 0x8000F000
MASK, DMA, DRAIN = 0x80073E10, 0x80073CF4, 0x80076EE4
RESET, TIMEOUT = 0x800773D0, 0x80077404
JOB, SOURCE = 0x80180000, 0x80180100
CHCR, GP1 = 0x80190000, 0x80190004
LINK = '''
Gpu_ResetDmaWaitTimer = 0x800773D0; Gpu_DmaTimeoutCheck = 0x80077404;
Gpu_SetDisplayBuffer = 0x80076EE4; SetIntrMask = 0x80073E10;
D_80095874 = 0x80095874; D_80095878 = 0x80095878;
D_8009587C = 0x8009587C; D_8009574C = 0x8009574C;
D_80095860 = 0x80095860; D_80095854 = 0x80095854;
DMACallback = 0x80073CF4; D_800BD030 = 0x800BD030;
SECTIONS { .text 0x80076C34 : SUBALIGN(4) { *(.text .text.*) }
/DISCARD/ : { *(.reginfo) *(.mdebug) } }
'''


def execute(exe, code, case):
    head, occupancy, enabled, busy, callback, size, delay, timeout, drained = case
    cpu = Uc(UC_ARCH_MIPS, UC_MODE_MIPS32 | UC_MODE_LITTLE_ENDIAN)
    cpu.mem_map(0, 0x200000)
    address, length = struct.unpack_from('<II', exe, 0x18)
    cpu.mem_write(address & 0x1FFFFFFF, exe[0x800:0x800 + length])
    if code is not None:
        cpu.mem_write(ENTRY & 0x1FFFFFFF, code)

    def put(address, value):
        cpu.mem_write(address & 0x1FFFFFFF, struct.pack('<I', value))

    def get(address):
        return struct.unpack('<I', cpu.mem_read(address & 0x1FFFFFFF, 4))[0]

    cpu.mem_write(0xBD030, b'\xA5' * 0x1800)
    payload = bytes(range(96))
    cpu.mem_write(SOURCE & 0x1FFFFFFF, payload)
    for address, value in [(0x80095860, CHCR), (0x80095854, GP1),
                           (CHCR, busy << 24), (GP1, 0 if delay else 0x04000000),
                           (0x80095874, head), (0x80095878, (head - occupancy) & 63),
                           (0x80095754, 0), (0x80095758, JOB if callback else 0),
                           (0x8009587C, 0xAABBCCDD)]:
        put(address, value)
    cpu.mem_write(0x9574D, bytes([enabled]))
    state = {'mask': 0x55, 'masks': 0, 'reads': 0, 'waits': 0}
    trace = []

    def snapshot():
        return (get(0x80095874), get(0x80095878), get(0x80095754),
                get(0x8009587C), bytes(cpu.mem_read(0xBD030, 0x1800)))

    def hook(cpu, address, length, _):
        result = 0
        if address == MASK:
            value = cpu.reg_read(UC_MIPS_REG_A0)
            trace.append(('mask', value, snapshot()))
            result, state['mask'] = state['mask'], value
            state['masks'] += 1
        elif address == DMA:
            trace.append(('dma', cpu.reg_read(UC_MIPS_REG_A0), cpu.reg_read(UC_MIPS_REG_A1), snapshot()))
            assert cpu.reg_read(UC_MIPS_REG_A0) == 2
            assert cpu.reg_read(UC_MIPS_REG_A1) == DRAIN
        elif address == RESET:
            trace.append(('reset', snapshot()))
        elif address == TIMEOUT:
            trace.append(('timeout', snapshot()))
            result = timeout
        elif address == DRAIN:
            trace.append(('drain', snapshot()))
            if state['masks'] == 0:
                state['waits'] += 1
                # Leave the ring full on the first pass, free one slot on the second.
                if state['waits'] == 2:
                    put(0x80095878, (get(0x80095878) + 1) & 63)
            elif drained:
                put(0x80095878, get(0x80095874))
        elif address == JOB:
            trace.append(('job', cpu.reg_read(UC_MIPS_REG_A0), cpu.reg_read(UC_MIPS_REG_A1), snapshot()))
            assert cpu.reg_read(UC_MIPS_REG_A0) == SOURCE
            assert cpu.reg_read(UC_MIPS_REG_A1) == 0x12345678
            assert state['mask'] == 0
        else:
            return
        cpu.reg_write(UC_MIPS_REG_V0, result)
        cpu.reg_write(UC_MIPS_REG_PC, cpu.reg_read(UC_MIPS_REG_RA))

    def read(cpu, access, address, length, value, _):
        if address & 0x1FFFFFFF == GP1 & 0x1FFFFFFF:
            state['reads'] += 1
            if state['reads'] >= delay:
                put(GP1, 0x04000000)

    cpu.hook_add(UC_HOOK_CODE, hook)
    cpu.hook_add(UC_HOOK_MEM_READ, read)
    for register, value in [(UC_MIPS_REG_A0, JOB), (UC_MIPS_REG_A1, SOURCE),
                            (UC_MIPS_REG_A2, size & 0xFFFFFFFF), (UC_MIPS_REG_A3, 0x12345678),
                            (UC_MIPS_REG_SP, 0x801FF000), (UC_MIPS_REG_RA, RETURN)]:
        cpu.reg_write(register, value)
    cpu.emu_start(ENTRY, RETURN, count=100000)
    assert cpu.reg_read(UC_MIPS_REG_PC) == RETURN, ('did not return', case)
    assert cpu.reg_read(UC_MIPS_REG_SP) == 0x801FF000
    assert state['mask'] == 0x55
    failed = occupancy == 63 and timeout
    queued = not failed and enabled and (occupancy != 0 or busy or callback)
    assert state['masks'] == (0 if failed else 2)
    assert get(0x80095754) == (0 if failed else 1)
    assert any(event[0] == 'job' for event in trace) == bool(not failed and not queued)
    if queued:
        entry = 0x800BD030 + head * 96
        assert get(entry) == JOB and get(entry + 8) == 0x12345678
        assert get(entry + 4) == (entry + 12 if size else SOURCE)
        copied = max(0, int(size / 4)) * 4
        assert bytes(cpu.mem_read((entry + 12) & 0x1FFFFFFF, 84)) == payload[:copied] + b'\xA5' * (84 - copied)
        expected_queue = bytearray(b'\xA5' * 0x1800)
        struct.pack_into('<III', expected_queue, head * 96, JOB, entry + 12 if size else SOURCE, 0x12345678)
        expected_queue[head * 96 + 12:head * 96 + 12 + copied] = payload[:copied]
        assert bytes(cpu.mem_read(0xBD030, 0x1800)) == expected_queue
        assert get(0x80095874) == (head + 1) & 63
    else:
        assert bytes(cpu.mem_read(0xBD030, 0x1800)) == b'\xA5' * 0x1800
        assert get(0x80095874) == head
    result = cpu.reg_read(UC_MIPS_REG_V0)
    assert result == (0xFFFFFFFF if failed else (get(0x80095874) - get(0x80095878)) & 63 if queued else 0)
    assert bytes(cpu.mem_read(SOURCE & 0x1FFFFFFF, 96)) == payload
    return result, trace, snapshot()


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
    for count, case in enumerate(product([0, 63], [0, 2, 63], [0, 1], [0, 1],
                                         [0, 1], [-4, -1, 0, 1, 3, 4, 8, 84], [0, 3], [0, 1], [0, 1]), 1):
        assert execute(exe, None, case) == execute(exe, code, case), case
    print(f'PASS: {count} retail-oracle cases, queue publication, packet copies, timeout and callbacks')


if __name__ == '__main__':
    main()
