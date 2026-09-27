"""Nested stream bank copy/reload model; external routines are controlled stubs."""
from pathlib import Path
import itertools
import re
import struct
import unittest

ROOT = Path(__file__).resolve().parents[2]


class AkaoNestedStreamStartTests(unittest.TestCase):
    def test_translation_unit_is_plain_c(self):
        source = (ROOT/'src/main/akao/Akao_NestedStreamStart.c').read_text()
        source = re.sub(r'/\*.*?\*/|//[^\n]*', '', source, flags=re.S)
        self.assertNotRegex(source, r'\b(?:asm|__asm__|INCLUDE_ASM|CC_POSTPASS)\b')
        self.assertIn('&g_AkaoVoiceStateTable[AKAO_VOICE_COUNT]', source)

    @unittest.skipUnless((ROOT/'assets/USA/main.exe').is_file() and
                         (ROOT/'build/USA/main.exe').is_file(), 'retail/build unavailable')
    def test_retail_and_built_copy_model(self):
        try:
            from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN, UC_HOOK_CODE
            from unicorn import mips_const as R
        except ImportError:
            self.skipTest('unicorn unavailable')
        entry, stop, stack = 0x8008AFB8, 0x80010000, 0x801F0000
        copy, step, current = 0x8008D820, 0x8008A068, 0x8009D2C8
        state, replacement, packet = 0x80100000, 0x80101000, 0x80102000
        voices, bank_size = 0x800B8AC0, 0x1AA0
        offset, size = entry-0x8000F800, 0x1B0
        bodies = [(ROOT/path).read_bytes()[offset:offset+size]
                  for path in ('assets/USA/main.exe', 'build/USA/main.exe')]
        # Exact gate covers all five functions in the translation unit; the
        # behavioral model below covers only the changed bank-copy function.
        self.assertEqual(len(bodies[0]), size)
        self.assertEqual(*bodies)
        cases = itertools.product((0,1,0x80000000,0xFFFFFF), (0,1,0xFFFFFFFF),
                                  (0,0x1234,0xFEDCBA98), (0,0x80112340), (False,True))
        for active,secondary,parent,script,mutate in cases:
            initial_state = bytearray((i*13+7)&255 for i in range(0xE0))
            struct.pack_into('<I',initial_state,4,active)
            struct.pack_into('<I',initial_state,0x6C,secondary)
            initial_voices = bytearray((i*17+i//256)&255 for i in range(2*bank_size+0x20))
            initial_replacement = bytearray([0xA5]*0x80)
            should_copy = bool(active) and secondary == 0
            expected_state = bytearray(initial_state)
            expected_voices = bytearray(initial_voices)
            expected_replacement = bytearray(initial_replacement)
            if should_copy:
                expected_state[0x68:0xD0] = initial_state[:0x68]
                expected_voices[bank_size:2*bank_size] = initial_voices[:bank_size]
            expected_parent = (parent ^ 0xAA55) if mutate else parent
            struct.pack_into('<H',expected_replacement if mutate else expected_state,0x54,expected_parent&0xFFFF)
            expected_events = [('copy',state,state+0x68,0x68),
                               ('copy',voices,voices+bank_size,bank_size)] if should_copy else []
            expected_events += [('step',script)]
            for body in bodies:
                m = Uc(UC_ARCH_MIPS,UC_MODE_MIPS32|UC_MODE_LITTLE_ENDIAN)
                m.mem_map(0,0x200000)
                def put(address,data): m.mem_write(address&0x1FFFFFFF,bytes(data))
                def word(address,value): put(address,struct.pack('<I',value))
                put(entry,body)
                put(state,initial_state)
                put(replacement,initial_replacement)
                put(voices,initial_voices)
                put(packet,struct.pack('<IIIII',0xBAD0,script,0xBAD2,parent,0xBAD4))
                word(current,state)
                for address in (copy,step): put(address,struct.pack('<III',0,0x03E00008,0))
                for name,value in (('A0',packet),('RA',stop),('SP',stack),('GP',0x8009CD70),('FP',0xACDC1234)):
                    m.reg_write(getattr(R,'UC_MIPS_REG_'+name),value)
                for i in range(8): m.reg_write(getattr(R,f'UC_MIPS_REG_S{i}'),0xABCD0000+i)
                events = []
                def hook(machine,address,insn_size,user):
                    if address not in (copy+4,step+4): return
                    args = tuple(machine.reg_read(getattr(R,f'UC_MIPS_REG_A{i}')) for i in range(3))
                    if address == copy+4:
                        events.append(('copy',)+args)
                        self.assertIn(args,((state,state+0x68,0x68),(voices,voices+bank_size,bank_size)))
                        put(args[1],machine.mem_read(args[0]&0x1FFFFFFF,args[2]))
                    else:
                        events.append(('step',args[0]))
                        if mutate:
                            word(current,replacement)
                            word(packet+12,expected_parent)
                    for reg in ('V0','V1','A0','A1','A2','A3','T0','T1','T2','T3','T4','T5','T6','T7','T8','T9'):
                        machine.reg_write(getattr(R,'UC_MIPS_REG_'+reg),0xDEADCAFE)
                m.hook_add(UC_HOOK_CODE,hook)
                m.emu_start(entry,stop,count=200)
                self.assertEqual(events,expected_events)
                for address,expected in ((state,expected_state),(replacement,expected_replacement),(voices,expected_voices)):
                    self.assertEqual(bytes(m.mem_read(address&0x1FFFFFFF,len(expected))),expected)
                self.assertEqual(int.from_bytes(m.mem_read(current&0x1FFFFFFF,4),'little'),replacement if mutate else state)
                self.assertEqual(bytes(m.mem_read(packet&0x1FFFFFFF,20)),struct.pack('<IIIII',0xBAD0,script,0xBAD2,expected_parent,0xBAD4))
                self.assertEqual(m.reg_read(R.UC_MIPS_REG_PC),stop)
                self.assertEqual(m.reg_read(R.UC_MIPS_REG_SP),stack)
                self.assertEqual(m.reg_read(R.UC_MIPS_REG_GP),0x8009CD70)
                self.assertEqual(m.reg_read(R.UC_MIPS_REG_FP),0xACDC1234)
                for i in range(8): self.assertEqual(m.reg_read(getattr(R,f'UC_MIPS_REG_S{i}')),0xABCD0000+i)


if __name__ == '__main__':
    unittest.main()
