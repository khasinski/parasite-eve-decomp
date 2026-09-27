"""Plain SPU register TU gate and dispatcher/native-helper access traces."""
from pathlib import Path
import itertools
import random
import re
import struct
import unittest

ROOT = Path(__file__).resolve().parents[2]


class SpuVoiceParamDispatchTests(unittest.TestCase):
    def test_entire_translation_unit_is_plain_c(self):
        source = (ROOT/'src/main/akao/Akao_SpuVoiceRegisters.c').read_text()
        source = re.sub(r'/\*.*?\*/|//[^\n]*','',source,flags=re.S)
        self.assertNotRegex(source,r'\b(?:asm|__asm__|INCLUDE_ASM|CC_POSTPASS)\b')

    @unittest.skipUnless((ROOT/'assets/USA/main.exe').is_file() and
                         (ROOT/'build/USA/main.exe').is_file(), 'retail/build unavailable')
    def test_dispatch_and_register_access_traces(self):
        try:
            from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN, UC_HOOK_MEM_READ, UC_HOOK_MEM_WRITE, UC_MEM_WRITE
            from unicorn import mips_const as R
        except ImportError:
            self.skipTest('unicorn unavailable')
        base = 0x8008770C
        offset,size = base-0x8000F800,0x39C
        retail = (ROOT/'assets/USA/main.exe').read_bytes()[offset:offset+size]
        compiled = (ROOT/'build/USA/main.exe').read_bytes()[offset:offset+size]
        self.assertEqual(len(retail),size)
        self.assertEqual(retail,compiled)
        # Calls execute native helper bodies. MMIO is mapped RAM: we check
        # address/width/value/order, not physical SPU timing or side effects.
        retail_syms = candidate_syms = {
            'Spu_WriteKeyOn':0x8008770C,
            'Spu_WriteKeyOff':0x80087728,
            'Spu_WriteReverbEnable':0x80087744,
            'Spu_WriteNoiseEnable':0x80087760,
            'Spu_WriteFmEnable':0x8008777C,
            'AkaoSpuVoice_SetVolume':0x80087798,
            'AkaoSpuVoice_SetPitch':0x800877BC,
            'AkaoSpuVoice_SetStartAddress':0x800877D4,
            'AkaoSpuVoice_SetRepeatAddress':0x800877F0,
            'AkaoSpuVoice_SetAdsrAttack':0x8008780C,
            'AkaoSpuVoice_SetAdsrDecayRate':0x8008783C,
            'AkaoSpuVoice_SetAdsrSustainLevel':0x80087864,
            'AkaoSpuVoice_SetAdsrSustainRate':0x8008788C,
            'AkaoSpuVoice_SetAdsrReleaseRate':0x800878C0,
            'Akao_WriteVoiceParam':0x800878F0,
        }
        stop,stack,mmio = 0x80010000,0x801F0000,0x1F801000
        saved = [getattr(R,f'UC_MIPS_REG_S{i}') for i in range(8)]+[R.UC_MIPS_REG_GP,R.UC_MIPS_REG_FP]
        rng = random.Random(base)
        values = (0,1,127,255,0x7FFF,0xFFFF,0x80000000,0xFFFFFFFF)
        names = ('SetVolume','SetPitch','SetStartAddress','SetRepeatAddress','SetAdsrAttack','SetAdsrDecayRate','SetAdsrSustainLevel','SetAdsrSustainRate','SetAdsrReleaseRate')
        cases = [('AkaoSpuVoice_'+name,(index,value,value^0xFFFFFFFF),None) for name,index,value in itertools.product(names,range(24),values)]
        cases += [(name,(value,0,0),None) for name,value in itertools.product(('Spu_WriteKeyOn','Spu_WriteKeyOff','Spu_WriteReverbEnable','Spu_WriteNoiseEnable','Spu_WriteFmEnable'),values)]
        params = 0x80100020
        flag_values = [0,0xFFFFFFFF]+[1<<i for i in range(32)]+[rng.getrandbits(32) for _ in range(256)]
        cases += [('Akao_WriteVoiceParam',(index,params,0),flags) for index,flags in itertools.product((0,12,23),flag_values)]
        for case,(name,args,flags) in enumerate(cases):
            initial = rng.randbytes(0x1000)
            param_data = bytearray(rng.randbytes(0x80))
            if flags is not None: struct.pack_into('<I',param_data,0x24,flags)
            outputs = []
            for body,syms in ((retail,retail_syms),(compiled,candidate_syms)):
                m = Uc(UC_ARCH_MIPS,UC_MODE_MIPS32|UC_MODE_LITTLE_ENDIAN)
                m.mem_map(0,0x200000)
                m.mem_map(mmio,0x1000)
                m.mem_write(base&0x1FFFFFFF,body)
                m.mem_write(mmio,initial)
                m.mem_write((params-0x20)&0x1FFFFFFF,bytes(param_data))
                events = []
                def hook(machine,kind,address,size,value,user):
                    if kind == UC_MEM_WRITE: events.append(('write',address,size,value))
                    else: events.append(('read',address,size,int.from_bytes(machine.mem_read(address,size),'little')))
                m.hook_add(UC_HOOK_MEM_READ|UC_HOOK_MEM_WRITE,hook,begin=mmio,end=mmio+0xFFF)
                for reg,val in zip(('A0','A1','A2'),args): m.reg_write(getattr(R,'UC_MIPS_REG_'+reg),val)
                m.reg_write(R.UC_MIPS_REG_SP,stack)
                m.reg_write(R.UC_MIPS_REG_RA,stop)
                for i,reg in enumerate(saved): m.reg_write(reg,0xABCD0000+i)
                m.emu_start(syms[name],stop,count=1000)
                assert m.reg_read(R.UC_MIPS_REG_PC)==stop and m.reg_read(R.UC_MIPS_REG_SP)==stack,(case,name)
                for i,reg in enumerate(saved): assert m.reg_read(reg)==0xABCD0000+i,(case,name)
                assert (events or flags is not None) and all(size in (1,2) for _,addr,size,val in events),(case,name,events)
                param_result = bytes(m.mem_read((params-0x20)&0x1FFFFFFF,0x80))
                assert param_result[:0x24]==param_data[:0x24] and param_result[0x28:]==param_data[0x28:],(case,name)
                outputs.append((events,bytes(m.mem_read(mmio,0x1000)),param_result))
            assert outputs[0]==outputs[1],(case,name,args,outputs[0][0],outputs[1][0])


if __name__ == '__main__': unittest.main()

