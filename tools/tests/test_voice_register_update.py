"""Voice-register update: scoped old values and callback/LFO regression model."""
from pathlib import Path
import itertools
import random
import struct
import unittest

ROOT = Path(__file__).resolve().parents[2]


class VoiceRegisterUpdateTests(unittest.TestCase):
    def test_shared_old_value_pin_is_gone(self):
        source = (ROOT/'src/main/akao/Spu_UpdateVoiceRegisters.c').read_text()
        self.assertNotRegex(source,r'old_value\s+asm')
        self.assertLessEqual(source.count('asm("$'),6)

    @unittest.skipUnless((ROOT/'assets/USA/main.exe').is_file() and
                         (ROOT/'build/USA/main.exe').is_file(), 'retail/build unavailable')
    def test_lfo_and_callback_state(self):
        try:
            from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN, UC_HOOK_CODE
            from unicorn import mips_const as R
        except ImportError:
            self.skipTest('unicorn unavailable')
        base = 0x80087AA8
        offset,size = base-0x8000F800,0x4F8
        retail = (ROOT/'assets/USA/main.exe').read_bytes()[offset:offset+size]
        compiled = (ROOT/'build/USA/main.exe').read_bytes()[offset:offset+size]
        self.assertEqual(len(retail),size)
        self.assertEqual(retail,compiled)
        retail_syms = candidate_syms = {
            'Spu_UpdateVoiceRegisters':base,
            'D_8009D2C4':0x8009D2C4,
            'D_8009D2C8':0x8009D2C8,
            'Seq_MarkTrack34MaskDirty':0x80089960,
            'Seq_MarkTrack3CMaskDirty':0x80089CF0,
        }
        stop,stack,voice,waves,banks = 0x80010000,0x801F0000,0x80100020,0x80110000,0x80120000
        saved = [getattr(R,f'UC_MIPS_REG_S{i}') for i in range(8)]+[R.UC_MIPS_REG_GP,R.UC_MIPS_REG_FP]
        rng = random.Random(base)
        coverage=set()
        for case,(duration,phase,sentinel,mutate,enabled,mask) in enumerate(itertools.product((0,1,2,0xFFFF),(0,1,2),(False,True),(False,True),range(8),(1,0x80000000))):
            initial=bytearray(rng.randbytes(0x160))
            for i,off in enumerate((0x72,0x60,0x6E,0x74,0x78,0x8A,0x9E,0xBA,0xBC,0x7A)):
                struct.pack_into('<H',initial,0x20+off,(duration,0,1,2)[i%4])
            for i,(dur,phaseoff,ptr) in enumerate(((0x96,0x8E,0x1C),(0xA8,0xA2,0x20),(0xB6,0xB0,0x24))):
                struct.pack_into('<H',initial,0x20+dur,duration if enabled&(1<<i) else 0)
                struct.pack_into('<H',initial,0x20+phaseoff,phase)
                struct.pack_into('<I',initial,0x20+ptr,waves+i*0x20)
            # Counter and delay gates independently reach 0/1/>1 across iterations.
            for off in (0x8A,0x9E,0xBA,0xBC): struct.pack_into('<H',initial,0x20+off,(case+off//2)%3)
            table=bytearray(rng.randbytes(0x100))
            for i in range(3):
                sample=(-32768,-1,0,1,32767)[(case+i)%5]
                if sentinel:
                    struct.pack_into('<hhh',table,i*0x20,0,0,4)
                    struct.pack_into('<h',table,i*0x20+8,sample)
                else: struct.pack_into('<hh',table,i*0x20,sample,1)
            bank_data=rng.randbytes(0x100)
            control=rng.randbytes(0x10)
            outputs=[]
            for body,syms in ((retail,retail_syms),(compiled,candidate_syms)):
                m=Uc(UC_ARCH_MIPS,UC_MODE_MIPS32|UC_MODE_LITTLE_ENDIAN)
                m.mem_map(0,0x200000)
                def put(addr,data): m.mem_write(addr&0x1FFFFFFF,bytes(data))
                def read(addr,n): return bytes(m.mem_read(addr&0x1FFFFFFF,n))
                def get(addr): return int.from_bytes(read(addr,4),'little')
                def word(addr,value): put(addr,struct.pack('<I',value&0xFFFFFFFF))
                put(base,body)
                put(voice-0x20,initial)
                put(waves,table)
                put(banks,bank_data)
                put(0x8009D2C0,control)
                word(retail_syms['D_8009D2C8'],banks)
                callbacks={}
                for name,offset in (('Seq_MarkTrack34MaskDirty',0x34),('Seq_MarkTrack3CMaskDirty',0x3C)):
                    address=retail_syms[name]
                    put(address,struct.pack('<III',0,0x03E00008,0))
                    callbacks[address+4]=(name,offset)
                events=[]
                def hook(machine,pc,size,user):
                    if pc not in callbacks: return
                    name,offset=callbacks[pc]
                    current=get(retail_syms['D_8009D2C8'])
                    events.append((name,current,read(voice-0x20,len(initial)),read(banks,len(bank_data)),get(retail_syms['D_8009D2C4'])))
                    coverage.add(name)
                    if mutate:
                        word(current+offset,get(current+offset)^0x55AA55AA)
                        word(voice+0xF4,get(voice+0xF4)^0x5510)
                        word(retail_syms['D_8009D2C8'],banks+0x80 if current==banks else banks)
                    for reg in ('V0','V1','A0','A1','A2','A3','T0','T1','T2','T3','T4','T5','T6','T7','T8','T9'):
                        machine.reg_write(getattr(R,'UC_MIPS_REG_'+reg),0xDEADCAFE)
                m.hook_add(UC_HOOK_CODE,hook)
                for name,value in (('A0',voice),('A1',mask),('SP',stack),('RA',stop)): m.reg_write(getattr(R,'UC_MIPS_REG_'+name),value)
                for i,reg in enumerate(saved): m.reg_write(reg,0xABCD0000+i)
                m.emu_start(syms['Spu_UpdateVoiceRegisters'],stop,count=3000)
                assert m.reg_read(R.UC_MIPS_REG_PC)==stop and m.reg_read(R.UC_MIPS_REG_SP)==stack,case
                for i,reg in enumerate(saved): assert m.reg_read(reg)==0xABCD0000+i,case
                assert read(waves,len(table))==table,case
                assert read(voice-0x20,0x20)==initial[:0x20] and read(voice+0x11C,0x24)==initial[0x13C:],case
                outputs.append((events,read(voice-0x20,len(initial)),read(banks,len(bank_data)),read(0x8009D2C0,0x10)))
            assert outputs[0]==outputs[1],(case,duration,phase,sentinel,mutate,enabled,mask)
        assert len(coverage)==2,coverage



if __name__ == '__main__': unittest.main()

