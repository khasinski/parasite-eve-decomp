"""Actor/event-bank dispatch: constraint ratchet and callback regression model."""
from pathlib import Path
import itertools
import random
import struct
import unittest

ROOT = Path(__file__).resolve().parents[2]


class LoadVoiceBankTests(unittest.TestCase):
    def test_redundant_constraints_are_gone(self):
        source = (ROOT/'src/main/akao/Akao_LoadVoiceBank.c').read_text()
        self.assertNotIn('__asm__', source)
        self.assertLessEqual(source.count('asm("$'), 1)
        self.assertIn('extern AkaoVoiceActor *D_8009D254;', source)
        self.assertIn('extern int D_8009D1A0;', source)

    @unittest.skipUnless((ROOT/'assets/USA/main.exe').is_file() and
                         (ROOT/'build/USA/main.exe').is_file(), 'retail/build unavailable')
    def test_event_bank_callback_trace(self):
        try:
            from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN, UC_HOOK_CODE
            from unicorn import mips_const as R
        except ImportError:
            self.skipTest('unicorn unavailable')
        base = entry = 0x8006A318
        offset, size = base-0x8000F800, 0x2A4
        retail = (ROOT/'assets/USA/main.exe').read_bytes()[offset:offset+size]
        candidate = (ROOT/'build/USA/main.exe').read_bytes()[offset:offset+size]
        self.assertEqual(len(retail), size)
        self.assertEqual(retail, candidate)
        actor,stop,stack=0x80100020,0x80010000,0x801F0000
        regions=((actor-0x20,0x80),(0x80094468,0x100),(0x800B0CB8,0x60),(0x8009D180,0x100))
        saved=[getattr(R,f'UC_MIPS_REG_S{i}') for i in range(8)]+[R.UC_MIPS_REG_GP,R.UC_MIPS_REG_FP]
        rng=random.Random(base)
        calls=0
        for case,(step,pair,gate,dynamic,slot,mutate) in enumerate(itertools.product((-1,0,1),((0,0),(8,12),(12,8),(0,65535),(65535,0)),range(5),(0,1,4),(0,1),(False,True))):
            initial=[rng.randbytes(length) for _,length in regions]
            outputs=[]
            for body,start in ((retail,base),(candidate,entry)):
                m=Uc(UC_ARCH_MIPS,UC_MODE_MIPS32|UC_MODE_LITTLE_ENDIAN);m.mem_map(0,0x200000)
                def put(a,data): m.mem_write(a&0x1FFFFFFF,bytes(data))
                def read(a,n): return bytes(m.mem_read(a&0x1FFFFFFF,n))
                def word(a): return int.from_bytes(read(a,4),'little')
                def state(): return tuple(read(a,n) for a,n in regions)
                put(base,body)
                for (a,n),data in zip(regions,initial): put(a,data)
                put(actor+0x0C,bytes((1,2,3,4)))
                put(actor+0x16,struct.pack('<H',pair[0]));put(actor+0x1A,struct.pack('<Hi',pair[1],step))
                for off,val in ((0x2A,-32768),(0x2E,-1),(0x32,32767)): put(actor+off,struct.pack('<h',val))
                put(0x8009D254,struct.pack('<I',actor if gate!=1 else actor+0x80))
                put(0x8009D1A0,struct.pack('<I',2 if gate==2 else 0))
                put(0x800B0CD8,struct.pack('<I',0x800000 if gate==3 else 0))
                put(0x800B0CE9,bytes((dynamic,slot)))
                for i in range(8):
                    frame=(7,8,9,11,12,13,0,255)[i]
                    put(0x80094488+i*8,struct.pack('<BBBBHH',1,2,3 if (case+i)%4 else 9,frame,0 if i==2 else 100+i,200+i))
                callback=0x8006DCE4
                put(callback,struct.pack('<III',0,0x03E00008,0))
                events=[]
                def hook(uc,pc,n,user):
                    if pc!=callback+4: return
                    args=[uc.reg_read(getattr(R,f'UC_MIPS_REG_A{i}')) for i in range(4)]
                    args.append(word(uc.reg_read(R.UC_MIPS_REG_SP)+16))
                    events.append((args,state()))
                    if mutate:
                        put(0x800B0CEA,bytes((len(events)%2,)))
                        put(0x800B0CE9,bytes((4,)))
                        put(actor+0x2E,struct.pack('<h',len(events)))
                        put(0x80094488+7*8+4,struct.pack('<HH',300+len(events),400+len(events)))
                    for reg in ('V0','V1','A0','A1','A2','A3','T0','T1','T2','T3','T4','T5','T6','T7','T8','T9'):
                        uc.reg_write(getattr(R,'UC_MIPS_REG_'+reg),0xDEADCAFE)
                m.hook_add(UC_HOOK_CODE,hook)
                for name,val in (('A0',0 if gate==4 else actor),('SP',stack),('RA',stop)): m.reg_write(getattr(R,'UC_MIPS_REG_'+name),val)
                for i,reg in enumerate(saved): m.reg_write(reg,0xABCD0000+i)
                m.emu_start(start,stop,count=10000)
                assert m.reg_read(R.UC_MIPS_REG_PC)==stop and m.reg_read(R.UC_MIPS_REG_SP)==stack,case
                for i,reg in enumerate(saved): assert m.reg_read(reg)==0xABCD0000+i,(case,reg)
                assert m.reg_read(R.UC_MIPS_REG_V0)==(0xFFFFFFFF if gate==4 else 0),case
                outputs.append((events,state(),m.reg_read(R.UC_MIPS_REG_V0)))
            assert outputs[0]==outputs[1],(case,step,pair,gate,dynamic,slot,mutate)
            calls+=len(outputs[0][0])
        assert calls>0


if __name__ == '__main__': unittest.main()

