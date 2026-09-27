"""Scene audio transition states, callbacks, delays and retail bytes."""
from pathlib import Path
from importlib.util import find_spec
import itertools
import struct
import unittest

ROOT = Path(__file__).resolve().parents[2]
BASE, SIZE, TABLE = 0x8006D60C, 1340, 0x80011508
STATE, INDEX, START, DELAY, CLOCK = 0x800B0CD8, 0x8009D188, 0x8009D18C, 0x8009D190, 0x8009CDA4
F1, F0, STOP, PLAY = 0x80087024, 0x80086FF8, 0x800864CC, 0x80086464
SEND, FADE, VOLUME = 0x8006DF50, 0x80086C5C, 0x80086C1C
READ, FIND, SEEK, REGISTER = 0x8006CDA4, 0x8006D078, 0x8006D2B8, 0x8006DB48
ARG_COUNTS = {F1:0, F0:0, STOP:0, PLAY:1, SEND:5, FADE:3, VOLUME:2,
              READ:6, FIND:0, SEEK:5, REGISTER:4}


class StepReadTests(unittest.TestCase):
    @unittest.skipUnless((ROOT/'assets/USA/main.exe').is_file() and
                         (ROOT/'build/USA/main.exe').is_file() and find_spec('unicorn'),
                         'images or unicorn unavailable')
    def test_model_and_exact_bytes(self):
        from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN, UC_HOOK_CODE
        from unicorn import mips_const as R
        images = [(ROOT/p).read_bytes() for p in ('assets/USA/main.exe','build/USA/main.exe')]
        def section(image, address, length):
            offset = address - 0x8000F800
            return image[offset:offset+length]
        self.assertEqual(section(images[0],BASE,SIZE), section(images[1],BASE,SIZE))
        self.assertEqual(section(images[0],TABLE,260), section(images[1],TABLE,260))
        phases = (0,44,45,46,47,48,49,50,51,62,63,64,65,255)
        cases = list(itertools.product(phases, (0,4,0x40,0x44,0x400044), (0,1),
                                       range(4), range(4), range(3), (0,1)))
        ranges = ((STATE-8,0x1A8), (INDEX-8,28), (CLOCK-8,20))
        saved = [getattr(R,'UC_MIPS_REG_S'+str(i)) for i in range(8)] + [R.UC_MIPS_REG_FP]
        def signed(value, bits): return value-(1<<bits) if value & (1<<(bits-1)) else value
        for label, image in zip(('retail','candidate'), images):
            m = Uc(UC_ARCH_MIPS, UC_MODE_MIPS32 | UC_MODE_LITTLE_ENDIAN)
            m.mem_map(0,0x200000)
            def put(a,b): m.mem_write(a & 0x1fffffff,bytes(b))
            def read(a,n): return bytes(m.mem_read(a & 0x1fffffff,n))
            def ag(a,n=4): return int.from_bytes(read(a,n),'little')
            def aset(a,v,n=4): put(a,(v & ((1<<(8*n))-1)).to_bytes(n,'little'))
            put(BASE,section(image,BASE,SIZE)); put(TABLE,section(image,TABLE,260))
            for a in ARG_COUNTS: put(a,struct.pack('<II',0x03e00008,0))
            actual_events = []
            def environment(call,args,g,s,events):
                # Stack addresses are checked separately, not compared to model storage.
                events.append((call, args[:3]+args[4:] if call == SEEK else args))
                if mutation:
                    if call == SEND: s(STATE+0x130, 0)
                    if call == F0: s(STATE,g(STATE)^0x40)
                    if call in (FIND,FADE): s(CLOCK,g(CLOCK)+3)
                    if call == PLAY:
                        s(STATE+0xFE,0x5b,1); s(STATE+0xDC,-7,1)
                if call in (READ,FIND,SEEK): return (0,1,-1,0)[scenario]
                if call == PLAY: return (0,0,2,-1)[scenario]
                return 0
            def hook(uc,a,size,data):
                if a not in ARG_COUNTS: return
                args = tuple(uc.reg_read(getattr(R,'UC_MIPS_REG_A'+str(i))) if i<4 else
                             ag(uc.reg_read(R.UC_MIPS_REG_SP)+0x10+4*(i-4))
                             for i in range(ARG_COUNTS[a]))
                if a == SEEK:
                    self.assertEqual(args[3],0x801effe8)
                    aset(args[3],1)
                result = environment(a,args,ag,aset,actual_events)
                for name in ('V0','V1','A0','A1','A2','A3','T0','T1','T2','T3','T4','T5','T6','T7','T8','T9'):
                    uc.reg_write(getattr(R,'UC_MIPS_REG_'+name),0xcccccccc)
                uc.reg_write(R.UC_MIPS_REG_V0,result & 0xffffffff)
            m.hook_add(UC_HOOK_CODE,hook)
            for initial,flags,active,scenario,timing,banks,mutation in cases:
                regions = [(a,bytearray(b'\xa5'*n)) for a,n in ranges]
                def region(a,n):
                    for base,b in regions:
                        if base<=a and a+n<=base+len(b): return b,a-base
                    raise AssertionError(hex(a))
                def g(a,n=4):
                    b,i=region(a,n); return int.from_bytes(b[i:i+n],'little')
                def s(a,v,n=4):
                    b,i=region(a,n); b[i:i+n]=(v & ((1<<(8*n))-1)).to_bytes(n,'little')
                s(STATE,flags|0x10000); s(STATE+0xF2,initial,1)
                s(STATE+0x130,0 if banks==0 else 0x80120000)
                s(STATE+0x124,0x80121000); s(STATE+0x128,0x80122000)
                s(STATE+0x194,0x80130000); s(STATE+0xFE,0x7f,1)
                s(STATE+0xDC,0x80,1); s(STATE+0xDA,0xfe,1)
                s(STATE+0xE0,0x80 if banks==1 else 127,1); s(STATE+0xE1,0xff,1)
                s(STATE+0xE8,-1 if banks==0 else 0x8000,2)
                s(STATE+0xEA,0 if banks==0 else 3,1); s(STATE+0xEB,255,1)
                s(INDEX,min(banks,1) if initial==49 else banks)
                s(START,100); s(CLOCK,100+(0,54,60,61)[timing])
                s(DELAY,(-1,0,1,8)[timing])
                for a,b in regions: put(a,b)
                expected_events=[]; actual_events.clear()
                def invoke(call,*args):
                    return environment(call,tuple(v & 0xffffffff for v in args),g,s,expected_events)
                def fade():
                    s(START,g(CLOCK)); invoke(FADE,0,60,0)
                def countdown():
                    remaining=signed((60-(g(CLOCK)-g(START))) & 0xffffffff,32)
                    s(DELAY,max(0,min(8,remaining)))
                    if remaining>8: invoke(FADE,0,16,0)
                def read_bank(kind,bank,channel=0):
                    return invoke(READ,kind,bank,channel,g(STATE+0x194),0x21,0)
                result=0
                for step in range(30):
                    phase=g(STATE+0xF2,1)
                    next_phase=None
                    if phase==0:
                        s(INDEX,0); invoke(F1)
                        if active:
                            if not(g(STATE)&0x400000) and g(STATE+0x130):
                                invoke(SEND,g(STATE+0x130),0x451,0,0x80,0x7f)
                                if g(STATE+0x130): invoke(SEND,g(STATE+0x130),0x452,0,0x80,0x7f)
                            next_phase=44
                        else:
                            if g(STATE)&4: fade()
                            next_phase=45
                    elif phase==44:
                        if g(STATE+0xE0,1)!=g(STATE+0xDC,1):
                            if g(STATE)&0x40: fade()
                            s(STATE,g(STATE)|4)
                        next_phase=46
                    elif phase==46:
                        if invoke(FIND)==1: result=1; break
                        if (g(STATE)&0x44)==0x44: countdown()
                        next_phase=63
                    elif phase==63:
                        next_phase=48
                        if g(STATE)&4:
                            if g(STATE)&0x40:
                                if signed(g(DELAY),32)>0:
                                    s(DELAY,g(DELAY)-1); result=1; break
                                invoke(STOP); invoke(F0)
                            next_phase=47
                    elif phase==47:
                        if read_bank(0,signed(g(STATE+0xE1,1),8))==1: result=1; break
                        next_phase=48
                    elif phase==48:
                        invoke(PLAY,g(STATE+0x124)); invoke(VOLUME,0,127)
                        s(STATE+0xF2,0,1); break
                    elif phase==45:
                        next_phase=50
                        if signed(g(INDEX),32)<2:
                            if g(STATE+0xEA+g(INDEX),1): next_phase=49
                            else: s(INDEX,g(INDEX)+1)
                    elif phase==49:
                        if read_bank(2,g(STATE+0xEA+g(INDEX),1),g(INDEX))==1: result=1; break
                        s(INDEX,g(INDEX)+1); next_phase=45
                    elif phase==50:
                        bank=signed(g(STATE+0xE8,2),16)
                        if bank!=-1 and read_bank(1,bank)==1: result=1; break
                        if g(STATE)&4: countdown()
                        next_phase=64
                    elif phase==64:
                        if g(STATE)&4:
                            if signed(g(DELAY),32)>0:
                                s(DELAY,g(DELAY)-1); result=1; break
                            invoke(F0)
                            if g(STATE)&0x40:
                                s(STATE,g(STATE)&~0x40); next_phase=62
                        if next_phase is None:
                            s(STATE,g(STATE)&~4); s(STATE+0xF2,0,1); break
                    elif phase==62:
                        if read_bank(0,signed(g(STATE+0xDA,1),8))==1: result=1; break
                        next_phase=51
                    elif phase==51:
                        if invoke(SEEK,signed(g(STATE+0xDC,1),8),1,0,0x801effe8,0)==1: result=1; break
                        voice=invoke(PLAY,g(STATE+0x128))
                        if voice==-1:
                            s(STATE+0xF2,62,1); result=1; break
                        invoke(FADE,voice,60,g(STATE+0xFE,1))
                        if voice: invoke(REGISTER,0,signed(g(STATE+0xDC,1),8),voice,g(STATE+0xFE,1))
                        s(STATE+0xF2,0,1); s(STATE,(g(STATE)&~4)|0x40); break
                    else: break
                    s(STATE+0xF2,next_phase,1)
                else: self.fail('model did not terminate')
                for name,value in (('A0',active),('SP',0x801f0000),('GP',0x8009CD70),('RA',0x80010000)):
                    m.reg_write(getattr(R,'UC_MIPS_REG_'+name),value)
                for i,reg in enumerate(saved): m.reg_write(reg,0xabcd0000+i)
                m.emu_start(BASE,0x80010000,count=20000)
                case=(label,initial,flags,active,scenario,timing,banks,mutation)
                self.assertEqual(m.reg_read(R.UC_MIPS_REG_V0),result,case)
                self.assertEqual(actual_events,expected_events,case)
                for a,b in regions: self.assertEqual(read(a,len(b)),b,case)
                self.assertEqual(m.reg_read(R.UC_MIPS_REG_PC),0x80010000,case)
                self.assertEqual(m.reg_read(R.UC_MIPS_REG_SP),0x801f0000,case)
                for i,reg in enumerate(saved): self.assertEqual(m.reg_read(reg),0xabcd0000+i,case)


if __name__=='__main__': unittest.main()
