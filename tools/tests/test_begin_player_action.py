"""Player-action setup: flag semantics, overlap, callback order and ABI."""
from pathlib import Path
import itertools
import random
import re
import struct
import unittest

ROOT = Path(__file__).resolve().parents[2]


class BeginPlayerActionTests(unittest.TestCase):
    def test_plain_c(self):
        source = (ROOT / 'src/main/battle/Battle_BeginPlayerAction.c').read_text()
        source = re.sub(r'/\*.*?\*/|//[^\n]*', '', source, flags=re.S)
        self.assertIsNone(re.search(r'\b(?:asm|__asm__|INCLUDE_ASM|CC_POSTPASS|REGALLOC_BARRIER)\b', source))

    @unittest.skipUnless((ROOT / 'assets/USA/main.exe').is_file() and
                         (ROOT / 'build/USA/main.exe').is_file(), 'retail/build unavailable')
    def test_flags_and_callbacks(self):
        try:
            from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN, UC_HOOK_CODE
            from unicorn import mips_const as R
        except ImportError:
            self.skipTest('unicorn unavailable')
        base, size = 0x80020C74, 0x70
        offset = base - 0x8000F800
        bodies = [(ROOT / path).read_bytes()[offset:offset+size]
                  for path in ('assets/USA/main.exe', 'build/USA/main.exe')]
        self.assertEqual(len(bodies[0]), size)
        self.assertEqual(*bodies)
        player, globals_base, stop, stack = 0x80100020, 0x8009D240, 0x80010000, 0x801F0000
        callbacks = {0x8001A680: 'mode', 0x80021D4C: 'sounds'}
        saved = [getattr(R, f'UC_MIPS_REG_S{i}') for i in range(8)] + [R.UC_MIPS_REG_FP]
        rng = random.Random(base)
        object_seed, globals_seed = rng.randbytes(0x440), rng.randbytes(0xC0)
        values = (0, 1, 0x100, 0x10000, 0xA5A5A5A5, 0xFFFFFFFF)
        for entity_flags, actor_flags, control, alias, mutate in itertools.product(values, values, values, (False, True), (False, True)):
            actor = player + (0x4C if alias else 0x200)
            initial_objects, initial_globals = bytearray(object_seed), bytearray(globals_seed)
            struct.pack_into('<I', initial_objects, 0x20+0x98, entity_flags)
            struct.pack_into('<I', initial_objects, actor-player+0x20+0x4C, actor_flags)
            for address, value in ((0x8009D254, player), (0x8009D278, actor), (0x8009D2E8, control)):
                struct.pack_into('<I', initial_globals, address-globals_base, value)
            expected_objects, expected_globals = bytearray(initial_objects), bytearray(initial_globals)
            def update(data, offset, clear, set_bits):
                value = struct.unpack_from('<I', data, offset)[0]
                struct.pack_into('<I', data, offset, (value & ~clear) | set_bits)
            update(expected_globals, 0x8009D2E8-globals_base, 0, 1)
            update(expected_objects, 0xB8, 0x100, 0)
            update(expected_objects, actor-player+0x6C, 0, 0x10000)
            struct.pack_into('<H', expected_globals, 0x8009D298-globals_base, 0)
            for body in bodies:
                expected_o, expected_g = bytearray(expected_objects), bytearray(expected_globals)
                m = Uc(UC_ARCH_MIPS, UC_MODE_MIPS32 | UC_MODE_LITTLE_ENDIAN)
                m.mem_map(0, 0x200000)
                def put(a, data): m.mem_write(a & 0x1FFFFFFF, bytes(data))
                def read(a, n): return bytes(m.mem_read(a & 0x1FFFFFFF, n))
                put(base, body); put(player-0x20, initial_objects); put(globals_base, initial_globals)
                for address in callbacks: put(address, struct.pack('<III', 0, 0x03E00008, 0))
                events = []
                def hook(machine, pc, size, user):
                    if pc-4 not in callbacks: return
                    name = callbacks[pc-4]
                    events.append(name)
                    self.assertEqual(read(player-0x20, len(expected_o)), expected_o)
                    self.assertEqual(read(globals_base, len(expected_g)), expected_g)
                    if name == 'mode':
                        self.assertEqual(machine.reg_read(R.UC_MIPS_REG_A0), player)
                        self.assertEqual(machine.reg_read(R.UC_MIPS_REG_A1), 0x12)
                        if mutate:
                            struct.pack_into('<I', expected_o, 0xB8, 0xFEDCBA98)
                            struct.pack_into('<I', expected_g, 0x8009D254-globals_base, actor)
                            struct.pack_into('<H', expected_g, 0x8009D298-globals_base, 0x8123)
                            put(player-0x20, expected_o); put(globals_base, expected_g)
                    for reg in ('V0','V1','A0','A1','A2','A3','T0','T1','T2','T3','T4','T5','T6','T7','T8','T9'):
                        machine.reg_write(getattr(R, 'UC_MIPS_REG_'+reg), 0xDEADCAFE)
                m.hook_add(UC_HOOK_CODE, hook)
                m.reg_write(R.UC_MIPS_REG_GP, 0x8009CD70)
                m.reg_write(R.UC_MIPS_REG_SP, stack); m.reg_write(R.UC_MIPS_REG_RA, stop)
                for i, reg in enumerate(saved): m.reg_write(reg, 0xABCD0000+i)
                m.emu_start(base, stop, count=200)
                self.assertEqual(events, ['mode', 'sounds'])
                self.assertEqual(read(player-0x20, len(expected_o)), expected_o)
                self.assertEqual(read(globals_base, len(expected_g)), expected_g)
                self.assertEqual(m.reg_read(R.UC_MIPS_REG_PC), stop)
                self.assertEqual(m.reg_read(R.UC_MIPS_REG_SP), stack)
                self.assertEqual(m.reg_read(R.UC_MIPS_REG_GP), 0x8009CD70)
                for i, reg in enumerate(saved): self.assertEqual(m.reg_read(reg), 0xABCD0000+i)


if __name__ == '__main__': unittest.main()
