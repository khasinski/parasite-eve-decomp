"""CD event-byte decoding with native copy and overlapping source buffers."""
from pathlib import Path
import itertools
import random
import re
import struct
import unittest

ROOT=Path(__file__).resolve().parents[2]


class CDEventByteTests(unittest.TestCase):
    def test_pin_and_alias_removed(self):
        source=(ROOT/'src/main/cdrom/CdRom_ProcessEventByte.c').read_text()
        self.assertNotIn('asm("$',source)
        self.assertNotIn('asm("D_8009B588")',source)
        self.assertIn('extern DsDecodedEventFlags D_8009B588;',source)
        self.assertEqual(len(re.findall(r'asm volatile',source)),2)

    @unittest.skipUnless((ROOT/'assets/USA/main.exe').is_file() and
                         (ROOT/'build/USA/main.exe').is_file(),'retail/build unavailable')
    def test_decode_and_copy(self):
        try:
            from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN, UC_HOOK_CODE
            from unicorn import mips_const as R
        except ImportError:
            self.skipTest('unicorn unavailable')
        images=[(ROOT/p).read_bytes() for p in ('assets/USA/main.exe','build/USA/main.exe')]
        base,size=0x8008080C,0xB0
        offset=base-0x8000F800
        self.assertEqual(images[0][offset:offset+size],images[1][offset:offset+size])
        start,external,stop,stack=0x8009B534,0x80100020,0x80010000,0x801F0000
        initial=random.Random(base).randbytes(0x90)
        saved=[getattr(R,f'UC_MIPS_REG_S{i}') for i in range(8)]+[R.UC_MIPS_REG_GP,R.UC_MIPS_REG_FP]
        for image in images:
            m=Uc(UC_ARCH_MIPS,UC_MODE_MIPS32|UC_MODE_LITTLE_ENDIAN);m.mem_map(0,0x200000)
            def put(a,data):m.mem_write(a&0x1FFFFFFF,bytes(data))
            def read(a,n):return bytes(m.mem_read(a&0x1FFFFFFF,n))
            put(0x8000F800,image)
            calls=[]
            def hook(machine,pc,size,user):
                if pc==0x80080998:
                    calls.append((machine.reg_read(R.UC_MIPS_REG_A0),machine.reg_read(R.UC_MIPS_REG_A1)))
            m.hook_add(UC_HOOK_CODE,hook)
            for value,event,length,data in itertools.product(range(256),(2,5,0x105),(0,1,4,8,0xFFFFFFFF),(external,0x8009B588,0x8009B563)):
                put(start,initial)
                put(0x8009B558,b'\0')
                put(0x8009B624,struct.pack('<I',length))
                put(external-0x20,b'\xA5'*0x50)
                program=bytes([value]*8)
                put(data,program)
                expected={start+i:v for i,v in enumerate(read(start,len(initial)))}
                expected.update({external-0x20+i:v for i,v in enumerate(read(external-0x20,0x50))})
                raw_index=0 if event&255==5 else (length-1)&0xFFFFFFFF
                valid=raw_index<0x80000000
                if valid:
                    byte=expected[data+raw_index]
                    for index,bit in enumerate((7,6,5,1)):expected[0x8009B588+index]=(byte>>bit)&1
                    expected[0x8009B56C]=byte
                    # Util_Copy8 copies forwards byte-by-byte, not via a snapshot.
                    for i in range(8):expected[0x8009B564+i]=expected[data+i]
                calls.clear()
                for name,v in (('A0',event),('A1',data),('SP',stack),('RA',stop)):
                    m.reg_write(getattr(R,'UC_MIPS_REG_'+name),v)
                for i,r in enumerate(saved):m.reg_write(r,0xABCD0000+i)
                m.emu_start(base,stop,count=300)
                case=(value,event,length,data)
                self.assertEqual(calls,[(0x8009B564,data)] if valid else [],case)
                for p,n in ((start,len(initial)),(external-0x20,0x50)):
                    self.assertEqual(read(p,n),bytes(expected[p+i] for i in range(n)),case)
                self.assertEqual(read(0x8009B624,4),struct.pack('<I',length),case)
                self.assertEqual(m.reg_read(R.UC_MIPS_REG_PC),stop,case)
                self.assertEqual(m.reg_read(R.UC_MIPS_REG_SP),stack,case)
                for i,r in enumerate(saved):self.assertEqual(m.reg_read(r),0xABCD0000+i,case)


if __name__=='__main__':unittest.main()
