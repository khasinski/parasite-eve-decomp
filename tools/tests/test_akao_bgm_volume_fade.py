"""BGM command result/reload model; the audio command itself is stubbed."""
from pathlib import Path
import itertools
import re
import struct
import unittest

ROOT = Path(__file__).resolve().parents[2]


class AkaoBgmVolumeFadeTests(unittest.TestCase):
    def test_translation_unit_is_plain_c(self):
        source = (ROOT/'src/main/akao/Akao_SetBgmVolumeFade.c').read_text()
        source = re.sub(r'/\*.*?\*/|//[^\n]*', '', source, flags=re.S)
        self.assertNotRegex(source, r'\b(?:asm|__asm__|INCLUDE_ASM|CC_POSTPASS)\b')
        self.assertIn('value = Akao_SendTableCommand(', source)

    @unittest.skipUnless((ROOT/'assets/USA/main.exe').is_file() and
                         (ROOT/'build/USA/main.exe').is_file(), 'retail/build unavailable')
    def test_retail_and_built_model(self):
        try:
            from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN, UC_HOOK_CODE
            from unicorn import mips_const as R
        except ImportError:
            self.skipTest('unicorn unavailable')
        entry, stop, stack, gp = 0x8005270C, 0x80010000, 0x801F0000, 0x8009CD70
        command, handle, pending = 0x8006DF50, 0x800B0E08, 0x8009D01C
        offset, size = entry-0x8000F800, 0x58
        bodies = [(ROOT/path).read_bytes()[offset:offset+size]
                  for path in ('assets/USA/main.exe', 'build/USA/main.exe')]
        self.assertEqual(len(bodies[0]), size)
        self.assertEqual(*bodies)
        cases = itertools.product((0,0x80110000,0xFFFFFFFF),
                                  (0,1,127,128,0x7FFFFFFF,0x80000000,0xFFFFFFFF,0x12345678),
                                  (None,0,0x80110004),(False,True))
        for initial,result,reload,mutate_callback in cases:
            argument = initial if reload is None else reload
            expected_calls = [(argument,0x450,0x100,0x80,0x7F)] if initial else []
            expected_pending = result if initial else 0
            expected_handle = (0x80112340 if mutate_callback else argument) if initial else initial
            for body in bodies:
                m = Uc(UC_ARCH_MIPS,UC_MODE_MIPS32|UC_MODE_LITTLE_ENDIAN)
                m.mem_map(0,0x200000)
                def put(address,data): m.mem_write(address&0x1FFFFFFF,bytes(data))
                def word(address,value): put(address,struct.pack('<I',value))
                put(entry,body)
                put(command,struct.pack('<III',0,0x03E00008,0))
                word(handle,initial)
                word(pending,0xA5A5A5A5)
                for name,value in (('RA',stop),('SP',stack),('GP',gp),('FP',0xACDC1234)):
                    m.reg_write(getattr(R,'UC_MIPS_REG_'+name),value)
                for i in range(8): m.reg_write(getattr(R,f'UC_MIPS_REG_S{i}'),0xABCD0000+i)
                events = []
                def hook(machine,address,insn_size,user):
                    # After the first handle has selected the nonzero branch,
                    # update the volatile slot before its second load.
                    if address==entry+0x20:
                        if reload is not None: word(handle,reload)
                    elif address==command+4:
                        args = tuple(machine.reg_read(getattr(R,f'UC_MIPS_REG_A{i}')) for i in range(4))
                        fifth = int.from_bytes(machine.mem_read((machine.reg_read(R.UC_MIPS_REG_SP)+16)&0x1FFFFFFF,4),'little')
                        events.append(args+(fifth,))
                        if mutate_callback:
                            word(handle,0x80112340)
                            word(pending,0xCAFECAFE)
                        for reg in ('V0','V1','A0','A1','A2','A3','T0','T1','T2','T3','T4','T5','T6','T7','T8','T9'):
                            machine.reg_write(getattr(R,'UC_MIPS_REG_'+reg),0xDEADCAFE)
                        machine.reg_write(R.UC_MIPS_REG_V0,result)
                m.hook_add(UC_HOOK_CODE,hook)
                m.emu_start(entry,stop,count=100)
                self.assertEqual(events,expected_calls)
                self.assertEqual(int.from_bytes(m.mem_read(pending&0x1FFFFFFF,4),'little'),expected_pending)
                self.assertEqual(int.from_bytes(m.mem_read(handle&0x1FFFFFFF,4),'little'),expected_handle)
                self.assertEqual(m.reg_read(R.UC_MIPS_REG_PC),stop)
                self.assertEqual(m.reg_read(R.UC_MIPS_REG_SP),stack)
                self.assertEqual(m.reg_read(R.UC_MIPS_REG_GP),gp)
                self.assertEqual(m.reg_read(R.UC_MIPS_REG_FP),0xACDC1234)
                for i in range(8): self.assertEqual(m.reg_read(getattr(R,f'UC_MIPS_REG_S{i}')),0xABCD0000+i)


if __name__ == '__main__':
    unittest.main()
