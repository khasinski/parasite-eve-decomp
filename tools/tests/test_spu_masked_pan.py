"""Masked pan updates: whole-TU match and independent twelve-track models."""
from pathlib import Path
import itertools
import random
import re
import struct
import unittest

ROOT = Path(__file__).resolve().parents[2]


class SpuMaskedPanTests(unittest.TestCase):
    def test_translation_unit_is_plain_c(self):
        source = (ROOT/'src/main/akao/spu5.c').read_text()
        source = re.sub(r'/\*.*?\*/|//[^\n]*', '', source, flags=re.S)
        self.assertNotRegex(source, r'\b(?:asm|__asm__|INCLUDE_ASM|CC_POSTPASS|REGALLOC_BARRIER)\b')
        for field in ('key_on_mask','key_off_mask','panpot','panpot_delta',
                      'panpot_slide_duration','update_flags'):
            self.assertIn('voice->'+field, source)

    @unittest.skipUnless((ROOT/'assets/USA/main.exe').is_file() and
                         (ROOT/'build/USA/main.exe').is_file(), 'retail/build unavailable')
    def test_retail_and_built_field_models(self):
        try:
            from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN
            from unicorn import mips_const as R
        except ImportError:
            self.skipTest('unicorn unavailable')
        base,size = 0x8008BA3C,0x258
        offset = base-0x8000F800
        bodies = [(ROOT/path).read_bytes()[offset:offset+size]
                  for path in ('assets/USA/main.exe','build/USA/main.exe')]
        self.assertEqual(len(bodies[0]),size)
        self.assertEqual(*bodies)
        tracks,active_address,arg,stop,stack = 0x800BC000,0x800BCD50,0x80110020,0x80010000,0x801F0000
        saved = [getattr(R,f'UC_MIPS_REG_S{i}') for i in range(8)]+[R.UC_MIPS_REG_FP,R.UC_MIPS_REG_GP]
        initial = random.Random(base).randbytes(0x20+12*0x11C)
        active_masks = (0,0x1000,0x800000,0xFFF000,0x555000,0xAAA000,0xFFF,0xFFFFFFFF)
        selectors = ((0,1),(0,0x80000000),(1,99),(0x80000000,99),(0xFFFFFFFF,99))
        targets = (0,1,127,128,254,255)
        def signed16(value): return (value&32767)-(value&32768)
        # RAM buffers are disjoint. Nonzero full32 durations with low16 zero
        # trap on division and are excluded; no physical SPU I/O occurs here.
        for body in bodies:
            m=Uc(UC_ARCH_MIPS,UC_MODE_MIPS32|UC_MODE_LITTLE_ENDIAN)
            m.mem_map(0,0x200000)
            def put(address,data): m.mem_write(address&0x1FFFFFFF,bytes(data))
            def read(address,length): return bytes(m.mem_read(address&0x1FFFFFFF,length))
            put(base,body)
            for slide in (False,True):
                steps = (0,1,2,127,32767,32768,65535,0xFFFF0001) if slide else (0,)
                for active,(selector,selected_id),target,step in itertools.product(active_masks,selectors,targets,steps):
                    before=bytearray(initial)
                    for i in range(12):
                        off=0x20+i*0x11C
                        struct.pack_into('<II',before,off+0x28,(1,0x80000000,7)[i%3],(0,1,2,0x80000000,0xFFFFFFFF)[i%5])
                        struct.pack_into('<H',before,off+0x76,(0,1,0x7FFF,0x8000,0xFFFF,0xFF00)[i%6])
                    expected=bytearray(before)
                    for i in range(12):
                        off=0x20+i*0x11C
                        identity,keymask=struct.unpack_from('<II',before,off+0x28)
                        if not active&(0x1000<<i): continue
                        if not (keymask&selector if selector else identity==selected_id): continue
                        if slide:
                            duration=step or 1
                            current=struct.unpack_from('<H',before,off+0x76)[0]
                            delta,denom=signed16((target<<8)-current),signed16(duration)
                            quotient=(abs(delta)//abs(denom))*(-1 if (delta<0)!=(denom<0) else 1)
                            struct.pack_into('<H',expected,off+0x78,duration&0xFFFF)
                            struct.pack_into('<H',expected,off+0xDC,quotient&0xFFFF)
                        else:
                            struct.pack_into('<HH',expected,off+0x76,target<<8,0)
                            dirty=struct.unpack_from('<I',before,off+0xF4)[0]
                            struct.pack_into('<I',expected,off+0xF4,dirty|3)
                    arguments=struct.pack('<IIIII',0x12345678,selected_id,selector,step if slide else target,target)
                    guard=b'\xA5'*0x20
                    put(tracks-0x20,before)
                    put(active_address,struct.pack('<I',active));put(active_address+4,guard)
                    put(arg-0x20,guard+arguments+guard)
                    for name,value in (('A0',arg),('SP',stack),('RA',stop)):
                        m.reg_write(getattr(R,'UC_MIPS_REG_'+name),value)
                    for i,reg in enumerate(saved): m.reg_write(reg,0xABCD0000+i)
                    m.emu_start(base+(0xE0 if slide else 0),stop,count=3000)
                    case=(slide,active,selector,selected_id,target,step)
                    self.assertEqual(read(tracks-0x20,len(expected)),expected,case)
                    self.assertEqual(read(arg-0x20,0x40+len(arguments)),guard+arguments+guard,case)
                    self.assertEqual(read(active_address,0x24),struct.pack('<I',active)+guard,case)
                    self.assertEqual(m.reg_read(R.UC_MIPS_REG_PC),stop,case)
                    self.assertEqual(m.reg_read(R.UC_MIPS_REG_SP),stack,case)
                    for i,reg in enumerate(saved): self.assertEqual(m.reg_read(reg),0xABCD0000+i,case)


if __name__ == '__main__': unittest.main()
