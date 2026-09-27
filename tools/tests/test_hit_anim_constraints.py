"""Plain-C hit animation with angle boundaries and callback pointer reloads."""
from pathlib import Path
import itertools
import random
import re
import struct
import unittest

ROOT = Path(__file__).resolve().parents[2]


class HitAnimConstraintTests(unittest.TestCase):
    def test_constraints_removed(self):
        source = (ROOT / 'src/main/entity/Entity_ApplyHitAndSetAnim.c').read_text()
        source = re.sub(r'/\*.*?\*/|//[^\n]*', '', source, flags=re.S)
        self.assertIsNone(re.search(r'\b(?:asm|__asm__|INCLUDE_ASM|CC_POSTPASS|REGALLOC_BARRIER)\b', source))

    @unittest.skipUnless((ROOT / 'assets/USA/main.exe').is_file() and
                         (ROOT / 'build/USA/main.exe').is_file(), 'retail/build unavailable')
    def test_hit_animation(self):
        try:
            from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN, UC_HOOK_CODE
            from unicorn import mips_const as R
        except ImportError:
            self.skipTest('unicorn unavailable')
        images = [(ROOT / p).read_bytes() for p in ('assets/USA/main.exe', 'build/USA/main.exe')]
        base, size = 0x8001F814, 0x1B0
        offset = base-0x8000F800
        self.assertEqual(images[0][offset:offset+size], images[1][offset:offset+size])
        player, state, arg, stop, stack = 0x80180020, 0x80190020, 0x801A0000, 0x80010000, 0x801F0000
        callbacks = {0x800305C8:'angle', 0x8001A680:'mode', 0x8006DE80:'effect'}
        saved = [getattr(R,f'UC_MIPS_REG_S{i}') for i in range(8)]+[R.UC_MIPS_REG_FP]
        rng = random.Random(base)
        initial_players, initial_globals = rng.randbytes(0x440), rng.randbytes(0x80)
        angles = (-32768,-1,0,0x1FF,0x200,0x5FF,0x600,0x9FF,0xA00,0xDFF,0xE00,0x1000200)
        def s16(v): return ((v+32768)&65535)-32768
        for mode, angle, blocked, mutate in itertools.product(range(16),angles,(0,0x2000,0x10000),(False,True)):
            expected_g = bytearray(initial_globals)
            struct.pack_into('<I',expected_g,0x14,player)
            struct.pack_into('<I',expected_g,0x38,state)
            before_g = bytes(expected_g)
            if mode in range(6,16):
                struct.pack_into('<HBBI',expected_g,0x58,1,mode,0x81,0x810000 if mode in (7,9,11) else 0x12345678)
            bucket = 0 if s16(angle)<0x200 or s16(angle)>=0xE00 else 2 if s16(angle)<0x600 else 1 if s16(angle)<0xA00 else 3
            for image in images:
                m=Uc(UC_ARCH_MIPS,UC_MODE_MIPS32|UC_MODE_LITTLE_ENDIAN);m.mem_map(0,0x200000)
                def put(a,data):m.mem_write(a&0x1FFFFFFF,bytes(data))
                def read(a,n):return bytes(m.mem_read(a&0x1FFFFFFF,n))
                def word(a,v):put(a,struct.pack('<I',v&0xFFFFFFFF))
                put(0x8000F800,image);put(player-0x20,initial_players);put(0x8009D240,before_g)
                word(state+0x4C,blocked)
                for i in range(4):
                    p=player+i*0x100
                    put(p+0xE,bytes((mode,0x81)));word(p+0x14,0x12345678)
                    word(p+0x98,0x100 if mode&1 else 0)
                    for off,v in ((0x2A,-100-i),(0x2E,200+i),(0x32,-300-i)):put(p+off,struct.pack('<h',v))
                expected_p=bytearray(read(player-0x20,len(initial_players)))
                current_g=bytearray(expected_g)
                events=[]
                for a in callbacks:put(a,struct.pack('<III',0,0x03E00008,0))
                def hook(machine,pc,size,user):
                    if pc-4 not in callbacks:return
                    name=callbacks[pc-4];events.append(name)
                    self.assertEqual(read(0x8009D240,len(current_g)),current_g)
                    args=tuple(machine.reg_read(getattr(R,'UC_MIPS_REG_'+r)) for r in ('A0','A1','A2','A3'))
                    if name=='angle':self.assertEqual(args[:2],(arg,player))
                    elif name=='mode':self.assertEqual(args[:2],(player+(0x100 if mutate else 0),bucket))
                    else:
                        i=2 if mutate else 0
                        self.assertEqual(args,(0x46A,0,(-100-i)&0xFFFFFFFF,200+i))
                        self.assertEqual(int.from_bytes(read(machine.reg_read(R.UC_MIPS_REG_SP)+16,4),'little'),(-300-i)&0xFFFFFFFF)
                    if mutate:
                        index={'angle':1,'mode':2,'effect':3}[name]
                        struct.pack_into('<I',current_g,0x14,player+index*0x100)
                        word(0x8009D254,player+index*0x100)
                    for r in ('V0','V1','A0','A1','A2','A3','T0','T1','T2','T3','T4','T5','T6','T7','T8','T9'):machine.reg_write(getattr(R,'UC_MIPS_REG_'+r),0xDEADCAFE)
                    machine.reg_write(R.UC_MIPS_REG_V0,(angle if name=='angle' else 0xBADF00D)&0xFFFFFFFF)
                m.hook_add(UC_HOOK_CODE,hook)
                for n,v in (('A0',arg),('SP',stack),('RA',stop),('GP',0x8009CD70)):m.reg_write(getattr(R,'UC_MIPS_REG_'+n),v)
                for i,r in enumerate(saved):m.reg_write(r,0xABCD0000+i)
                m.emu_start(base,stop,count=300)
                self.assertEqual(events,[] if blocked else ['angle','mode','effect'])
                if not blocked and mode&1:
                    struct.pack_into('<I',expected_p,0x20+(0x300 if mutate else 0)+0x98,0)
                    struct.pack_into('<H',current_g,0x58,2)
                self.assertEqual(read(player-0x20,len(expected_p)),expected_p)
                self.assertEqual(read(0x8009D240,len(current_g)),current_g)
                self.assertEqual(m.reg_read(R.UC_MIPS_REG_V0),(0 if blocked else s16(angle))&0xFFFFFFFF)
                self.assertEqual(m.reg_read(R.UC_MIPS_REG_PC),stop)
                self.assertEqual(m.reg_read(R.UC_MIPS_REG_SP),stack)
                self.assertEqual(m.reg_read(R.UC_MIPS_REG_GP),0x8009CD70)
                for i,r in enumerate(saved):self.assertEqual(m.reg_read(r),0xABCD0000+i)


if __name__ == '__main__':unittest.main()
