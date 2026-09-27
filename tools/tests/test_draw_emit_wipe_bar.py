"""Retail wipe-bar ordering and allocation model; rendering calls are stubs."""
from pathlib import Path
import re
import struct
import unittest

ROOT = Path(__file__).resolve().parents[2]


class DrawEmitWipeBarTests(unittest.TestCase):
    def test_translation_unit_is_plain_c(self):
        source = (ROOT/'src/main/gpu/Draw_EmitWipeBar.c').read_text()
        source = re.sub(r'/\*.*?\*/|//[^\n]*', '', source, flags=re.S)
        self.assertNotRegex(source, r'\b(?:asm|__asm__|INCLUDE_ASM|CC_POSTPASS)\b')

    @unittest.skipUnless((ROOT/'assets/USA/main.exe').is_file() and
                         (ROOT/'build/USA/main.exe').is_file(), 'retail/build unavailable')
    def test_retail_and_built_model(self):
        try:
            from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN, UC_HOOK_CODE
            from unicorn import mips_const as R
        except ImportError:
            self.skipTest('unicorn unavailable')
        entry, stop, stack = 0x80061878, 0x80010000, 0x801F0000
        arena, edges, otbase = 0x80110000, 0x80120000, 0x80130000
        cursor_global, base_global, ot_global = 0x8009D100, 0x8009D104, 0x8009D11C
        set_mode, color_rect, assertion = 0x80077C84, 0x8006153C, 0x800527C0
        offset = entry - 0x8000F800
        bodies = [(ROOT/path).read_bytes()[offset:offset+0x1C4]
                  for path in ('assets/USA/main.exe', 'build/USA/main.exe')]
        self.assertEqual(*bodies)
        groups = [((), ()), (((0, -128),), ()),
                  ((), ((127, 127),)), (((1, -1), (126, 64)), ((7, -64), (0, 0)))]
        for mode in (-2, -1, 0, 1, 2, 3, 255, 65535):
            for left, right in groups:
                stream = bytes([v & 255 for pair in left for v in pair]+[255]+
                               [v & 255 for pair in right for v in pair]+[128])
                for initial in (0, 0x3FD8, 0x3FF0, 0x3FF8):
                    for mutate in (False, True):
                        expected = bytearray(b'\xA5'*0x4020)
                        ot = [0xAB123456, 0xCD654321]
                        active, cursor = 0, arena+initial
                        events, packets = [], []
                        failed = False
                        for mode_index, page in enumerate((((mode+1)&3)<<5, ((2-mode)&3)<<5)):
                            if cursor+8 >= arena+0x4000:
                                events.append(('assert', 1))
                                failed = True
                                break
                            packet = cursor
                            packets.append(packet)
                            cursor += 8
                            events.append(('mode', packet, 0, 0, page, cursor))
                            struct.pack_into('<II', expected, packet-arena, 0x01555555, 0xE1000000|page)
                            if mutate and mode_index == 0: cursor += 8
                        if not failed:
                            for phase, pairs in enumerate((left, right)):
                                for x, y in pairs:
                                    events.append(('rect', x&0xFFFFFFFF, y&0xFFFFFFFF,
                                                   (2 if phase == 0 else -2)&0xFFFFFFFF,
                                                   (mode if phase == 0 else int(not mode))&0xFFFFFFFF,
                                                   tuple(ot)))
                                    if mutate: active ^= 1
                                packet = packets[phase]
                                tag = struct.unpack_from('<I', expected, packet-arena)[0]
                                struct.pack_into('<I', expected, packet-arena, (tag&0xFF000000)|(ot[active]&0xFFFFFF))
                                ot[active] = (ot[active]&0xFF000000)|(packet&0xFFFFFF)
                        for body in bodies:
                            m = Uc(UC_ARCH_MIPS, UC_MODE_MIPS32|UC_MODE_LITTLE_ENDIAN)
                            m.mem_map(0, 0x200000)
                            def put(address, data): m.mem_write(address&0x1FFFFFFF, bytes(data))
                            def word(address, value): put(address, struct.pack('<I', value&0xFFFFFFFF))
                            def read(address): return struct.unpack('<I', m.mem_read(address&0x1FFFFFFF, 4))[0]
                            put(entry, body)
                            put(arena, b'\xA5'*len(expected))
                            put(edges, stream)
                            word(cursor_global, arena+initial)
                            word(base_global, arena)
                            word(ot_global, otbase)
                            put(otbase, struct.pack('<II', 0xAB123456, 0xCD654321))
                            for address in (set_mode, color_rect, assertion):
                                put(address, struct.pack('<III', 0, 0x03E00008, 0))
                            for name, value in (('A0', edges), ('A1', mode&0xFFFFFFFF),
                                                ('RA', stop), ('SP', stack), ('GP', 0x8009CD70)):
                                m.reg_write(getattr(R, 'UC_MIPS_REG_'+name), value)
                            for i in range(8): m.reg_write(getattr(R, f'UC_MIPS_REG_S{i}'), 0xABCD0000+i)
                            observed = []
                            state = [0, 0]
                            def hook(machine, address, size, data):
                                if address not in (set_mode+4, color_rect+4, assertion+4): return
                                args = [machine.reg_read(getattr(R, f'UC_MIPS_REG_A{i}')) for i in range(4)]
                                if address == assertion+4:
                                    observed.append(('assert', args[0]))
                                    machine.emu_stop()
                                    return
                                if address == set_mode+4:
                                    observed.append(('mode', *args, read(cursor_global)))
                                    word(args[0], 0x01555555)
                                    word(args[0]+4, 0xE1000000|args[3])
                                    if mutate and state[0] == 0: word(cursor_global, read(cursor_global)+8)
                                    state[0] += 1
                                else:
                                    observed.append(('rect', *args, (read(otbase), read(otbase+4))))
                                    if mutate:
                                        state[1] ^= 1
                                        word(ot_global, otbase+4*state[1])
                                for name in ('V0','V1','A0','A1','A2','A3','T0','T1','T2','T3','T4','T5','T6','T7','T8','T9'):
                                    machine.reg_write(getattr(R, 'UC_MIPS_REG_'+name), 0xDEADCAFE)
                            m.hook_add(UC_HOOK_CODE, hook)
                            m.emu_start(entry, stop, count=2000)
                            context = (mode, left, right, initial, mutate)
                            self.assertEqual(observed, events, context)
                            self.assertEqual(bytes(m.mem_read(arena&0x1FFFFFFF, len(expected))), bytes(expected), context)
                            self.assertEqual((read(otbase), read(otbase+4)), tuple(ot), context)
                            self.assertEqual(read(cursor_global), cursor, context)
                            self.assertEqual(read(ot_global), otbase+4*active, context)
                            self.assertEqual(bytes(m.mem_read(edges&0x1FFFFFFF, len(stream))), stream)
                            if failed:
                                self.assertEqual(m.reg_read(R.UC_MIPS_REG_PC), assertion+4)
                            else:
                                self.assertEqual(m.reg_read(R.UC_MIPS_REG_PC), stop)
                                self.assertEqual(m.reg_read(R.UC_MIPS_REG_SP), stack)
                                self.assertEqual(m.reg_read(R.UC_MIPS_REG_GP), 0x8009CD70)
                                for i in range(8):
                                    self.assertEqual(m.reg_read(getattr(R, f'UC_MIPS_REG_S{i}')), 0xABCD0000+i)


if __name__ == '__main__':
    unittest.main()
