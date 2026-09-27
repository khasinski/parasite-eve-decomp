"""CD event state model and removal of the duplicate local symbol alias."""
from pathlib import Path
import itertools
import random
import struct
import unittest

ROOT=Path(__file__).resolve().parents[2]


class CDCommandEventTests(unittest.TestCase):
    def test_local_alias_removed(self):
        source=(ROOT/'src/main/cdrom/CdRom_CmdEventCallback.c').read_text()
        self.assertNotIn('D_8009B558_o',source)
        self.assertNotIn('__asm__',source)
        self.assertIn('extern CdRomEventCommandState D_8009B558;',source)
        self.assertEqual(source.count('asm("$'),1)

    @unittest.skipUnless((ROOT/'assets/USA/main.exe').is_file() and
                         (ROOT/'build/USA/main.exe').is_file(),'retail/build unavailable')
    def test_event_state(self):
        try:
            from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN, UC_HOOK_CODE
            from unicorn import mips_const as R
        except ImportError:
            self.skipTest('unicorn unavailable')
        base=0x8008068C
        retail=(ROOT/'assets/USA/main.exe').read_bytes()[base-0x8000F800:base-0x8000F800+0xEC]
        candidate=(ROOT/'build/USA/main.exe').read_bytes()[base-0x8000F800:base-0x8000F800+0xEC]
        self.assertEqual(len(retail),0xEC)
        self.assertEqual(retail,candidate)
        start,callback,result,stop,stack=0x8009B534,0x80010040,0x80100020,0x80010000,0x801F0000
        saved=[getattr(R,f'UC_MIPS_REG_S{i}') for i in range(8)]+[R.UC_MIPS_REG_GP,R.UC_MIPS_REG_FP]
        rng=random.Random(base);initial=rng.randbytes(0x90);result_initial=rng.randbytes(0x48)
        cases=list(itertools.product((0,2,5,0x102,0x105,0xFFFFFFFF),(0,14,15,255),(0,128),(0,128),(0,0x10),(0,1),(False,True)))
        cases += [(2,pending,0,128,0,0,False) for pending in range(256)]
        for case,(event,pending,old,new,flag,enabled,has_callback) in enumerate(cases):
            before=bytearray(initial)
            def setat(data,address,value,width=4):
                data[address-start:address-start+width]=(value&((1<<(8*width))-1)).to_bytes(width,'little')
            setat(before,0x8009B554,enabled)
            setat(before,0x8009B558,pending,1);setat(before,0x8009B559,new,1)
            setat(before,0x8009B56C,flag,1);setat(before,0x8009B581,old,1)
            expected=bytearray(before);expected_result=bytearray(result_initial)
            fire=False
            if event&255==2 and pending==14:
                if (old^new)&128:
                    setat(expected,0x8009B578,15);setat(expected,0x8009B574,2);setat(expected,0x8009B594,3)
                setat(expected,0x8009B581,new,1)
            if event&255==5:
                setat(expected,0x8009B574,2 if flag else 1)
                setat(expected,0x8009B578,12 if flag else 11)
                fire=bool(flag and enabled and has_callback)
            for body in (retail,candidate):
                m=Uc(UC_ARCH_MIPS,UC_MODE_MIPS32|UC_MODE_LITTLE_ENDIAN);m.mem_map(0,0x200000)
                def put(a,data):m.mem_write(a&0x1FFFFFFF,bytes(data))
                def read(a,n):return bytes(m.mem_read(a&0x1FFFFFFF,n))
                put(base,body);put(start,before);put(result-0x20,result_initial)
                put(0x800A36A4,struct.pack('<I',callback if has_callback else 0))
                put(callback,struct.pack('<III',0,0x03E00008,0))
                calls=[];final_state=bytearray(expected);final_result=bytearray(expected_result)
                def hook(machine,pc,size,user):
                    if pc!=callback+4:return
                    assert read(start,len(expected))==expected,case
                    args=tuple(machine.reg_read(getattr(R,'UC_MIPS_REG_'+r)) for r in ('A0','A1'))
                    assert args==(5,result),(case,args)
                    calls.append(args)
                    setat(final_state,0x8009B574,0xDEADBEEF);setat(final_state,0x8009B554,0)
                    final_result[0x20]=0x99
                    put(start,final_state);put(result-0x20,final_result)
                    for r in ('V0','V1','A0','A1','A2','A3','T0','T1','T2','T3','T4','T5','T6','T7','T8','T9'):machine.reg_write(getattr(R,'UC_MIPS_REG_'+r),0xDEADCAFE)
                m.hook_add(UC_HOOK_CODE,hook)
                for n,v in (('A0',event),('A1',result),('SP',stack),('RA',stop)):m.reg_write(getattr(R,'UC_MIPS_REG_'+n),v)
                for i,r in enumerate(saved):m.reg_write(r,0xABCD0000+i)
                m.emu_start(base,stop,count=300)
                assert len(calls)==int(fire),(case,calls,fire)
                assert read(start,len(expected))==final_state,(case,event,pending,old,new)
                assert read(result-0x20,len(final_result))==final_result,case
                assert read(0x800A36A4,4)==struct.pack('<I',callback if has_callback else 0),case
                assert m.reg_read(R.UC_MIPS_REG_PC)==stop and m.reg_read(R.UC_MIPS_REG_SP)==stack,case
                for i,r in enumerate(saved):assert m.reg_read(r)==0xABCD0000+i,case


if __name__ == '__main__': unittest.main()
