"""Retail byte/behavior checks, including state-changing queue callbacks."""
from pathlib import Path
import itertools
import shutil
import struct
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[2]


@unittest.skipUnless(shutil.which('mipsel-none-elf-ld') and (ROOT/'assets/USA/main.exe').is_file(), 'retail/toolchain unavailable')
class GpuQueueRetailTests(unittest.TestCase):
    def test_bytes_and_callback_state_model(self):
        try:
            from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN, UC_HOOK_CODE
            from unicorn import mips_const as R
        except ImportError:
            self.skipTest('unicorn unavailable')
        entry, stop, state = 0x80074C14, 0x80010000, 0x8009574C
        print_cb, reset_cb, table = 0x80010100, 0x80010110, 0x80100000
        dma_cb, format_addr = 0x80073CF4, 0x80011840
        symbols = dict(D_8009574C=state, D_80095744=0x80095744,
                       D_80095748=0x80095748, D_80011840=format_addr, DMACallback=dma_cb)
        retail = (ROOT/'assets/USA/main.exe').read_bytes()[0x65414:0x654B8]
        with tempfile.TemporaryDirectory() as directory:
            work = Path(directory)
            subprocess.run([str(ROOT/'tools/scripts/cc.sh'), str(ROOT/'src/main/gpu/SetGraphQueue.c'), str(work/'test.o')], check=True, capture_output=True)
            script = 'SECTIONS { .text 0x80074C14 : SUBALIGN(4) { *(.text) } /DISCARD/ : { *(.reginfo) *(.mdebug) *(.pdr) *(.MIPS.abiflags) } }\n'
            (work/'test.ld').write_text(script+'\n'.join(f'{name} = 0x{address:X};' for name,address in symbols.items()))
            subprocess.run(['mipsel-none-elf-ld', '-EL', '-T', str(work/'test.ld'), str(work/'test.o'), '-o', str(work/'test.elf')], check=True, capture_output=True)
            subprocess.run(['mipsel-none-elf-objcopy', '-O', 'binary', '-j', '.text', str(work/'test.elf'), str(work/'test.bin')], check=True, capture_output=True)
            compiled = (work/'test.bin').read_bytes()
        self.assertEqual(compiled, retail)
        modes = (-2147483648, -1, 0, 1, 2, 255, 256, 300, 2147483647)
        for old, mode, debug, mutation, mutate_dma in itertools.product((0, 1, 2, 255), modes, (0, 1, 2, 255), range(3), (False, True)):
            initial = bytearray(range(16))
            initial[1], initial[2] = old, debug
            expected = initial.copy()
            events = []
            if debug >= 2:
                events.append(('print', format_addr, mode & 0xFFFFFFFF, old))
                if mutation:
                    expected[1] = mode & 255 if mutation == 1 else 73
            if mode != expected[1]:
                events.append(('reset', 1, expected[1]))
                # The production store must supersede a queue change in reset.
                expected[1] = mode & 255
                events.append(('dma', 2, 0, expected[1]))
                if mutate_dma:
                    expected[1] = 91
            for body in (retail, compiled):
                machine = Uc(UC_ARCH_MIPS, UC_MODE_MIPS32 | UC_MODE_LITTLE_ENDIAN)
                machine.mem_map(0, 0x200000)
                def put(address, value): machine.mem_write(address & 0x1FFFFFFF, bytes(value))
                def word(address, value): put(address, struct.pack('<I', value))
                def queue(): return machine.mem_read((state+1) & 0x1FFFFFFF, 1)[0]
                put(entry, body)
                put(state, initial)
                word(symbols['D_80095744'], table)
                word(symbols['D_80095748'], print_cb)
                word(table+0x34, reset_cb)
                machine.reg_write(R.UC_MIPS_REG_A0, mode & 0xFFFFFFFF)
                machine.reg_write(R.UC_MIPS_REG_RA, stop)
                machine.reg_write(R.UC_MIPS_REG_SP, 0x801F0000)
                for i in range(8): machine.reg_write(getattr(R, f'UC_MIPS_REG_S{i}'), 0xABCD0000+i)
                actual = []
                def hook(m, address, size, user):
                    if address not in (print_cb, reset_cb, dma_cb): return
                    a0, a1 = m.reg_read(R.UC_MIPS_REG_A0), m.reg_read(R.UC_MIPS_REG_A1)
                    if address == print_cb:
                        actual.append(('print', a0, a1, queue()))
                        if mutation: put(state+1, bytes([mode & 255 if mutation == 1 else 73]))
                    elif address == reset_cb:
                        actual.append(('reset', a0, queue()))
                        put(state+1, bytes([64]))
                    else:
                        actual.append(('dma', a0, a1, queue()))
                        if mutate_dma: put(state+1, bytes([91]))
                    for name in ('V0', 'V1', 'A0', 'A1', 'A2', 'A3', 'T0', 'T1', 'T2', 'T3', 'T4', 'T5', 'T6', 'T7', 'T8', 'T9'):
                        m.reg_write(getattr(R, 'UC_MIPS_REG_'+name), 0xBADCAFE)
                    m.reg_write(R.UC_MIPS_REG_PC, m.reg_read(R.UC_MIPS_REG_RA))
                machine.hook_add(UC_HOOK_CODE, hook)
                machine.emu_start(entry, stop, count=300)
                self.assertEqual(machine.reg_read(R.UC_MIPS_REG_PC), stop)
                self.assertEqual(actual, events, (old, mode, debug, mutation, mutate_dma))
                self.assertEqual(machine.reg_read(R.UC_MIPS_REG_V0), old)
                self.assertEqual(machine.reg_read(R.UC_MIPS_REG_SP), 0x801F0000)
                for i in range(8):
                    self.assertEqual(machine.reg_read(getattr(R, f'UC_MIPS_REG_S{i}')), 0xABCD0000+i)
                self.assertEqual(bytes(machine.mem_read(state & 0x1FFFFFFF, 16)), expected)
