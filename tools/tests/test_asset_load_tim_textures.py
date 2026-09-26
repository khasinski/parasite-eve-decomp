"""Differential execution of the complete TIM loading state machine.

External calls are deterministic stubs. Compare their arguments/order, data
RAM, return values and preserved registers against retail and a state model.
"""
from pathlib import Path
import random
import struct
import subprocess
import tempfile

import unittest
import shutil

ROOT = Path(__file__).resolve().parents[2]

@unittest.skipUnless(shutil.which('mipsel-none-elf-ld') and (ROOT/'assets/USA/main.exe').is_file(), 'retail image/toolchain unavailable')
class AssetLoadTimTexturesTests(unittest.TestCase):
    def test_retail_bytes_and_state_model(self):
        try:
            from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN, UC_HOOK_CODE
            from unicorn import mips_const as R
        except ImportError:
            self.skipTest('unicorn unavailable')
        ENTRY, EXIT, STACK = 0x8006914C, 0x80010000, 0x801F0000
        STATE, FLAGS = 0x800B0CD8, 0x8009D1A0
        SLOTS, SCENE, TIM, TABLE, HANDLERS, IMAGES = (0x80100000, 0x80110000, 0x80120000, 0x80130000, 0x80131000, 0x80140000)
        SYMBOLS = dict(g_GameState=STATE, g_GameStateFlags=FLAGS,
                       g_PmSlotTable=0x800942E4, g_PmSlotTable2=0x800942E8,
                       g_PmCmdHandlerTable=0x800942E0, D_800930E2=0x800930E2,
                       D_800930E4=0x800930E4, Gpu_LoadTimAsset=0x8006E1C0,
                       CdRom_ReadSectorsFromLba=0x8006E6A8, CdRom_PollReady=0x8006E7E8,
                       Asset_FindTable08ByU32Key=0x8006E498, LoadImage=0x8007506C)
        CALLS = {value: name for name, value in SYMBOLS.items() if name in (
            'Gpu_LoadTimAsset', 'CdRom_ReadSectorsFromLba', 'CdRom_PollReady', 'Asset_FindTable08ByU32Key', 'LoadImage')}
        source = 'src/main/asset/Asset_LoadTimTextures.c'
        retail = (ROOT/'assets/USA/main.exe').read_bytes()[0x5994C:0x59D94]
        with tempfile.TemporaryDirectory() as directory:
            work = Path(directory)
            subprocess.run([str(ROOT/'tools/scripts/cc.sh'), str(ROOT/source), str(work/'test.o')], check=True, capture_output=True)
            (work/'test.ld').write_text('SECTIONS { .text 0x8006914C : SUBALIGN(4) { *(.text) } /DISCARD/ : { *(.reginfo) *(.mdebug) *(.pdr) } }\n' + '\n'.join(f'{name} = 0x{address:X};' for name, address in SYMBOLS.items()))
            subprocess.run(['mipsel-none-elf-ld', '-EL', '-T', str(work/'test.ld'), str(work/'test.o'), '-o', str(work/'test.elf')], check=True, capture_output=True)
            subprocess.run(['mipsel-none-elf-objcopy', '-O', 'binary', '-j', '.text', str(work/'test.elf'), str(work/'test.bin')], check=True, capture_output=True)
            compiled = (work/'test.bin').read_bytes()

        self.assertEqual(compiled, retail, 'Production TIM loader must exactly match retail bytes')
        rng = random.Random(ENTRY)
        for case in range(384):
            initial = bytearray(0x200000)
            def put(address, data):
                offset = address & 0x1FFFFFFF
                initial[offset:offset+len(data)] = data
            def word(address, value): put(address, struct.pack('<I', value & 0xFFFFFFFF))
            state = (0, 0x34, 0x35, 0x36, 1, 0xFF)[case % 6]
            force = (0, 1, 0xFFFFFFFF)[case//6 % 3]
            flags = (0, 2, 0x80, 0x82)[case//18 % 4]
            read_result = (-1, 0, 1)[case//7 % 3]
            poll_result = (-1, 0, 1, 2)[case//9 % 4]
            nrecords, nuploads, nimages = case % 8, case//8 % 5, case//40 % 4
            put(STATE, rng.randbytes(0x95C))
            word(STATE, (0xA5A10000 | (8 if case % 4 < 2 else 0)))
            put(STATE+0xEF, bytes([state]))
            word(FLAGS, flags)
            word(STATE+0x188, SLOTS)
            word(STATE+0x18C, SCENE)
            word(STATE+0x194, TIM)
            word(STATE+0x100, 12345)
            put(0x800930E2, struct.pack('<HH', 100, 130))
            put(SLOTS, rng.randbytes(0x7A08))
            word(0x800942E0, TABLE)
            callbacks = {}
            for index in range(86):
                if (index+case) % 3:
                    word(TABLE+4*index, HANDLERS+0x20*index)
                    if (index+case) % 4:
                        address = 0x80020000 + index*16
                        word(HANDLERS+0x20*index, address)
                        callbacks[address] = index
            word(SCENE+4, 0x40)
            word(SCENE+0x44, 0x100 | nrecords << 22)
            for index in range(nrecords):
                put(SCENE+0x100+12*index+7, bytes([(7, 8, 84, 85, 0, 255, 9, 83)[index]]))
            word(TIM+4, 0x40)
            word(TIM+0x68, 0x100 | nuploads << 22)
            for index in range(nimages):
                word(IMAGES+index*0x100+4, (index*73 << 24) | 0x20)
                word(IMAGES+index*0x100+8, rng.getrandbits(32))

            model = initial.copy()
            expected_events = []
            def model_word(address, value):
                struct.pack_into('<I', model, address & 0x1FFFFFFF, value & 0xFFFFFFFF)
            def get_word(address):
                return struct.unpack_from('<I', model, address & 0x1FFFFFFF)[0]
            def initialize_handler(index):
                if (index+case) % 3 and (index+case) % 4:
                    expected_events.append(('init', index))
            next_state, result = state, 0
            if state == 0:
                if not (flags & 0x80):
                    model_word(0x800942E4, SLOTS)
                    model_word(0x800942E8, SLOTS+0x6E84)
                    for start, stride in ((SLOTS, 0xA0C), (SLOTS+0x6E84, 0x10C)):
                        for index in range(11):
                            offset = (start+index*stride) & 0x1FFFFFFF
                            model[offset:offset+12] = b'\0\xff\xff\xff'+bytes(8)
                    for index in range(8): initialize_handler(index)
                    initialize_handler(85)
                    for index in range(nrecords):
                        handler = initial[(SCENE+0x100+12*index+7) & 0x1FFFFFFF]
                        if 8 <= handler < 85: initialize_handler(handler)
                    model_word(STATE, (get_word(STATE) & ~0x10000) | 8)
                    model_word(FLAGS, flags | 0x80)
                if (force or (get_word(FLAGS) & 2)) and (get_word(STATE) & 8):
                    next_state = 0x34
            if next_state == 0x34:
                expected_events.append(('CdRom_ReadSectorsFromLba', 12445, TIM, 30))
                next_state = 0x34 if read_result == -1 else 0x35
                result = 1
            elif next_state == 0x35:
                expected_events.append(('CdRom_PollReady',))
                if poll_result == -1:
                    next_state, result = 0x34, 1
                elif poll_result != 0:
                    result = 1
                else:
                    next_state = 0x36
            if next_state == 0x36:
                for index in range(nuploads):
                    expected_events.append(('Gpu_LoadTimAsset', TIM+0x100+index*20, TIM))
                for index in range(nimages+1):
                    expected_events.append(('Asset_FindTable08ByU32Key', SCENE, 0x73DECD80+4*index))
                    if index < nimages:
                        geometry = get_word(IMAGES+index*0x100+8)
                        source_word = get_word(IMAGES+index*0x100+4)
                        rectangle = struct.pack('<HHHH', (geometry >> 10) & 2047,
                                                geometry >> 21, geometry & 1023,
                                                (source_word >> 24) or 256)
                        expected_events.append(('LoadImage', rectangle, IMAGES+index*0x100+(source_word & 0xFFFFFF)))
                model_word(STATE, get_word(STATE) & ~8)
                next_state = 0
            model[(STATE+0xEF) & 0x1FFFFFFF] = next_state
            modeled = (result, expected_events, bytes(model[0x90000:0x180000]))

            def execute(body):
                machine = Uc(UC_ARCH_MIPS, UC_MODE_MIPS32 | UC_MODE_LITTLE_ENDIAN)
                machine.mem_map(0, 0x200000)
                machine.mem_write(0, bytes(initial))
                machine.mem_write(ENTRY & 0x1FFFFFFF, body)
                machine.reg_write(R.UC_MIPS_REG_A0, force)
                machine.reg_write(R.UC_MIPS_REG_SP, STACK)
                machine.reg_write(R.UC_MIPS_REG_RA, EXIT)
                for i in range(8): machine.reg_write(getattr(R, f'UC_MIPS_REG_S{i}'), 0xABCD0000+i)
                events = []
                def hook(m, address, size, data):
                    name = CALLS.get(address)
                    if name is None and address not in callbacks: return
                    args = [m.reg_read(getattr(R, f'UC_MIPS_REG_A{i}')) for i in range(3)]
                    result = 0
                    if address in callbacks:
                        events.append(('init', callbacks[address]))
                    elif name == 'CdRom_ReadSectorsFromLba':
                        events.append((name, *args)); result = read_result
                    elif name == 'CdRom_PollReady':
                        events.append((name,)); result = poll_result
                    elif name == 'Gpu_LoadTimAsset':
                        events.append((name, *args[:2]))
                    elif name == 'Asset_FindTable08ByU32Key':
                        events.append((name, *args[:2]))
                        index = (args[1]-0x73DECD80)//4
                        result = IMAGES+index*0x100 if 0 <= index < nimages else 0
                    elif name == 'LoadImage':
                        rect = bytes(m.mem_read(args[0] & 0x1FFFFFFF, 8))
                        events.append((name, rect, args[1]))
                    # Deliberately clobber all call-volatile registers except return/ra.
                    for reg in ('V1', 'A0', 'A1', 'A2', 'A3', 'T0', 'T1', 'T2', 'T3', 'T4', 'T5', 'T6', 'T7', 'T8', 'T9'):
                        m.reg_write(getattr(R, 'UC_MIPS_REG_'+reg), 0xCAFE0000)
                    m.reg_write(R.UC_MIPS_REG_V0, result & 0xFFFFFFFF)
                    m.reg_write(R.UC_MIPS_REG_PC, m.reg_read(R.UC_MIPS_REG_RA))
                machine.hook_add(UC_HOOK_CODE, hook)
                machine.emu_start(ENTRY, EXIT, count=50000)
                assert machine.reg_read(R.UC_MIPS_REG_PC) == EXIT, (case, 'return')
                assert machine.reg_read(R.UC_MIPS_REG_SP) == STACK, (case, 'stack')
                assert all(machine.reg_read(getattr(R, f'UC_MIPS_REG_S{i}')) == 0xABCD0000+i for i in range(8)), (case, 'saved')
                # Entire data RAM excludes executable bytes and the temporary stack.
                memory = bytes(machine.mem_read(0x90000, 0xF0000))
                return machine.reg_read(R.UC_MIPS_REG_V0), events, memory
            expected, actual = execute(retail), execute(compiled)
            assert expected == modeled, (case, 'retail/model', expected[:2], modeled[:2])
            assert actual == expected, (case, state, flags, force, actual[:2], expected[:2])

if __name__ == '__main__':
    unittest.main()
