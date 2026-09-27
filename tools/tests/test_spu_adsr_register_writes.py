"""ADSR read/modify/write ordering in mapped RAM, not physical SPU behavior."""
from pathlib import Path
import itertools
import random
import struct
import unittest

ROOT = Path(__file__).resolve().parents[2]
CASES = (
    ('Attack',0x8008780C,0x30,0x1F801C08,1,2,15,8,0xFF),
    ('SustainRate',0x8008788C,0x34,0x1F801C0A,2,1,14,6,0x3F),
    ('ReleaseRate',0x800878C0,0x30,0x1F801C0A,2,2,5,0,0xFFC0),
)


class SpuAdsrRegisterTests(unittest.TestCase):
    def test_changed_helpers_are_plain(self):
        source = (ROOT/'src/main/akao/Akao_SpuVoiceRegisters.c').read_text()
        for name,*_ in CASES:
            body = source.split('void AkaoSpuVoice_SetAdsr'+name+'(',1)[1].split('\n}',1)[0]
            self.assertNotRegex(body,r'\b(?:asm|__asm__|INCLUDE_ASM|CC_POSTPASS)\b')

    @unittest.skipUnless((ROOT/'assets/USA/main.exe').is_file() and
                         (ROOT/'build/USA/main.exe').is_file(), 'retail/build unavailable')
    def test_read_modify_write(self):
        try:
            from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN, UC_HOOK_MEM_READ, UC_HOOK_MEM_WRITE, UC_MEM_WRITE, UC_MEM_READ
            from unicorn import mips_const as R
        except ImportError:
            self.skipTest('unicorn unavailable')
        images = [(ROOT/p).read_bytes() for p in ('assets/USA/main.exe','build/USA/main.exe')]
        offset,size = 0x8008770C-0x8000F800,0x39C
        self.assertEqual(len(images[0][offset:offset+size]),size)
        self.assertEqual(images[0][offset:offset+size],images[1][offset:offset+size])
        stop,stack,mmio = 0x80010000,0x801F0000,0x1F801000
        saved = [getattr(R,f'UC_MIPS_REG_S{i}') for i in range(8)]+[R.UC_MIPS_REG_GP,R.UC_MIPS_REG_FP]
        rng = random.Random(offset)
        values = (0,1,127,255,0x7FFF,0xFFFF,0x80000000,0xFFFFFFFF)
        for name,entry,length,register,width,rs,ls,vs,mask in CASES:
            bodies = [data[entry-0x8000F800:entry-0x8000F800+length] for data in images]
            for index,left,right in itertools.product(range(24),values,values):
                initial = rng.randbytes(0x1000)
                address = register+index*16
                current = int.from_bytes(initial[address-mmio:address-mmio+width],'little')
                value = ((current&mask)|((right>>rs)<<ls)|(left<<vs))&0xFFFF
                expected = bytearray(initial)
                struct.pack_into('<H',expected,address-mmio,value)
                for body in bodies:
                    m = Uc(UC_ARCH_MIPS,UC_MODE_MIPS32|UC_MODE_LITTLE_ENDIAN)
                    m.mem_map(0,0x200000)
                    m.mem_map(mmio,0x1000)
                    m.mem_write(entry&0x1FFFFFFF,body)
                    m.mem_write(mmio,initial)
                    events = []
                    def hook(machine,kind,addr,n,value,user):
                        if kind == UC_MEM_READ: value = int.from_bytes(machine.mem_read(addr,n),'little')
                        # Unicorn's SH hook exposes the full source register;
                        # only the low 16 bits are transferred to this address.
                        else: value &= (1<<(8*n))-1
                        events.append((kind,addr,n,value))
                    m.hook_add(UC_HOOK_MEM_READ|UC_HOOK_MEM_WRITE,hook,begin=mmio,end=mmio+0xFFF)
                    for reg,val in (('A0',index),('A1',left),('A2',right),('SP',stack),('RA',stop)):
                        m.reg_write(getattr(R,'UC_MIPS_REG_'+reg),val)
                    for i,reg in enumerate(saved): m.reg_write(reg,0xABCD0000+i)
                    m.emu_start(entry,stop,count=100)
                    context = (name,index,left,right)
                    self.assertEqual(m.reg_read(R.UC_MIPS_REG_PC),stop,context)
                    self.assertEqual(m.reg_read(R.UC_MIPS_REG_SP),stack,context)
                    for i,reg in enumerate(saved): self.assertEqual(m.reg_read(reg),0xABCD0000+i,context)
                    self.assertEqual(events,[(UC_MEM_READ,address,width,current),(UC_MEM_WRITE,address,2,value)],context)
                    self.assertEqual(bytes(m.mem_read(mmio,0x1000)),bytes(expected),context)


if __name__ == '__main__': unittest.main()
