"""Typed voice banks: copy/transpose and callback-reload regression models."""
from pathlib import Path
import itertools
import re
import struct
import unittest

ROOT = Path(__file__).resolve().parents[2]


class AkaoVoiceBankAliasTests(unittest.TestCase):
    def test_translation_units_are_plain_c(self):
        for name in ('Akao_PlaybackBank','Akao_GlobalSlideAndVoices'):
            source = (ROOT/f'src/main/akao/{name}.c').read_text()
            source = re.sub(r'/\*.*?\*/|//[^\n]*', '', source, flags=re.S)
            self.assertNotRegex(source,r'\b(?:asm|__asm__|INCLUDE_ASM|CC_POSTPASS)\b')

    def images(self, entry, size):
        paths = [ROOT/p for p in ('assets/USA/main.exe','build/USA/main.exe')]
        if not all(p.is_file() for p in paths): self.skipTest('retail/build unavailable')
        offset = entry-0x8000F800
        bodies = [p.read_bytes()[offset:offset+size] for p in paths]
        self.assertEqual(len(bodies[0]),size)
        self.assertEqual(*bodies)
        return bodies

    def machine(self, entry, body, callbacks, callback):
        try:
            from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN, UC_HOOK_CODE
            from unicorn import mips_const as R
        except ImportError:
            self.skipTest('unicorn unavailable')
        m = Uc(UC_ARCH_MIPS,UC_MODE_MIPS32|UC_MODE_LITTLE_ENDIAN)
        m.mem_map(0,0x200000)
        self.put(m,entry,body)
        for address in callbacks: self.put(m,address,struct.pack('<III',0,0x03E00008,0))
        for name,value in (('A0',0x80102000),('RA',0x80010000),('SP',0x801F0000),('GP',0x8009CD70),('FP',0xACDC1234)):
            m.reg_write(getattr(R,'UC_MIPS_REG_'+name),value)
        for i in range(8): m.reg_write(getattr(R,f'UC_MIPS_REG_S{i}'),0xABCD0000+i)
        def hook(machine,address,size,user):
            if address-4 not in callbacks: return
            args = tuple(machine.reg_read(getattr(R,f'UC_MIPS_REG_A{i}')) for i in range(3))
            callback(machine,address-4,args)
            for reg in ('V0','V1','A0','A1','A2','A3','T0','T1','T2','T3','T4','T5','T6','T7','T8','T9'):
                machine.reg_write(getattr(R,'UC_MIPS_REG_'+reg),0xDEADCAFE)
        m.hook_add(UC_HOOK_CODE,hook)
        return m

    @staticmethod
    def put(m,address,data): m.mem_write(address&0x1FFFFFFF,bytes(data))

    @staticmethod
    def read(m,address,size): return bytes(m.mem_read(address&0x1FFFFFFF,size))

    def word(self,m,address,value): self.put(m,address,struct.pack('<I',value&0xFFFFFFFF))

    def run_and_check_abi(self,m,entry):
        from unicorn import mips_const as R
        m.emu_start(entry,0x80010000,count=2000)
        for name,value in (('PC',0x80010000),('SP',0x801F0000),('GP',0x8009CD70),('FP',0xACDC1234)):
            self.assertEqual(m.reg_read(getattr(R,'UC_MIPS_REG_'+name)),value)
        for i in range(8): self.assertEqual(m.reg_read(getattr(R,f'UC_MIPS_REG_S{i}')),0xABCD0000+i)

    def test_copy_and_pitch_transpose(self):
        # Exact gate covers both functions; behavior model covers restoration.
        bodies = self.images(0x8008AE94,0x124)
        entry, copy, current = 0x8008AF08,0x8008D820,0x8009D2C8
        state, backup_state = 0x80100000,0x800B8968
        voices, backup_voices, bank_size = 0x800B8AC0,0x800B6B80,0x1AA0
        edges = (0,0x4F,0x50,0x51,0x7F,0xFF,0x8000,0xFFFF)
        for active,flags,rotation in itertools.product((0,1,0x80000000),(0,0x100,0xFFFFFFFF),range(8)):
            state_data = bytearray((i*13+7)&255 for i in range(0x68))
            struct.pack_into('<II',state_data,0,flags,active)
            voices_data = bytearray((i*17+i//256)&255 for i in range(bank_size))
            for i in range(24): struct.pack_into('<H',voices_data,i*0x11C+0x5A,edges[(i+rotation)%8])
            expected_state = bytearray([0xA5]*0x88)
            expected_voices = bytearray([0x5A]*(bank_size+0x20))
            if active:
                expected_state[0x10:0x78] = state_data
                expected_voices[0x10:0x10+bank_size] = voices_data
                if flags&0x100:
                    for i in range(24):
                        value = edges[(i+rotation)%8]
                        struct.pack_into('<H',expected_voices,0x10+i*0x11C+0x5A,value-0x30 if value>=0x50 else value)
            expected_calls = [(state,backup_state,0x68),(voices,backup_voices,bank_size)] if active else []
            for body in bodies:
                calls = []
                def callback(m,address,args):
                    calls.append(args)
                    self.assertIn(args,expected_calls)
                    self.put(m,args[1],self.read(m,args[0],args[2]))
                m = self.machine(0x8008AE94,body,(copy,),callback)
                self.put(m,state,state_data)
                self.put(m,voices,voices_data)
                self.put(m,backup_state-0x10,bytes([0xA5])*0x88)
                self.put(m,backup_voices-0x10,bytes([0x5A])*(bank_size+0x20))
                self.word(m,current,state)
                self.run_and_check_abi(m,entry)
                self.assertEqual(calls,expected_calls)
                self.assertEqual(self.read(m,backup_state-0x10,0x88),expected_state)
                self.assertEqual(self.read(m,backup_voices-0x10,bank_size+0x20),expected_voices)
                self.assertEqual(self.read(m,state,0x68),state_data)
                self.assertEqual(self.read(m,voices,bank_size),voices_data)

    def test_dual_bank_initializer_reloads(self):
        # Whole TU exact gate also covers slides and stop-voices; model only
        # executes the two changed initializers, with Akao_InitVoices stubbed.
        bodies = self.images(0x8008C18C,0x3D0)
        init, current, packet = 0x8008A354,0x8009D2C8,0x80102000
        state, alternate = 0x80100000,0x80101000
        for with_mode,initial,reload,mutate in itertools.product((False,True),(0,1,0xFFFFFFFF),(None,0,7),(False,True)):
            entry = 0x8008C3E4 if with_mode else 0x8008C374
            effective = initial if reload is None else reload
            second = not with_mode or effective != 0
            after_first = alternate if mutate else state
            before_second = after_first+0x68
            after_second = alternate+0x200 if mutate else before_second
            expected_current = after_second-0x68 if second else after_first
            expected_calls = [(initial if with_mode else 0,0x800B8AC0,state)]
            if second: expected_calls += [(effective if with_mode else 0,0x800BA560,before_second)]
            for body in bodies:
                calls = []
                def callback(m,address,args):
                    calls.append((args[0],args[1],int.from_bytes(self.read(m,current,4),'little')))
                    if len(calls)==1 and reload is not None: self.word(m,packet+4,reload)
                    if mutate: self.word(m,current,alternate if len(calls)==1 else alternate+0x200)
                m = self.machine(0x8008C18C,body,(init,),callback)
                self.word(m,current,state)
                self.put(m,packet,struct.pack('<III',0xBAD0,initial,0xBAD2))
                self.run_and_check_abi(m,entry)
                self.assertEqual(calls,expected_calls)
                self.assertEqual(int.from_bytes(self.read(m,current,4),'little'),expected_current)
                self.assertEqual(self.read(m,packet,12),struct.pack('<III',0xBAD0,effective,0xBAD2))


if __name__ == '__main__': unittest.main()
