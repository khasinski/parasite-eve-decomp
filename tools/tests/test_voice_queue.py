"""Partial voice-queue constraint removal with mutating callback coverage."""
from pathlib import Path
import itertools
import random
import struct
import unittest

ROOT = Path(__file__).resolve().parents[2]


class VoiceQueueTests(unittest.TestCase):
    def test_global_memory_barrier_is_removed(self):
        source = (ROOT/'src/main/akao/Akao_ProcessVoiceQueue.c').read_text()
        self.assertNotIn('asm("" : : : "memory")',source)
        self.assertLessEqual(source.count('asm("$'),1)

    @unittest.skipUnless((ROOT/'assets/USA/main.exe').is_file() and
                         (ROOT/'build/USA/main.exe').is_file(), 'retail/build unavailable')
    def test_queue_callbacks_and_state(self):
        try:
            from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN, UC_HOOK_CODE
            from unicorn import mips_const as R
        except ImportError:
            self.skipTest('unicorn unavailable')
        base = 0x80089328
        offset,size = base-0x8000F800,0x3FC
        retail = (ROOT/'assets/USA/main.exe').read_bytes()[offset:offset+size]
        compiled = (ROOT/'build/USA/main.exe').read_bytes()[offset:offset+size]
        self.assertEqual(len(retail),size)
        self.assertEqual(retail,compiled)
        retail_syms = candidate_syms = {
            'Akao_ProcessVoiceQueue':0x80089328,
            'Akao_SetMasterVolume':0x80089f28,
            'Akao_SetVoiceAdsr':0x80089b48,
            'Akao_SetVoiceKeyOn':0x80088980,
            'Akao_SetVoiceStartAddr':0x80089d10,
            'Akao_SetVoiceVolume':0x80089980,
            'Akao_StepVoiceNote':0x8008900c,
            'Akao_UpdateVoiceEnvelopes':0x80089250,
            'Akao_WriteVoiceParam':0x800878f0,
            'D_800BCD58':0x800bcd58,
            'g_AkaoCurTrack':0x8009d2c8,
            'g_AkaoTrack5ATransposeValue':0x800bcd78,
            'g_AkaoVoiceChannelTable':0x800bc000,
            'g_AkaoVoiceMaskScratch':0x800bcd54,
            'g_AkaoVoiceStateTable':0x800b8ac0,
            'g_AkaoVoiceStateTable2':0x800ba560,
            'g_AkaoVoiceUpdateFlags':0x8009d2c4,
            'g_SpuActiveVoiceMask':0x800bcd50,
            'g_SpuStoppedVoiceMask':0x800bcd60,
            'Spu_WriteFmEnable':0x8008777c,
            'Spu_WriteKeyOn':0x8008770c,
            'Spu_WriteNoiseEnable':0x80087760,
            'Spu_WriteReverbEnable':0x80087744,
            'SpuSetNoiseClock':0x80089eb8,
        }
        stop,stack,banks = 0x80010000,0x801F0000,0x80100020
        saved = [getattr(R,f'UC_MIPS_REG_S{i}') for i in range(8)]+[R.UC_MIPS_REG_GP,R.UC_MIPS_REG_FP]
        rng = random.Random(base)
        channel = retail_syms['g_AkaoVoiceChannelTable']
        functions = {'Akao_UpdateVoiceEnvelopes':1,'Akao_StepVoiceNote':4,'Akao_SetVoiceKeyOn':2,'Akao_WriteVoiceParam':3,'Akao_SetMasterVolume':2,'SpuSetNoiseClock':1,'Akao_SetVoiceAdsr':0,'Akao_SetVoiceVolume':0,'Akao_SetVoiceStartAddr':0,'Spu_WriteReverbEnable':1,'Spu_WriteNoiseEnable':1,'Spu_WriteFmEnable':1,'Spu_WriteKeyOn':1}
        coverage = set()
        for case,(updates,channels,pattern,mutate) in enumerate(itertools.product((0,0x10,0x80,0x100,0x190),(0,0x1000,0xA000,0xF000),range(4),(False,True))):
            bank_data = bytearray(rng.randbytes(0x110))
            for i in range(2):
                for offset,value in ((4,(0,0xF,0xA,0xF)[pattern]),(8,(0,1,0xA,5)[(pattern+i)%4]),(12,(0,0xF,5,0xA)[pattern]),(16,(0,0xF,5,0xA)[(pattern+i)%4]),(20,0xF)):
                    struct.pack_into('<I',bank_data,0x20+i*0x68+offset,value)
            channel_data = bytearray(rng.randbytes(4*0x11C+0x20))
            for i in range(4):
                struct.pack_into('<II',channel_data,i*0x11C+0xF0,i+12,(i+case)%2)
            global_data = rng.randbytes(0x30)
            control_data = rng.randbytes(0x10)
            reverb_data = rng.randbytes(0x10)
            outputs = []
            for body,syms in ((retail,retail_syms),(compiled,candidate_syms)):
                m = Uc(UC_ARCH_MIPS,UC_MODE_MIPS32|UC_MODE_LITTLE_ENDIAN)
                m.mem_map(0,0x200000)
                def put(addr,data): m.mem_write(addr&0x1FFFFFFF,bytes(data))
                def read(addr,n): return bytes(m.mem_read(addr&0x1FFFFFFF,n))
                def word(addr,value): put(addr,struct.pack('<I',value&0xFFFFFFFF))
                def get(addr): return int.from_bytes(read(addr,4),'little')
                put(base,body)
                put(banks-0x20,bank_data)
                put(channel,channel_data)
                put(0x800BCD50,global_data)
                put(0x8009D2C0,control_data)
                put(0x800C0DD0,reverb_data)
                word(retail_syms['g_AkaoCurTrack'],banks)
                word(retail_syms['g_AkaoVoiceUpdateFlags'],updates)
                word(retail_syms['g_SpuActiveVoiceMask'],channels)
                word(retail_syms['D_800BCD58'],0xF000)
                word(retail_syms['g_SpuStoppedVoiceMask'],(0,1,5,0xF)[pattern])
                word(retail_syms['g_AkaoVoiceMaskScratch'],0x123000)
                put(retail_syms['g_AkaoTrack5ATransposeValue'],struct.pack('<H',0x8123))
                callbacks = {}
                for name,n in functions.items():
                    address = retail_syms[name]
                    put(address,struct.pack('<III',0,0x03E00008,0))
                    callbacks[address+4] = (name,n)
                events = []
                def hook(machine,pc,size,user):
                    if pc not in callbacks: return
                    name,n = callbacks[pc]
                    args = tuple(machine.reg_read(getattr(R,'UC_MIPS_REG_'+reg)) for reg in ('A0','A1','A2','A3')[:n])
                    current = get(retail_syms['g_AkaoCurTrack'])
                    assert current in (banks,banks+0x68),(case,name,current)
                    # Result is a stack local whose exact offset may change.
                    logged = args[:3] if name=='Akao_StepVoiceNote' else args
                    events.append((name,logged,current,read(banks-0x20,len(bank_data))))
                    coverage.add(name)
                    if name=='Akao_StepVoiceNote':
                        assert args[0] in (retail_syms['g_AkaoVoiceStateTable'],retail_syms['g_AkaoVoiceStateTable2']),(case,args)
                        word(args[3],get(args[3]) | args[1])
                    if name=='Akao_SetVoiceKeyOn':
                        assert channel <= args[0] < channel+4*0x11C,(case,args)
                        if mutate: word(args[0]+0xF4,get(args[0]+0xF4)^3)
                    if mutate:
                        if name in ('Akao_UpdateVoiceEnvelopes','Akao_StepVoiceNote'):
                            word(current+8,get(current+8)^3)
                            word(current+16,get(current+16)^5)
                        if name=='Akao_WriteVoiceParam': word(args[1]+4,0)
                        if name=='Akao_SetMasterVolume':
                            word(retail_syms['g_AkaoVoiceUpdateFlags'],get(retail_syms['g_AkaoVoiceUpdateFlags'])^0x100)
                    for reg in ('V0','V1','A0','A1','A2','A3','T0','T1','T2','T3','T4','T5','T6','T7','T8','T9'):
                        machine.reg_write(getattr(R,'UC_MIPS_REG_'+reg),0xDEADCAFE)
                m.hook_add(UC_HOOK_CODE,hook)
                m.reg_write(R.UC_MIPS_REG_SP,stack)
                m.reg_write(R.UC_MIPS_REG_RA,stop)
                for i,reg in enumerate(saved): m.reg_write(reg,0xABCD0000+i)
                m.emu_start(syms['Akao_ProcessVoiceQueue'],stop,count=10000)
                assert m.reg_read(R.UC_MIPS_REG_PC)==stop and m.reg_read(R.UC_MIPS_REG_SP)==stack,case
                for i,reg in enumerate(saved): assert m.reg_read(reg)==0xABCD0000+i,case
                assert get(retail_syms['g_AkaoCurTrack'])==banks,case
                assert read(banks-0x20,0x20)==bank_data[:0x20] and read(banks+0xD0,0x20)==bank_data[0xF0:],case
                assert read(0x800C0DD0,0x10)==reverb_data,case
                outputs.append((events,read(banks-0x20,len(bank_data)),read(channel,len(channel_data)),read(0x800BCD50,0x30),read(0x8009D2C0,0x10)))
            assert outputs[0]==outputs[1],(case,updates,channels,pattern,mutate)
        assert coverage == set(functions),(coverage,set(functions)-coverage)



if __name__ == '__main__': unittest.main()

