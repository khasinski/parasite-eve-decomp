"""SPU initialization: constraint regression and callback/ABI trace checks."""
from pathlib import Path
import random
import struct
import unittest

ROOT = Path(__file__).resolve().parents[2]


class SpuInitVoiceStateTests(unittest.TestCase):
    def test_redundant_constraints_are_gone(self):
        source = (ROOT/'src/main/akao/Spu_InitVoiceState.c').read_text()
        self.assertNotRegex(source, r'\b(?:asm|__asm__|INCLUDE_ASM|REGALLOC_BARRIER)\b')
        self.assertIn('track = (u8 *)g_AkaoVoiceChannelTable;', source)
        for name in ('voice_base', 'voice_count', 'track_enabled', 'track_volume'):
            self.assertNotRegex(source, name+r'\s+asm')

    @unittest.skipUnless((ROOT/'assets/USA/main.exe').is_file() and
                         (ROOT/'build/USA/main.exe').is_file(), 'retail/build unavailable')
    def test_initialization_and_callback_trace(self):
        try:
            from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN, UC_HOOK_CODE
            from unicorn import mips_const as R
        except ImportError:
            self.skipTest('unicorn unavailable')
        base, size = 0x80085290, 0x3B4
        offset = base-0x8000F800
        bodies = [(ROOT/path).read_bytes()[offset:offset+size]
                  for path in ('assets/USA/main.exe', 'build/USA/main.exe')]
        self.assertEqual(len(bodies[0]), size)
        self.assertEqual(bodies[0], bodies[1])
        stop, stack = 0x80010000, 0x801F0000
        callbacks = {0x80085F74: ('common', 1), 0x800862F4: ('voice', 5),
                     0x8008CB54: ('reset', 1), 0x80085A64: ('reverb', 1)}
        regions = ((0x8009D1E0, 0x140), (0x800B6900, 0xA600))
        saved = [getattr(R, f'UC_MIPS_REG_S{i}') for i in range(8)] + [R.UC_MIPS_REG_GP, R.UC_MIPS_REG_FP]
        rng = random.Random(base)
        for case in range(16):
            initial = [rng.randbytes(length) for _, length in regions]
            results = []
            for body in bodies:
                machine = Uc(UC_ARCH_MIPS, UC_MODE_MIPS32 | UC_MODE_LITTLE_ENDIAN)
                machine.mem_map(0, 0x200000)
                def put(address, data): machine.mem_write(address & 0x1FFFFFFF, bytes(data))
                def read(address, length): return bytes(machine.mem_read(address & 0x1FFFFFFF, length))
                def word(address): return int.from_bytes(read(address, 4), 'little')
                def state(): return tuple(read(address, length) for address, length in regions)
                put(base, body)
                for (address, _), data in zip(regions, initial): put(address, data)
                for address in callbacks: put(address, struct.pack('<III', 0, 0x03E00008, 0))
                events = []
                def hook(uc, pc, length, user):
                    if pc-4 not in callbacks: return
                    name, count = callbacks[pc-4]
                    args = [uc.reg_read(getattr(R, f'UC_MIPS_REG_A{i}')) for i in range(min(count,4))]
                    if count == 5: args.append(word(uc.reg_read(R.UC_MIPS_REG_SP)+16))
                    events.append((name, tuple(args), state()))
                    if case & 1:
                        # Model callback writes and force callers to reload global state.
                        put(0x8009D2C4, struct.pack('<I', word(0x8009D2C4) ^ 0x5510))
                        put(0x800B6980+0x48, struct.pack('<I', len(events)*0x12345))
                    for reg in ('V0','V1','A0','A1','A2','A3','T0','T1','T2','T3','T4','T5','T6','T7','T8','T9'):
                        uc.reg_write(getattr(R, 'UC_MIPS_REG_'+reg), 0xDEADCAFE)
                machine.hook_add(UC_HOOK_CODE, hook)
                machine.reg_write(R.UC_MIPS_REG_SP, stack)
                machine.reg_write(R.UC_MIPS_REG_RA, stop)
                for index, reg in enumerate(saved): machine.reg_write(reg, 0xABCD0000+index)
                machine.emu_start(base, stop, count=10000)
                self.assertEqual(machine.reg_read(R.UC_MIPS_REG_PC), stop, case)
                self.assertEqual(machine.reg_read(R.UC_MIPS_REG_SP), stack, case)
                for index, reg in enumerate(saved): self.assertEqual(machine.reg_read(reg), 0xABCD0000+index, case)
                self.assertEqual([(name, args) for name, args, _ in events],
                                 [('common', (0x800C0D90,))] +
                                 [('voice', (i,0,0,0,0)) for i in list(range(24))*2] +
                                 [('reset', (4,)), ('reverb', (1,))])
                # Independent field oracle for both banks and twelve nested slots.
                for index in range(48):
                    voice = 0x800B8AC0 + index*0x11C
                    for field, width, expected in ((0x38,4,0), (0xF0,4,24),
                                                   (0x54,2,0), (0x50,4,0)):
                        self.assertEqual(int.from_bytes(read(voice+field,width),'little'), expected,
                                         (case,index,field))
                for index in range(12):
                    track = 0x800BC000 + index*0x11C
                    for field, width, expected in ((0x38,4,0), (0xF0,4,index+12),
                                                   (0x54,2,1), (0x50,4,0),
                                                   (0xD8,2,0x7F00), (0x74,2,0),
                                                   (0x70,2,0), (0x3C,4,0)):
                        self.assertEqual(int.from_bytes(read(track+field,width),'little'), expected,
                                         (case,index,field))
                results.append((events, state()))
            self.assertEqual(results[0], results[1], case)


if __name__ == '__main__': unittest.main()
