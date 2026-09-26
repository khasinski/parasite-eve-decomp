"""Retail bytes and scripted DMA/GPU polling model; external calls are stubs."""
from pathlib import Path
import shutil
import struct
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[2]


@unittest.skipUnless(shutil.which('mipsel-none-elf-ld') and
                     (ROOT/'assets/USA/main.exe').is_file() and
                     (ROOT/'build/USA/main.elf').is_file(), 'retail/toolchain/build unavailable')
class GpuDmaTransferTests(unittest.TestCase):
    def test_retail_bytes_and_polling_model(self):
        try:
            from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN, UC_HOOK_CODE, UC_HOOK_MEM_READ
            from unicorn import mips_const as R
        except ImportError:
            self.skipTest('unicorn unavailable')
        symbols = {}
        for line in subprocess.check_output(['mipsel-none-elf-nm', str(ROOT/'build/USA/main.elf')], text=True).splitlines():
            fields = line.split()
            if len(fields) == 3:
                symbols[fields[2]] = int(fields[0], 16)
        entry, stop, stack = 0x800775E8, 0x80010000, 0x801F0000
        retail = (ROOT/'assets/USA/main.exe').read_bytes()[0x67DE8:0x68200]
        with tempfile.TemporaryDirectory() as directory:
            work = Path(directory)
            subprocess.run([str(ROOT/'tools/scripts/cc.sh'), str(ROOT/'src/main/gpu/dma_transfer.c'), str(work/'test.o')], check=True, capture_output=True)
            needed = [line.split()[-1] for line in subprocess.check_output(['mipsel-none-elf-nm', '-u', str(work/'test.o')], text=True).splitlines()]
            script = 'SECTIONS { .text 0x800775E8 : SUBALIGN(4) { *(.text) } /DISCARD/ : { *(.reginfo) *(.mdebug) *(.pdr) *(.MIPS.abiflags) } }\n'
            (work/'test.ld').write_text(script + '\n'.join(f'{name} = 0x{symbols[name]:X};' for name in needed))
            subprocess.run(['mipsel-none-elf-ld', '-EL', '-T', str(work/'test.ld'), str(work/'test.o'), '-o', str(work/'test.elf')], check=True, capture_output=True)
            subprocess.run(['mipsel-none-elf-objcopy', '-O', 'binary', '-j', '.text', str(work/'test.elf'), str(work/'test.bin')], check=True, capture_output=True)
            compiled = (work/'test.bin').read_bytes()
        self.assertEqual(compiled, retail)
        rect_addr, data_addr, table = 0x80100000, 0x80101000, 0x80102000
        dma, gpu = 0x80180000, 0x80180004
        load_cb, store_cb, move_cb, debug_cb = 0x80010100, 0x80010110, 0x80010120, 0x80010130
        callbacks = {load_cb: ('load', 2), store_cb: ('store', 2), move_cb: ('move', 1), debug_cb: ('debug', 2)}
        external = {symbols[name]: name for name in ('checkRECT', 'VSync', 'Gpu_DmaTimeoutCheck', 'DMACallback')}
        # Each unsuccessful poll advances only through the timeout callback.
        sequences = [[], [(1, 1)], [(0, 0)], [(1, 0), (0, 0), (1, 1)], [(0, 0)]*4]
        for function in ('LoadImage2', 'StoreImage2', 'MoveImage2', 'Gpu_DmaTransfer'):
            for sequence in sequences:
                for timeout_at in (0, 1, 3):
                    for shape in range(4):
                        rect = struct.pack('<hhhh', -7, 23, (0, 320, -1, 1)[shape], (224, 0, -2, 1)[shape])
                        x, y = 0xFFFF8123, 0x1234FEDC
                        debug = shape
                        expected = []
                        if function != 'Gpu_DmaTransfer':
                            label = {'LoadImage2': 'D_800119BC', 'StoreImage2': 'D_800118E0', 'MoveImage2': 'D_800118EC'}[function]
                            expected.append(('checkRECT', symbols[label], rect_addr))
                        elif debug >= 2:
                            expected.append(('debug', symbols['D_80011928'], data_addr))
                        expected.append(('VSync', 0xFFFFFFFF))
                        failed = False
                        for index, (busy, ready) in enumerate(sequence + [(0, 1)]):
                            expected.append(('dma', busy))
                            if not busy:
                                expected.append(('gpu', ready))
                            if not busy and ready:
                                break
                            expected.append(('timeout',))
                            if timeout_at and index + 1 == timeout_at:
                                failed = True
                                break
                        packet = bytes(12)
                        result = 0xFFFFFFFF if failed else 0
                        if not failed:
                            expected.append(('DMACallback', 2, symbols['Gpu_RestoreDmaCallback']))
                            if function == 'MoveImage2':
                                if shape < 2:
                                    result = 0xFFFFFFFF
                                else:
                                    packet = rect[:4] + struct.pack('<I', ((y << 16) | (x & 0xFFFF)) & 0xFFFFFFFF) + rect[4:]
                                    expected.append(('move', symbols['D_800957EC'] - 8))
                            elif function == 'Gpu_DmaTransfer':
                                expected.append(('move', data_addr))
                            else:
                                expected.append(('load' if function == 'LoadImage2' else 'store', rect_addr, data_addr))
                        for body in (retail, compiled):
                            machine = Uc(UC_ARCH_MIPS, UC_MODE_MIPS32 | UC_MODE_LITTLE_ENDIAN)
                            machine.mem_map(0, 0x200000)
                            def put(address, value):
                                machine.mem_write(address & 0x1FFFFFFF, value)
                            def word(address, value):
                                put(address, struct.pack('<I', value & 0xFFFFFFFF))
                            put(entry, body)
                            put(rect_addr, rect)
                            word(symbols['g_GpuDmaChcrPtr'], dma)
                            word(symbols['g_GpuGp1Ptr'], gpu)
                            word(symbols['D_80095744'], table)
                            word(table + 0x18, move_cb)
                            word(table + 0x1C, store_cb)
                            word(table + 0x20, load_cb)
                            word(symbols['D_80095748'], debug_cb)
                            put(symbols['D_8009574C'] + 2, bytes([debug]))
                            word(symbols['g_GpuDmaWaitLoopCounter'], 99)
                            machine.reg_write(R.UC_MIPS_REG_A0, data_addr if function == 'Gpu_DmaTransfer' else rect_addr)
                            machine.reg_write(R.UC_MIPS_REG_A1, x if function == 'MoveImage2' else data_addr)
                            machine.reg_write(R.UC_MIPS_REG_A2, y)
                            machine.reg_write(R.UC_MIPS_REG_SP, stack)
                            machine.reg_write(R.UC_MIPS_REG_RA, stop)
                            for i in range(8):
                                machine.reg_write(getattr(R, f'UC_MIPS_REG_S{i}'), 0xABCD0000+i)
                            events, poll = [], [0]
                            def read_hook(m, access, address, size, value, user):
                                physical = address & 0x1FFFFFFF
                                if physical not in (dma & 0x1FFFFFFF, gpu & 0x1FFFFFFF):
                                    return
                                busy, ready = sequence[poll[0]] if poll[0] < len(sequence) else (0, 1)
                                is_dma = physical == dma & 0x1FFFFFFF
                                events.append(('dma' if is_dma else 'gpu', busy if is_dma else ready))
                                word(address, (busy << 24) if is_dma else (ready << 26))
                            def call_hook(m, address, size, user):
                                name = external.get(address)
                                if name is None and address not in callbacks:
                                    return
                                args = [m.reg_read(getattr(R, f'UC_MIPS_REG_A{i}')) for i in range(2)]
                                value = 0
                                if address in callbacks:
                                    label, arity = callbacks[address]
                                    events.append((label, *args[:arity]))
                                    value = 0xBADCAFE
                                elif name == 'Gpu_DmaTimeoutCheck':
                                    events.append(('timeout',))
                                    poll[0] += 1
                                    value = int(bool(timeout_at) and poll[0] == timeout_at)
                                else:
                                    events.append((name, *args[:1 if name == 'VSync' else 2]))
                                    if name == 'VSync':
                                        value = 1234
                                for register in ('V1', 'A0', 'A1', 'A2', 'A3', 'T0', 'T1', 'T2', 'T3', 'T4', 'T5', 'T6', 'T7', 'T8', 'T9'):
                                    m.reg_write(getattr(R, 'UC_MIPS_REG_' + register), 0xCAFE0000)
                                m.reg_write(R.UC_MIPS_REG_V0, value)
                                m.reg_write(R.UC_MIPS_REG_PC, m.reg_read(R.UC_MIPS_REG_RA))
                            machine.hook_add(UC_HOOK_MEM_READ, read_hook)
                            machine.hook_add(UC_HOOK_CODE, call_hook)
                            machine.emu_start(symbols[function], stop, count=4000)
                            self.assertEqual(machine.reg_read(R.UC_MIPS_REG_PC), stop)
                            self.assertEqual(events, expected, (function, sequence, timeout_at, shape))
                            self.assertEqual(machine.reg_read(R.UC_MIPS_REG_V0), result)
                            self.assertEqual(machine.reg_read(R.UC_MIPS_REG_SP), stack)
                            for i in range(8):
                                self.assertEqual(machine.reg_read(getattr(R, f'UC_MIPS_REG_S{i}')), 0xABCD0000+i)
                            self.assertEqual(bytes(machine.mem_read(rect_addr & 0x1FFFFFFF, 8)), rect)
                            self.assertEqual(bytes(machine.mem_read(symbols['D_800957EC'] & 0x1FFFFFFF, 12)), packet)
                            for name, value in (('g_GpuDmaTimeoutDeadline', 1474), ('g_GpuDmaWaitLoopCounter', 0)):
                                self.assertEqual(bytes(machine.mem_read(symbols[name] & 0x1FFFFFFF, 4)), struct.pack('<I', value))
