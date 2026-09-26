"""Queue field-write/OT-read ordering and callback-visible countdown updates."""
from pathlib import Path
import re
import struct
import unittest

ROOT = Path(__file__).resolve().parents[2]


class GpuQueuePrimitiveTests(unittest.TestCase):
    def test_queue_function_has_no_assembly(self):
        source = (ROOT/'src/main/gpu/Gpu_SetupSprites.c').read_text()
        source = source.split('void Gpu_QueuePrimitive(void) {',1)[1].split('\n#include',1)[0]
        source = re.sub(r'/\*.*?\*/|//[^\n]*','',source,flags=re.S)
        self.assertNotRegex(source,r'\b(?:asm|__asm__)\b')

    @unittest.skipUnless((ROOT/'build/USA/main.exe').is_file() and
                         (ROOT/'assets/USA/main.exe').is_file(), 'retail/build unavailable')
    def test_retail_and_built_queue_model(self):
        try:
            from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN, UC_HOOK_CODE, UC_HOOK_MEM_WRITE
            from unicorn import mips_const as R
        except ImportError:
            self.skipTest('unicorn unavailable')
        entry, stop, stack, callback = 0x80033430,0x80010000,0x801F0000,0x80077AC4
        offset = entry-0x8000F800
        bodies = [(ROOT/path).read_bytes()[offset:offset+124] for path in ('assets/USA/main.exe','build/USA/main.exe')]
        self.assertEqual(*bodies)
        for index in (0,1):
            for y in (0,1,2,0x7FFF,0x8000,0xFFFF):
                for countdown in (0,1,30,255):
                    for mutate in (False,True):
                        primitive = 0x8009EC38+index*28
                        field = primitive+18
                        slot = 0x800B0E38+index*4
                        expected_ot = 0x80120000 if mutate else 0x80110000
                        expected_count = ((73 if mutate else countdown)-1)&255
                        for body in bodies:
                            m = Uc(UC_ARCH_MIPS,UC_MODE_MIPS32|UC_MODE_LITTLE_ENDIAN)
                            m.mem_map(0,0x200000)
                            def put(address,data): m.mem_write(address&0x1FFFFFFF,bytes(data))
                            def word(address,value): put(address,struct.pack('<I',value&0xFFFFFFFF))
                            put(entry,body)
                            put(callback,struct.pack('<III',0,0x03E00008,0))
                            initial = bytearray(range(40))
                            struct.pack_into('<H',initial,22,y)
                            put(primitive-4,initial)
                            word(0x8009CDDC,index)
                            word(slot,0x80110000)
                            put(0x8009D235,bytes([countdown]))
                            for name,value in (('RA',stop),('SP',stack),('GP',0x8009CD70)):
                                m.reg_write(getattr(R,'UC_MIPS_REG_'+name),value)
                            for i in range(8): m.reg_write(getattr(R,f'UC_MIPS_REG_S{i}'),0xABCD0000+i)
                            events = []
                            def write_hook(machine,access,address,size,value,data):
                                if (address&0x1FFFFFFF)==(field&0x1FFFFFFF):
                                    events.append(('write',size,value&65535))
                                    if mutate: word(slot,expected_ot)
                            def code_hook(machine,address,size,data):
                                if address!=callback+4: return
                                events.append(('add',machine.reg_read(R.UC_MIPS_REG_A0),machine.reg_read(R.UC_MIPS_REG_A1)))
                                self.assertEqual(bytes(machine.mem_read(field&0x1FFFFFFF,2)),struct.pack('<H',(y-2)&65535))
                                self.assertEqual(bytes(machine.mem_read(0x9D235,1)),bytes([countdown]))
                                if mutate: put(0x8009D235,b'\x49')
                                for reg in ('V0','V1','A0','A1','A2','A3','T0','T1','T2','T3','T4','T5','T6','T7','T8','T9'):
                                    machine.reg_write(getattr(R,'UC_MIPS_REG_'+reg),0xDEADCAFE)
                            m.hook_add(UC_HOOK_MEM_WRITE,write_hook)
                            m.hook_add(UC_HOOK_CODE,code_hook)
                            m.emu_start(entry,stop,count=200)
                            self.assertEqual(events,[('write',2,(y-2)&65535),('add',expected_ot+28,primitive)])
                            expected = bytearray(initial)
                            struct.pack_into('<H',expected,22,(y-2)&65535)
                            self.assertEqual(bytes(m.mem_read((primitive-4)&0x1FFFFFFF,40)),bytes(expected))
                            self.assertEqual(bytes(m.mem_read(0x9D235,1)),bytes([expected_count]))
                            self.assertEqual(m.reg_read(R.UC_MIPS_REG_PC),stop)
                            self.assertEqual(m.reg_read(R.UC_MIPS_REG_SP),stack)
                            self.assertEqual(m.reg_read(R.UC_MIPS_REG_GP),0x8009CD70)
                            for i in range(8): self.assertEqual(m.reg_read(getattr(R,f'UC_MIPS_REG_S{i}')),0xABCD0000+i)


if __name__=='__main__':
    unittest.main()
