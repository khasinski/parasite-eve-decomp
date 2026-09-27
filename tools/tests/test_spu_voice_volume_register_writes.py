"""Plain volume helper and ordered MMIO writes; not physical SPU emulation."""
from pathlib import Path
import itertools
import random
import struct
import unittest

ROOT = Path(__file__).resolve().parents[2]


class SpuVoiceVolumeTests(unittest.TestCase):
    def test_volume_helper_is_plain(self):
        source = (ROOT/'src/main/akao/Akao_SpuVoiceRegisters.c').read_text()
        body = source.split('void AkaoSpuVoice_SetVolume(',1)[1].split('void AkaoSpuVoice_SetPitch',1)[0]
        self.assertNotRegex(body,r'\b(?:asm|__asm__|INCLUDE_ASM|CC_POSTPASS)\b')

    @unittest.skipUnless((ROOT/'assets/USA/main.exe').is_file() and
                         (ROOT/'build/USA/main.exe').is_file(), 'retail/build unavailable')
    def test_ordered_volume_writes(self):
        try:
            from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN, UC_HOOK_MEM_READ, UC_HOOK_MEM_WRITE, UC_MEM_WRITE
            from unicorn import mips_const as R
        except ImportError:
            self.skipTest('unicorn unavailable')
        # Entire TU byte gate; this model exercises the changed volume helper.
        # Other functions still contain pins/barriers, so TU is not clean yet.
        images = [(ROOT/p).read_bytes() for p in ('assets/USA/main.exe','build/USA/main.exe')]
        offset,size = 0x8008770C-0x8000F800,0x39C
        self.assertEqual(len(images[0][offset:offset+size]),size)
        self.assertEqual(images[0][offset:offset+size],images[1][offset:offset+size])
        entry,stop,stack,mmio = 0x80087798,0x80010000,0x801F0000,0x1F801000
        bodies = [data[entry-0x8000F800:entry-0x8000F800+0x24] for data in images]
        saved = [getattr(R,f'UC_MIPS_REG_S{i}') for i in range(8)]+[R.UC_MIPS_REG_GP,R.UC_MIPS_REG_FP]
        rng = random.Random(entry)
        values = (0,1,127,255,0x7FFF,0xFFFF,0x80000000,0xFFFFFFFF)
        for case,(index,left,right) in enumerate(itertools.product(range(24),values,values)):
            initial = rng.randbytes(0x1000)
            address = 0x1F801C00+16*index
            expected = bytearray(initial)
            struct.pack_into('<HH',expected,address-mmio,left&0x7FFF,right&0x7FFF)
            for body in bodies:
                m = Uc(UC_ARCH_MIPS,UC_MODE_MIPS32|UC_MODE_LITTLE_ENDIAN)
                m.mem_map(0,0x200000)
                m.mem_map(mmio,0x1000)
                m.mem_write(entry&0x1FFFFFFF,body)
                m.mem_write(mmio,initial)
                events = []
                def hook(machine,kind,addr,width,value,user):
                    events.append((kind,addr,width,value))
                m.hook_add(UC_HOOK_MEM_READ|UC_HOOK_MEM_WRITE,hook,begin=mmio,end=mmio+0xFFF)
                for reg,val in (('A0',index),('A1',left),('A2',right),('SP',stack),('RA',stop)):
                    m.reg_write(getattr(R,'UC_MIPS_REG_'+reg),val)
                for i,reg in enumerate(saved): m.reg_write(reg,0xABCD0000+i)
                m.emu_start(entry,stop,count=100)
                self.assertEqual(m.reg_read(R.UC_MIPS_REG_PC),stop,case)
                self.assertEqual(m.reg_read(R.UC_MIPS_REG_SP),stack,case)
                for i,reg in enumerate(saved): self.assertEqual(m.reg_read(reg),0xABCD0000+i,case)
                self.assertEqual(events,[(UC_MEM_WRITE,address,2,left&0x7FFF),(UC_MEM_WRITE,address+2,2,right&0x7FFF)],case)
                self.assertEqual(bytes(m.mem_read(mmio,0x1000)),bytes(expected),case)


if __name__ == '__main__': unittest.main()
