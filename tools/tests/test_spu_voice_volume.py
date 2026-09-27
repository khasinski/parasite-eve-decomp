"""Voice-volume packet model; the downstream SPU sender is stubbed."""
from pathlib import Path
import random
import re
import struct
import unittest

ROOT = Path(__file__).resolve().parents[2]


class SpuVoiceVolumeTests(unittest.TestCase):
    def test_translation_unit_is_plain_c(self):
        source = (ROOT/'src/main/akao/Spu_SetVoiceVolume.c').read_text()
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
        entry, stop, stack, gp = 0x800870F0, 0x80010000, 0x801F0000, 0x8009CD70
        sender, packet, mode_address = 0x8007A88C, 0x8009D1C8, 0x8009D2C0
        offset, size = entry-0x8000F800, 0xA8
        bodies = [(ROOT/path).read_bytes()[offset:offset+size]
                  for path in ('assets/USA/main.exe','build/USA/main.exe')]
        self.assertEqual(len(bodies[0]),size)
        self.assertEqual(*bodies)
        rng = random.Random(entry)
        values = list(range(256))+[256,8191,8192,0x7FFFFFFF,0x80000000,0xFFFFFFFF]
        values += [rng.getrandbits(32) for _ in range(256)]
        saved = [getattr(R,f'UC_MIPS_REG_S{i}') for i in range(8)]+[R.UC_MIPS_REG_FP]
        for body in bodies:
            m = Uc(UC_ARCH_MIPS,UC_MODE_MIPS32|UC_MODE_LITTLE_ENDIAN)
            m.mem_map(0,0x200000)
            def put(address,data): m.mem_write(address&0x1FFFFFFF,bytes(data))
            put(entry,body)
            put(sender,struct.pack('<III',0,0x03E00008,0))
            calls = []
            def hook(machine,address,insn_size,user):
                if address == sender+4:
                    calls.append((machine.reg_read(R.UC_MIPS_REG_A0),bytes(machine.mem_read(packet&0x1FFFFFFF,4))))
                    for name in ('V0','V1','A0','A1','A2','A3','T0','T1','T2','T3','T4','T5','T6','T7','T8','T9'):
                        machine.reg_write(getattr(R,'UC_MIPS_REG_'+name),0xDEADCAFE)
            m.hook_add(UC_HOOK_CODE,hook)
            for mode in (0,1,2,3,0xFFFFFFFD,0xFFFFFFFF):
                for value in values:
                    calls.clear()
                    initial = bytes(range(20))
                    put(packet-8,initial)
                    put(mode_address,struct.pack('<I',mode))
                    for name,number in (('A0',value),('SP',stack),('RA',stop),('GP',gp)):
                        m.reg_write(getattr(R,'UC_MIPS_REG_'+name),number)
                    for i,reg in enumerate(saved): m.reg_write(reg,0xABCD0000+i)
                    level = (((value*2903)&0xFFFFFFFF)>>13)&255 if mode&2 else value&255
                    expected = bytes([level]*4) if mode&2 else bytes([level,0,level,0])
                    m.emu_start(entry,stop,count=100)
                    self.assertEqual(calls,[(packet,expected)],(mode,value))
                    self.assertEqual(bytes(m.mem_read((packet-8)&0x1FFFFFFF,20)),initial[:8]+expected+initial[12:])
                    self.assertEqual(bytes(m.mem_read(mode_address&0x1FFFFFFF,4)),struct.pack('<I',mode))
                    self.assertEqual(m.reg_read(R.UC_MIPS_REG_PC),stop)
                    self.assertEqual(m.reg_read(R.UC_MIPS_REG_SP),stack)
                    self.assertEqual(m.reg_read(R.UC_MIPS_REG_GP),gp)
                    for i,reg in enumerate(saved): self.assertEqual(m.reg_read(reg),0xABCD0000+i)


if __name__ == '__main__':
    unittest.main()
