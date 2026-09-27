"""Plain nested-start TU gate and differential alias/callback model."""
from pathlib import Path
import itertools
import random
import re
import struct
import unittest

ROOT = Path(__file__).resolve().parents[2]


class SeqStartNestedTrackTests(unittest.TestCase):
    def test_entire_translation_unit_is_plain_c(self):
        source = (ROOT/'src/main/akao/Akao_NestedTrack.c').read_text()
        source = re.sub(r'/\*.*?\*/|//[^\n]*','',source,flags=re.S)
        self.assertNotRegex(source,r'\b(?:asm|__asm__|INCLUDE_ASM|CC_POSTPASS)\b')

    @unittest.skipUnless((ROOT/'assets/USA/main.exe').is_file() and
                         (ROOT/'build/USA/main.exe').is_file(), 'retail/build unavailable')
    def test_retail_and_built_start(self):
        try:
            from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN, UC_HOOK_CODE
            from unicorn import mips_const as R
        except ImportError:
            self.skipTest('unicorn unavailable')
        # Exact gate covers all four functions. The behavioral model here
        # covers nested start; initialization/management have separate tests.
        images = [(ROOT/p).read_bytes() for p in ('assets/USA/main.exe','build/USA/main.exe')]
        offset,size = 0x8008A354-0x8000F800,0xB40
        self.assertEqual(len(images[0][offset:offset+size]),size)
        self.assertEqual(images[0][offset:offset+size],images[1][offset:offset+size])
        entry = base = 0x8008A750
        stop,stack = 0x80010000,0x801F0000
        track,input_buffer,script = 0x80100010,0x80110000,0x80120000
        slots,globals_base,control_addr,init = 0x800BC000,0x800BCD50,0x8009D2DC,0x80089F58
        retail = (ROOT/'assets/USA/main.exe').read_bytes()[entry-0x8000F800:entry-0x8000F800+0x17C]
        compiled = (ROOT/'build/USA/main.exe').read_bytes()[entry-0x8000F800:entry-0x8000F800+0x17C]
        self.assertEqual(len(retail),0x17C)
        self.assertEqual(compiled,retail)
        saved = [getattr(R,f'UC_MIPS_REG_S{i}') for i in range(8)]+[R.UC_MIPS_REG_GP,R.UC_MIPS_REG_FP]
        rng = random.Random(entry)
        cases = itertools.product((0,1,0x1000,0x80000000,0xFFFFFFFF),(0,2,0xFFFFFFFF),(0,0x555,0xFFF),(None,0x20,0x24),(False,True))
        for case,(mask,control,busy,alias,mutate) in enumerate(cases):
            track_data = bytearray(rng.randbytes(0x160))
            input_data = rng.randbytes(0x40)
            slot_data = bytearray(rng.randbytes(0xD50))
            for i in range(12):
                flags = rng.getrandbits(32)&~0x2000000
                if busy&(1<<i): flags |= 0x2000000
                struct.pack_into('<I',slot_data,i*0x11C+0x2C,flags)
            global_data = rng.randbytes(0x30)
            source_ptr = input_buffer if alias is None else track+alias
            outputs = []
            for address,body in ((entry,retail),(base,compiled)):
                m = Uc(UC_ARCH_MIPS,UC_MODE_MIPS32|UC_MODE_LITTLE_ENDIAN)
                m.mem_map(0,0x200000)
                def put(addr,data): m.mem_write(addr&0x1FFFFFFF,bytes(data))
                def read(addr,size): return bytes(m.mem_read(addr&0x1FFFFFFF,size))
                def word(addr,value): put(addr,struct.pack('<I',value&0xFFFFFFFF))
                put(address,body)
                put(track-0x10,track_data)
                put(input_buffer,input_data)
                put(script,b'\x5A'*0x20)
                put(slots,slot_data)
                put(globals_base,global_data)
                word(control_addr,control)
                put(init,struct.pack('<III',0,0x03E00008,0))
                events = []
                def hook(machine,pc,size,user):
                    if pc == init+4:
                        args = tuple(machine.reg_read(getattr(R,'UC_MIPS_REG_'+reg)) for reg in ('A0','A1'))
                        assert args == (track,script),(case,args)
                        events.append((args,read(track-0x10,len(track_data))))
                        if mutate:
                            word(track+0xF4,0xCAFE1234)
                            word(control_addr,control^2)
                            for offset in range(0,0x28,4):
                                word(globals_base+offset,int.from_bytes(read(globals_base+offset,4),'little')^0x55555555)
                            word(slots+0x2C,int.from_bytes(read(slots+0x2C,4),'little')^0x2000000)
                        for reg in ('V0','V1','A0','A1','A2','A3','T0','T1','T2','T3','T4','T5','T6','T7','T8','T9'):
                            machine.reg_write(getattr(R,'UC_MIPS_REG_'+reg),0xDEADCAFE)
                m.hook_add(UC_HOOK_CODE,hook)
                for name,value in (('A0',track),('A1',source_ptr),('A2',mask),('A3',script),('SP',stack),('RA',stop)):
                    m.reg_write(getattr(R,'UC_MIPS_REG_'+name),value)
                for i,reg in enumerate(saved): m.reg_write(reg,0xABCD0000+i)
                m.emu_start(entry,stop,count=1000)
                assert m.reg_read(R.UC_MIPS_REG_PC)==stop and m.reg_read(R.UC_MIPS_REG_SP)==stack,case
                for i,reg in enumerate(saved): assert m.reg_read(reg)==0xABCD0000+i,case
                assert len(events)==1 and read(input_buffer,len(input_data))==input_data,case
                assert read(script,0x20)==b'\x5A'*0x20,case
                outputs.append((events,read(track-0x10,len(track_data)),read(slots,len(slot_data)),read(globals_base,len(global_data)),read(control_addr,4)))
            assert outputs[0]==outputs[1],case


if __name__ == '__main__': unittest.main()
