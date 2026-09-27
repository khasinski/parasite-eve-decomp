"""ATB gauge signed arithmetic, callback reloads, and exact plain C."""
from pathlib import Path
import itertools
import random
import re
import struct
import unittest

ROOT = Path(__file__).resolve().parents[2]


class ATBGaugeTests(unittest.TestCase):
    def test_plain_c(self):
        source = (ROOT / 'src/main/battle/Battle_DrawATBGauge.c').read_text()
        source = re.sub(r'/\*.*?\*/|//[^\n]*', '', source, flags=re.S)
        self.assertIsNone(re.search(r'\b(?:asm|__asm__|INCLUDE_ASM|CC_POSTPASS|REGALLOC_BARRIER)\b', source))

    @unittest.skipUnless((ROOT / 'assets/USA/main.exe').is_file() and
                         (ROOT / 'build/USA/main.exe').is_file(), 'retail/build unavailable')
    def test_gauge(self):
        try:
            from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN, UC_HOOK_CODE
            from unicorn import mips_const as R
        except ImportError:
            self.skipTest('unicorn unavailable')
        base, size = 0x800325DC, 0x1FC
        offset = base - 0x8000F800
        bodies = [(ROOT / p).read_bytes()[offset:offset+size] for p in ('assets/USA/main.exe', 'build/USA/main.exe')]
        self.assertEqual(len(bodies[0]), size)
        self.assertEqual(*bodies)
        actor, action, stop, stack = 0x80100020, 0x80110020, 0x80010000, 0x801F0000
        callbacks = {0x80056C14: ('ammo', 1), 0x80077AC4: ('add', 2),
                     0x80021054: ('pad', 0), 0x800328DC: ('digits', 4)}
        saved = [getattr(R, f'UC_MIPS_REG_S{i}') for i in range(8)] + [R.UC_MIPS_REG_GP, R.UC_MIPS_REG_FP]
        sprite_initial = random.Random(base).randbytes(0x78)
        def s16(v): return ((v + 32768) & 65535) - 32768
        for slot, category, mode, ammo_base, count, mutate in itertools.product(
                (0, 1), range(4), (0, 0x40, 0x80, 0xC0), (-32768, -1, 0, 32767), (0, 4, 128, 255), (False, True)):
            active = slot ^ int(mutate)
            shot_count = 33 if mutate else 7
            effective_mode = mode ^ (0x80 if mutate else 0)
            amount = s16(ammo_base + shot_count)
            if count == 4:
                for kind in (1, 2, 0x189, -1):
                    if kind in (1, 0x189): amount = s16(amount - 1)
                    elif kind == 2:
                        if effective_mode == 0xC0: amount = s16(amount - 15)
                        elif effective_mode == 0x40: amount = s16(amount - 1)
            amount = max(amount, 0)
            y = s16((20, 32760)[slot] + 22)
            x = (100, 65530)[active]
            expected_sprite = bytearray(sprite_initial)
            struct.pack_into('<HH', expected_sprite, 0x20 + active*0x1C + 16, (x+8)&65535, y&65535)
            for body in bodies:
                m = Uc(UC_ARCH_MIPS, UC_MODE_MIPS32 | UC_MODE_LITTLE_ENDIAN)
                m.mem_map(0, 0x200000)
                def put(a, data): m.mem_write(a & 0x1FFFFFFF, bytes(data))
                def read(a, n): return bytes(m.mem_read(a & 0x1FFFFFFF, n))
                def word(a, v): put(a, struct.pack('<I', v & 0xFFFFFFFF))
                put(base, body)
                word(0x8009CDDC, slot); word(0x8009D278, actor); word(actor+0x68, action)
                word(action+0xC, (category << 20) | 7); word(action+0x10, mode | 15)
                for i in range(2):
                    put(0x8009E358+i*48+8, struct.pack('<HH', (100,65530)[i], (20,32760)[i]))
                    word(0x800B0E38+i*4, 0x80120000+i*0x100)
                put(0x8009E748, sprite_initial)
                for i, kind in enumerate((1, 2, 0x189, -1)):
                    put(0x800BE830+i*8+4, struct.pack('<h', kind))
                for a in callbacks: put(a, struct.pack('<III', 0, 0x03E00008, 0))
                events = []
                def hook(machine, pc, size, user):
                    if pc-4 not in callbacks: return
                    name, n = callbacks[pc-4]
                    args = tuple(machine.reg_read(getattr(R,'UC_MIPS_REG_'+r)) for r in ('A0','A1','A2','A3')[:n])
                    events.append(name)
                    result = 0
                    if name == 'ammo':
                        self.assertEqual(args, ((category-1)&0xFFFFFFFF,))
                        result = ammo_base
                        if mutate:
                            word(0x8009CDDC, active)
                            word(action+0xC, (category << 20) | 33)
                            word(action+0x10, effective_mode | 15)
                    elif name == 'add':
                        self.assertEqual(args, (0x80120000+active*0x100+16, 0x8009E768+active*0x1C))
                        self.assertEqual(read(0x8009E748, len(expected_sprite)), expected_sprite)
                    elif name == 'pad': result = count
                    else:
                        self.assertEqual(args, (0x8009E7A0+active*0x70, s16(x+64)&0xFFFFFFFF, s16(y-1)&0xFFFFFFFF, amount))
                        self.assertEqual(read(machine.reg_read(R.UC_MIPS_REG_SP)+16,4), b'\0'*4)
                    for r in ('V0','V1','A0','A1','A2','A3','T0','T1','T2','T3','T4','T5','T6','T7','T8','T9'):
                        machine.reg_write(getattr(R,'UC_MIPS_REG_'+r),0xDEADCAFE)
                    machine.reg_write(R.UC_MIPS_REG_V0, result & 0xFFFFFFFF)
                m.hook_add(UC_HOOK_CODE,hook)
                m.reg_write(R.UC_MIPS_REG_SP,stack); m.reg_write(R.UC_MIPS_REG_RA,stop)
                for i,reg in enumerate(saved): m.reg_write(reg,0xABCD0000+i)
                m.emu_start(base,stop,count=3000)
                self.assertEqual(events, ['ammo','add']+['pad']*(5 if count==4 else 1)+['digits'])
                self.assertEqual(read(0x8009E748,len(expected_sprite)),expected_sprite)
                self.assertEqual(m.reg_read(R.UC_MIPS_REG_PC),stop)
                self.assertEqual(m.reg_read(R.UC_MIPS_REG_SP),stack)
                for i,reg in enumerate(saved): self.assertEqual(m.reg_read(reg),0xABCD0000+i)


if __name__ == '__main__': unittest.main()
