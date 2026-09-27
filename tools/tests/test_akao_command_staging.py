"""Two-command staging model; the command executor is stubbed."""
from pathlib import Path
import itertools
import re
import struct
import unittest

ROOT = Path(__file__).resolve().parents[2]


class CommandStagingTests(unittest.TestCase):
    def test_translation_unit_is_plain_c(self):
        source = (ROOT/'src/main/akao/Akao_CommandStaging.c').read_text()
        source = re.sub(r'/\*.*?\*/|//[^\n]*', '', source, flags=re.S)
        self.assertNotRegex(source, r'\b(?:asm|__asm__|INCLUDE_ASM|CC_POSTPASS)\b')

    @unittest.skipUnless((ROOT/'assets/USA/main.exe').is_file() and
                         (ROOT/'build/USA/main.exe').is_file(), 'retail/build unavailable')
    def test_retail_and_built_model(self):
        try:
            from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN, UC_HOOK_CODE
            from unicorn import mips_const as R
        except ImportError:
            self.skipTest('unicorn unavailable')
        entry,stop,stack,enqueue,staging = 0x800864F8,0x80010000,0x801F0000,0x8008CBA8,0x800BCD80
        start,size = 0x80086464,0x2C4
        bodies = [(ROOT/path).read_bytes()[start-0x8000F800:start-0x8000F800+size]
                  for path in ('assets/USA/main.exe','build/USA/main.exe')]
        self.assertEqual(len(bodies[0]),size)
        self.assertEqual(*bodies)
        cases = list(itertools.product((0,1,0xFFFFFFFF),(0,127,128,255,0x80000000,0xFFFFFFFF),
                                       (0,1,0x80000000,0xFFFFFFFF),(False,True)))
        saved = [getattr(R,f'UC_MIPS_REG_S{i}') for i in range(8)]+[R.UC_MIPS_REG_GP,R.UC_MIPS_REG_FP]
        for body in bodies:
            m = Uc(UC_ARCH_MIPS,UC_MODE_MIPS32|UC_MODE_LITTLE_ENDIAN)
            m.mem_map(0,0x200000)
            def put(address,data): m.mem_write(address&0x1FFFFFFF,bytes(data))
            put(start,body)
            put(enqueue,struct.pack('<III',0,0x03E00008,0))
            for arg0,arg1,result,mutate in cases:
                initial = (0xDEADBEEF,0x11223344,0x55667788,0xAABBCCDD,0x12345678)
                changed = (0x87654321,0xFEDCBA98,0xBEEFCAFE,0xDEADCAFE,0xA5A5A5A5)
                second = (0xC0,arg1&127,(changed if mutate else initial)[2],(changed if mutate else initial)[3],0)
                expected_calls = [(0x19,arg0,*initial[2:]),second]
                put(staging-4,b'LEFT'+struct.pack('<IIIII',*initial)+b'RGHT')
                events = []
                def hook(machine,address,size,user):
                    if address==enqueue+4:
                        events.append(struct.unpack('<IIIII',bytes(machine.mem_read(staging&0x1FFFFFFF,20))))
                        if mutate: put(staging,struct.pack('<IIIII',*changed))
                        for name in ('V0','V1','A0','A1','A2','A3','T0','T1','T2','T3','T4','T5','T6','T7','T8','T9'):
                            machine.reg_write(getattr(R,'UC_MIPS_REG_'+name),0xDEADCAFE)
                        machine.reg_write(R.UC_MIPS_REG_V0,result if len(events)==1 else result^0xFFFFFFFF)
                handle = m.hook_add(UC_HOOK_CODE,hook)
                for name,value in (('A0',arg0),('A1',arg1),('SP',stack),('RA',stop)):
                    m.reg_write(getattr(R,'UC_MIPS_REG_'+name),value)
                for i,reg in enumerate(saved): m.reg_write(reg,0xABCD0000+i)
                m.emu_start(entry,stop,count=200)
                m.hook_del(handle)
                self.assertEqual(events,expected_calls)
                self.assertEqual(bytes(m.mem_read((staging-4)&0x1FFFFFFF,28)),b'LEFT'+struct.pack('<IIIII',*(changed if mutate else second))+b'RGHT')
                self.assertEqual(m.reg_read(R.UC_MIPS_REG_V0),result)
                self.assertEqual(m.reg_read(R.UC_MIPS_REG_PC),stop)
                self.assertEqual(m.reg_read(R.UC_MIPS_REG_SP),stack)
                for i,reg in enumerate(saved): self.assertEqual(m.reg_read(reg),0xABCD0000+i)


if __name__ == '__main__':
    unittest.main()
