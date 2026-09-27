"""Slide tick model, including cursor mutation by the two callees."""
from pathlib import Path
from importlib.util import find_spec
import itertools
import struct
import unittest

ROOT = Path(__file__).resolve().parents[2]
BASE, SIZE = 0x8008D844, 824
PITCH, DIRTY = 0x8008D7D0, 0x8008AB9C
BANKS, VOICES, SFX = 0x80110010, 0x800B8AC0, 0x800BC000
CURSOR, MASK = 0x8009D2C8, 0x800BCD50


class SlideTickTests(unittest.TestCase):
    @unittest.skipUnless((ROOT/'assets/USA/main.exe').is_file() and
                         (ROOT/'build/USA/main.exe').is_file() and
                         find_spec('unicorn'), 'images or unicorn unavailable')
    def test_model_and_exact_bytes(self):
        from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN, UC_HOOK_CODE
        from unicorn import mips_const as R
        start = BASE - 0x8000F800
        retail, code = [(ROOT/p).read_bytes()[start:start+SIZE]
                        for p in ('assets/USA/main.exe','build/USA/main.exe')]
        self.assertEqual(code,retail)
        self.assertEqual(len(code),SIZE)
        cases=list(itertools.product((0,3,65535),(0,1,-1,-32768),
            (0,0x1000,0x800000,0xfff000),(0,1,0x10000,0xffffffff),(0,1)))
        ranges=((0x8009CDE0,0x510),(BANKS-8,8*0x68+16),(VOICES-8,60*0x11c+16))
        saved=[getattr(R,'UC_MIPS_REG_S'+str(i)) for i in range(8)]+[R.UC_MIPS_REG_FP]
        for label,body in (('retail',retail),('candidate',code)):
            m=Uc(UC_ARCH_MIPS,UC_MODE_MIPS32|UC_MODE_LITTLE_ENDIAN);m.mem_map(0,0x200000)
            def put(a,b):m.mem_write(a&0x1fffffff,bytes(b))
            def read(a,n):return bytes(m.mem_read(a&0x1fffffff,n))
            def actual_get(a,n=4,signed=False):return int.from_bytes(read(a,n),'little',signed=signed)
            def actual_set(a,v,n=4):put(a,(v&((1<<(n*8))-1)).to_bytes(n,'little'))
            put(BASE,body)
            for call in (PITCH,DIRTY):put(call,struct.pack('<II',0x03e00008,0))
            actual_events=[]
            def callback(g,s,events,call,arg):
                cursor=g(CURSOR)
                events.append((call,arg if call==DIRTY else None,cursor,
                    g(0x8009D2A2,2),g(0x8009D2B4),g(cursor+0x50,2),g(cursor+0x48)))
                if mutation:
                    if call==PITCH:
                        s(CURSOR,BANKS+2*0x68)
                        s(0x8009D220,2,2)
                        s(0x8009D210,0x10001)
                    else:
                        s(CURSOR,cursor+0x68)
                        s(cursor+0x68+0x48,0x99887766)
            def hook(uc,a,size,data):
                if a not in (PITCH,DIRTY):return
                callback(actual_get,actual_set,actual_events,a,uc.reg_read(R.UC_MIPS_REG_A0))
                for name in ('V0','V1','A0','A1','A2','A3','T0','T1','T2','T3','T4','T5','T6','T7','T8','T9'):
                    uc.reg_write(getattr(R,'UC_MIPS_REG_'+name),0xcccccccc)
            m.hook_add(UC_HOOK_CODE,hook)
            for tick,counter,mask,delta,mutation in cases:
                regions=[(a,bytearray(b'\xa5'*n)) for a,n in ranges]
                def region(a,n):
                    for base,b in regions:
                        if base<=a and a+n<=base+len(b):return b,a-base
                    raise AssertionError(hex(a))
                def g(a,n=4,signed=False):
                    b,i=region(a,n);return int.from_bytes(b[i:i+n],'little',signed=signed)
                def s(a,v,n=4):
                    b,i=region(a,n);b[i:i+n]=(v&((1<<(n*8))-1)).to_bytes(n,'little')
                s(0x8009CDEC,tick,2);s(CURSOR,BANKS);s(MASK,mask)
                for address in (0x8009D2A2,0x8009D220,0x8009D21E):s(address,counter,2)
                for address in (0x8009D2B4,0x8009D2D0,0x8009D2CC):s(address,0x7fffff)
                for address in (0x8009D284,0x8009D214,0x8009D210):s(address,delta)
                for i in range(8):
                    bank=BANKS+i*0x68
                    s(bank+4,0 if i%3==1 and delta==0 else 1)
                    s(bank+0x48,0x7fffff+i);s(bank+0x4c,delta);s(bank+0x50,counter,2)
                for i in range(12):
                    voice=SFX+i*0x11c
                    for off in (0x70,0x74,0x78):s(voice+off,(counter+i)&0xffff,2)
                    s(voice+0xd8,0x7fff-i,2);s(voice+0xda,delta,2)
                    s(voice+0x76,0xffff-i,2);s(voice+0xdc,delta,2)
                    s(voice+0x3c,0x7fffffff-i);s(voice+0x40,delta)
                    s(voice+0xf4,0x80000000)
                for address,b in regions:put(address,b)
                expected_events=[];actual_events.clear()
                s(0x8009CDEC,g(0x8009CDEC,2)+1,2)
                if not (g(0x8009CDEC,2)&3):
                    for ticks,value,step,call in ((0x8009D2A2,0x8009D2B4,0x8009D284,PITCH),
                                                 (0x8009D220,0x8009D2D0,0x8009D214,None)):
                        if g(ticks,2):
                            s(ticks,g(ticks,2)-1,2);s(value,g(value)+g(step))
                            if call:callback(g,s,expected_events,call,None)
                    if g(0x8009D21E,2):
                        old=g(0x8009D2CC);value=(old+g(0x8009D210))&0xffffffff
                        s(0x8009D21E,g(0x8009D21E,2)-1,2)
                        if (old&0xff0000)!=(value&0xff0000):
                            for i in range(24):
                                a=VOICES+i*0x11c+0xf4;s(a,g(a)|16)
                        s(0x8009D2CC,value)
                    for index in range(2):
                        bank=g(CURSOR)
                        if g(bank+4) and g(bank+0x50,2):
                            old=g(bank+0x48);value=(old+g(bank+0x4c))&0xffffffff
                            s(bank+0x50,g(bank+0x50,2)-1,2)
                            if (old&0x7f0000)!=(value&0x7f0000):
                                callback(g,s,expected_events,DIRTY,VOICES+index*24*0x11c)
                            s(g(CURSOR)+0x48,value)
                        if index==0:s(CURSOR,g(CURSOR)+0x68)
                    s(CURSOR,g(CURSOR)-0x68)
                    for i in range(12):
                        if not(mask&(0x1000<<i)):continue
                        voice=SFX+i*0x11c
                        for ticks,value,step,width,is_signed,flags in (
                            (0x74,0xd8,0xda,2,True,3),(0x78,0x76,0xdc,2,False,3),
                            (0x70,0x3c,0x40,4,False,16)):
                            if g(voice+ticks,2):
                                old=g(voice+value,width,is_signed)
                                new=old+g(voice+step,width,width==2)
                                s(voice+ticks,g(voice+ticks,2)-1,2)
                                if (old&0xff00)!=(new&0xff00):s(voice+0xf4,g(voice+0xf4)|flags)
                                s(voice+value,new,width)
                for name,value in (('SP',0x801f0000),('GP',0x8009CD70),('RA',0x80010000)):
                    m.reg_write(getattr(R,'UC_MIPS_REG_'+name),value)
                for i,r in enumerate(saved):m.reg_write(r,0xabcd0000+i)
                m.emu_start(BASE,0x80010000,count=20000)
                case=(label,tick,counter,mask,delta,mutation)
                for address,b in regions:self.assertEqual(read(address,len(b)),b,case)
                self.assertEqual(actual_events,expected_events,case)
                self.assertEqual(m.reg_read(R.UC_MIPS_REG_SP),0x801f0000,case)
                self.assertEqual(m.reg_read(R.UC_MIPS_REG_PC),0x80010000,case)
                for i,r in enumerate(saved):self.assertEqual(m.reg_read(r),0xabcd0000+i,case)


if __name__ == '__main__': unittest.main()
