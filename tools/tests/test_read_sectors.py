"""CD/SPU transfer states against a bounded callback model and retail bytes.

The 144 explicitly counted exclusions are inconsistent blocking resumes that
wrap the remaining count and exceed the model's 100-transition bound.
"""
from pathlib import Path
from importlib.util import find_spec
import itertools
import struct
import unittest

ROOT=Path(__file__).resolve().parents[2]
base=0x8006CDA4
syms={
    'D_800B0CD8':0x800B0CD8, 'D_8009317C':0x8009317C, 'D_8009D170':0x8009D170,
    'Spu_SetStreamModeA':0x80087198, 'Spu_SetStreamModeB':0x80087414,
    'CdRom_ReadSectors':0x8006E6D4, 'CdRom_PollReady':0x8006E7E8,
    'Akao_StepNoteSequencer':0x800871AC, 'Spu_UploadSampleBlockBlocking':0x80087090,
    'Spu_UploadStreamBlockB':0x800875FC, 'Spu_UploadStreamBlockA':0x80087428,
    'Spu_GetTransferStatus':0x800870E0,
}


class ReadSectorsTests(unittest.TestCase):
    @unittest.skipUnless((ROOT/'assets/USA/main.exe').is_file() and
                         (ROOT/'build/USA/main.exe').is_file() and find_spec('unicorn'),
                         'images or unicorn unavailable')
    def test_model_and_exact_bytes(self):
        from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN, UC_HOOK_CODE
        from unicorn import mips_const as R
        images = [(ROOT / p).read_bytes() for p in ('assets/USA/main.exe', 'build/USA/main.exe')]
        def section(image, address, size):
            offset = address - 0x8000F800
            return image[offset:offset + size]
        self.assertEqual(section(images[0], base, 724), section(images[1], base, 724))
        self.assertEqual(section(images[0], 0x80011428, 44), section(images[1], 0x80011428, 44))
        callbacks={'Spu_SetStreamModeA':0,'Spu_SetStreamModeB':0,'CdRom_ReadSectors':4,'CdRom_PollReady':0,'Akao_StepNoteSequencer':2,'Spu_UploadSampleBlockBlocking':2,'Spu_UploadStreamBlockB':2,'Spu_UploadStreamBlockA':3,'Spu_GetTransferStatus':0}
        cases=list(itertools.product((0,7,8,9,10),range(4),(-1,0,1),(0,1,2,3,0xFFFFFFFF),(0,1,3),(1,2)))
        cases += [(7,kind,result,0,remaining,maximum)
                  for kind,result,remaining,maximum in itertools.product(range(4),(-1,0,1),(65537,0xFFFFFFFF),(1,2))]
        cases=[case+(mutate,0,17,10,13,100,1000) for case in cases for mutate in (False,True)]
        cases += [(phase,kind,result,0,3,2,mutate,index,channel,lo,hi,archive,image_base)
                  for phase,kind,result,index,channel,(lo,hi),(archive,image_base),mutate
                  in itertools.product((0,9),range(4),(-1,0,1),(1,3,9),
                                       (0,255,256,0xFFFFFFFF),
                                       ((0,1),(0x8000,0x8003),(0xFFFE,0xFFFF)),
                                       ((100,1000),(0x100,0xFFFFFFF0)),(False,True))]
        saved=[getattr(R,f'UC_MIPS_REG_S{i}') for i in range(8)]+[R.UC_MIPS_REG_FP]
        for candidate, image in enumerate(images):
            verified=0
            skipped=0
            m=Uc(UC_ARCH_MIPS,UC_MODE_MIPS32|UC_MODE_LITTLE_ENDIAN);m.mem_map(0,0x200000)
            def put(a,v):m.mem_write(a&0x1FFFFFFF,bytes(v))
            def read(a,n):return bytes(m.mem_read(a&0x1FFFFFFF,n))
            def word(a,v):put(a,struct.pack('<I',v&0xFFFFFFFF))
            put(base,section(image,base,724))
            put(0x80011428,section(image,0x80011428,44))
            callback_addrs={syms[n]:n for n in callbacks}
            for a in callback_addrs:put(a,struct.pack('<II',0x03E00008,0))
            actual=[]
            def hook(uc,address,size,data):
                if address not in callback_addrs:return
                name=callback_addrs[address]
                args=tuple(uc.reg_read(getattr(R,f'UC_MIPS_REG_A{i}')) for i in range(callbacks[name]))
                value=first_result if not actual else 0
                actual.append((name,args))
                if mutate and len(actual)==1:
                    put(globals_,struct.pack('<4I',2220,7,4,2))
                    put(state+0xF0,b'\x07');put(state,b'\xA6')
                    put(0x80140000,b'CALL')
                for reg in ('V1','A0','A1','A2','A3','T0','T1','T2','T3','T4','T5','T6','T7','T8','T9'):
                    uc.reg_write(getattr(R,'UC_MIPS_REG_'+reg),0xCC000000)
                uc.reg_write(R.UC_MIPS_REG_V0,value&0xFFFFFFFF)
            m.hook_add(UC_HOOK_CODE,hook)
            for phase,kind,first_result,blocking,remaining,maximum,mutate,index,channel,lo,hi,archive,image_base in cases:
                case=(candidate,phase,kind,first_result,blocking,remaining,maximum,mutate,index,channel,lo,hi,archive,image_base)
                state=syms['D_800B0CD8'];table=syms['D_8009317C'];globals_=syms['D_8009D170']
                put(state,b'\xA5'*0x104);put(state+0xF0,bytes([phase]));word(state+0x100,image_base)
                put(table,b'\x5A'*32)
                word(table,archive)
                put(table+4+2*index,struct.pack('<HH',lo,hi))
                put(globals_,struct.pack('<4I',1110,3,remaining,1))
                put(0x80140000,b'\x5A'*0x100)
                expect_state=bytearray(read(state,0x104));before_table=read(table,32)
                p=phase;start=1110;total=3;left=remaining;chunk=1;result=-1;busy=0;done=False;expected=[]
                def call(name,*args):
                    nonlocal p,start,total,left,chunk
                    value=first_result if not expected else 0
                    expected.append((name,args))
                    if mutate and len(expected)==1:
                        p=7;start=2220;total=7;left=4;chunk=2
                        expect_state[0]=0xA6
                    return value
                for step in range(100):
                    if p==0:
                        start=(image_base+archive+lo)&0xFFFFFFFF;total=hi-lo;left=total
                        if kind in (0,3):
                            result=call('Spu_SetStreamModeA' if kind==0 else 'Spu_SetStreamModeB')
                            if result==-1:busy=1;done=(blocking&1)==0
                        p=7
                    elif p==7:
                        if left:
                            chunk=min(left,maximum)
                            result=call('CdRom_ReadSectors',start,(total-left)&0xFFFFFFFF,0x80140000,chunk)
                            if result!=-1:p=8
                            busy=1;done=(blocking&1)==0
                        else:p=0;busy=0;done=True
                    elif p==8:
                        result=call('CdRom_PollReady')
                        if result==0:p=9
                        else:
                            if result==-1:p=7
                            busy=1;done=(blocking&1)==0
                    elif p==9:
                        names=('Akao_StepNoteSequencer','Spu_UploadSampleBlockBlocking','Spu_UploadStreamBlockB','Spu_UploadStreamBlockA')
                        args=((0x80140000,chunk<<11),(0x80140000,0),(channel,0x80140000),(channel,0x80140000,chunk<<11))
                        result=call(names[kind],*args[kind])
                        if result==-1:p=0;busy=1;done=(blocking&1)==0
                        else:p=10
                    elif p==10:
                        result=call('Spu_GetTransferStatus')
                        if result==-1:p=0
                        elif result==0:p=7;left=(left-chunk)&0xFFFFFFFF
                        busy=1;done=(blocking&1)==0
                    if done:break
                else:
                    # Inconsistent resumed state (zero remaining but pending chunk)
                    # wraps and needs millions of iterations, outside bounded model.
                    skipped+=1
                    continue
                expect_state[0xF0]=p
                actual.clear()
                for name,value in (('A0',kind),('A1',index),('A2',channel),('A3',0x80140000),('SP',0x801F0000),('RA',0x80010000),('GP',0x8009CD70)):
                    m.reg_write(getattr(R,'UC_MIPS_REG_'+name),value)
                word(0x801F0010,maximum);word(0x801F0014,blocking)
                for i,r in enumerate(saved):m.reg_write(r,0xABCD0000+i)
                m.emu_start(base,0x80010000,count=50000)
                assert actual==expected,(case,actual,expected)
                assert read(state,0x104)==expect_state,case
                assert read(globals_,16)==struct.pack('<4I',start,total,left,chunk),case
                expected_buffer=(b'CALL'+b'\x5A'*0xFC) if mutate and expected else b'\x5A'*0x100
                assert read(table,32)==before_table and read(0x80140000,0x100)==expected_buffer,case
                assert m.reg_read(R.UC_MIPS_REG_V0)==busy,case
                assert m.reg_read(R.UC_MIPS_REG_PC)==0x80010000 and m.reg_read(R.UC_MIPS_REG_SP)==0x801F0000,case
                assert m.reg_read(R.UC_MIPS_REG_GP)==0x8009CD70,case
                for i,r in enumerate(saved):assert m.reg_read(r)==0xABCD0000+i,case
                verified+=1
            self.assertEqual(verified,7008)
            self.assertEqual(skipped,144)
