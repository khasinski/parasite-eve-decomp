"""Pitch command constraints and mutating-bank callback model."""
from pathlib import Path
import itertools
import random
import struct
import unittest

ROOT = Path(__file__).resolve().parents[2]


class Seq3PitchCommandTests(unittest.TestCase):
    def test_immediate_value_pin_is_gone(self):
        source = (ROOT/'src/main/akao/seq3.c').read_text()
        immediate = source.split('void Seq_SetTrackPitchImmediate(',1)[1].split('void Seq_SlideTrackPitch(',1)[0]
        self.assertNotRegex(immediate, r'\b(?:asm|__asm__)\b')
        self.assertLessEqual(source.count('asm("$'),4)

    @unittest.skipUnless((ROOT/'assets/USA/main.exe').is_file() and
                         (ROOT/'build/USA/main.exe').is_file(), 'retail/build unavailable')
    def test_pitch_commands_and_mutating_bank_callback(self):
        try:
            from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN, UC_HOOK_CODE
            from unicorn import mips_const as R
        except ImportError:
            self.skipTest('unicorn unavailable')
        base,size = 0x8008B168,0x450
        offset = base-0x8000F800
        retail = (ROOT/'assets/USA/main.exe').read_bytes()[offset:offset+size]
        candidate = (ROOT/'build/USA/main.exe').read_bytes()[offset:offset+size]
        self.assertEqual(len(retail),size)
        self.assertEqual(retail,candidate)
        syms = candidate_syms = {
            'Seq_SetTrackPitchImmediate':0x8008B1FC,
            'Seq_SlideTrackPitch':0x8008B2CC,
            'Seq_TrackPitchSetup':0x8008B410,
            'Seq_MarkDirtyTracks':0x8008AB9C,
            'g_AkaoCurTrack':0x8009D2C8,
            'g_AkaoVoiceStateTable':0x800B8AC0,
            'g_AkaoVoiceStateTable2':0x800BA560,
        }
        banks,args,stop,stack=0x80100020,0x80110020,0x80010000,0x801F0000
        saved=[getattr(R,f'UC_MIPS_REG_S{i}') for i in range(8)]+[R.UC_MIPS_REG_GP,R.UC_MIPS_REG_FP]
        rng=random.Random(base)
        functions=('Seq_SetTrackPitchImmediate','Seq_SlideTrackPitch','Seq_TrackPitchSetup')
        def signed(value): return value if value<0x80000000 else value-0x100000000
        def divide(left,right): return (abs(left)//abs(right))*(-1 if (left<0)!=(right<0) else 1)
        for case,(function,selector,duration,values,current,mutate) in enumerate(itertools.product(functions,(0,7,9,11),(0,1,2,-1,32768,0x10001),((0,127),(127,0),(0x180,0x1FF),(1,64)),(0,0x7F0000,0xF0000000),(False,True))):
            initial=bytearray(rng.randbytes(0x440))
            for bank_offset,identity in ((0,7),(0x68,9)):
                struct.pack_into('<H',initial,0x20+bank_offset+0x54,identity)
                struct.pack_into('<I',initial,0x20+bank_offset+0x48,current)
            message=struct.pack('<IIIII',0x1234,duration&0xFFFFFFFF,values[0],values[1],selector)
            selected=0 if selector in (0,7) else 0x68 if selector==9 else None
            expected=bytearray(initial)
            if selected is not None:
                off=0x20+selected
                if function==functions[0]:
                    struct.pack_into('<I',expected,off+0x48,(duration&127)<<16)
                    struct.pack_into('<H',expected,off+0x50,0)
                else:
                    start=signed(current)
                    if function==functions[2]:
                        start=(values[0]&127)<<16
                        struct.pack_into('<I',expected,off+0x48,start)
                    target=(values[1 if function==functions[2] else 0]&127)<<16
                    delta=divide(target-start,duration or 1)
                    struct.pack_into('<H',expected,off+0x50,(duration or 1)&65535)
                    struct.pack_into('<I',expected,off+0x4C,delta&0xFFFFFFFF)
            outputs=[]
            for body,symbols_ in ((retail,syms),(candidate,candidate_syms)):
                m=Uc(UC_ARCH_MIPS,UC_MODE_MIPS32|UC_MODE_LITTLE_ENDIAN);m.mem_map(0,0x200000)
                def put(a,data): m.mem_write(a&0x1FFFFFFF,bytes(data))
                def read(a,n): return bytes(m.mem_read(a&0x1FFFFFFF,n))
                def word(a,value): put(a,struct.pack('<I',value&0xFFFFFFFF))
                def get(a): return int.from_bytes(read(a,4),'little')
                put(base,body);put(banks-0x20,initial)
                guard=b'\xA5'*0x20;put(args-0x20,guard+message+guard)
                word(syms['g_AkaoCurTrack'],banks)
                callback=syms['Seq_MarkDirtyTracks'];put(callback,struct.pack('<III',0,0x03E00008,0))
                events=[]
                def hook(uc,pc,width,user):
                    if pc!=callback+4: return
                    pointer=get(syms['g_AkaoCurTrack'])
                    events.append((uc.reg_read(R.UC_MIPS_REG_A0),pointer,read(banks-0x20,len(initial))))
                    if mutate:
                        word(pointer+0x48,get(pointer+0x48)^0x13579BDF)
                        word(syms['g_AkaoCurTrack'],banks+0x200)
                    for reg in ('V0','V1','A0','A1','A2','A3','T0','T1','T2','T3','T4','T5','T6','T7','T8','T9'):
                        uc.reg_write(getattr(R,'UC_MIPS_REG_'+reg),0xDEADCAFE)
                m.hook_add(UC_HOOK_CODE,hook)
                for name,value in (('A0',args),('SP',stack),('RA',stop)): m.reg_write(getattr(R,'UC_MIPS_REG_'+name),value)
                for i,reg in enumerate(saved): m.reg_write(reg,0xABCD0000+i)
                m.emu_start(symbols_[function],stop,count=1000)
                assert m.reg_read(R.UC_MIPS_REG_PC)==stop and m.reg_read(R.UC_MIPS_REG_SP)==stack,case
                for i,reg in enumerate(saved): assert m.reg_read(reg)==0xABCD0000+i,(case,reg)
                want=[] if selected is None else [(syms['g_AkaoVoiceStateTable2' if selected else 'g_AkaoVoiceStateTable'],banks+selected,bytes(expected))]
                assert events==want,(case,function,selector,duration,values,current,mutate)
                assert read(args-0x20,len(message)+0x40)==guard+message+guard,case
                finalpointer=banks if selected is None or not mutate else banks+0x200-(0x68 if selected else 0)
                assert get(syms['g_AkaoCurTrack'])==finalpointer,case
                outputs.append((events,read(banks-0x20,len(initial)),get(syms['g_AkaoCurTrack'])))
            assert outputs[0]==outputs[1],case


if __name__ == '__main__': unittest.main()

