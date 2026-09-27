"""Retail/C particle behavior with deterministic, state-mutating call stubs.

Valid disjoint state and palette storage. Checks defined vector components,
not uninitialized vector padding or physical GPU behavior.
"""
from pathlib import Path
import itertools
import struct
import unittest

ROOT = Path(__file__).resolve().parents[2]


class RisingEffectTests(unittest.TestCase):
    def test_plain_source(self):
        source = (ROOT/'src/main/engine/FieldEng_RisingEffect.c').read_text()
        self.assertNotRegex(source, r'\b(?:asm|__asm__|INCLUDE_ASM|REGALLOC_BARRIER)\b')

    @unittest.skipUnless((ROOT/'assets/USA/main.exe').is_file() and
                         (ROOT/'build/USA/main.exe').is_file(), 'images unavailable')
    def test_behavior(self):
        try:
            from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN, UC_HOOK_CODE
            from unicorn import mips_const as R
        except ImportError:
            self.skipTest('unicorn unavailable')
        base, length = 0x800D5CE4, 464
        images = [(ROOT/p).read_bytes()[base-0x8000F800:base-0x8000F800+length]
                  for p in ('assets/USA/main.exe', 'build/USA/main.exe')]
        self.assertEqual(len(images[1]), length)
        self.assertEqual(*images)
        state, sp, stop, gp = 0x80100020, 0x801F0000, 0x80010000, 0x8009CD70
        time, selector, factor, flag, palette = 0x800E27EC, 0x800F336C, 0x800F336A, 0x800F3428, 0x800E1204
        regions = ((state-8,32),(time-8,20),(factor-8,24),(flag-8,20),(palette-8,32))
        hooks = {0x80071A54:'rand',0x800CF3AC:'color',0x80077CF4:'sin',0x80077AA4:'clut',0x800CEE20:'draw'}
        saved = [getattr(R,f'UC_MIPS_REG_S{i}') for i in range(8)]+[R.UC_MIPS_REG_FP]
        def signed(v):
            v &= 0xFFFFFFFF
            return v-0x100000000 if v & 0x80000000 else v
        def div(v,d): return (-1 if v < 0 else 1)*(abs(v)//d)
        cases = list(itertools.product((-1,0,1,2,3),
            (-2147483648,-25,-1,0,23,24,25,2097152,2147483647),range(4),(0,1,7)))
        for body in images:
            m = Uc(UC_ARCH_MIPS,UC_MODE_MIPS32|UC_MODE_LITTLE_ENDIAN)
            m.mem_map(0,0x200000)
            def put(a,b):m.mem_write(a&0x1FFFFFFF,bytes(b))
            def read(a,n):return bytes(m.mem_read(a&0x1FFFFFFF,n))
            def get(a,n=4):return int.from_bytes(read(a,n),'little')
            def set_(a,v,n=4):put(a,(v&((1<<(8*n))-1)).to_bytes(n,'little'))
            put(base,body)
            for address in hooks:put(address,struct.pack('<II',0x03E00008,0))
            actual=[]
            def callback(kind,args,g,s,events):
                events.append((kind,args))
                if kind=='rand':
                    if mutation&1:s(state+8,65535,2);s(time,24)
                    return (0,1,0x7FFFFFFF,0x80000000)[variant]
                if kind=='color':
                    if mutation&1:s(state,32768,2);s(time,25)
                    return 0
                if kind=='sin':
                    if mutation&2:s(selector,4,2);s(flag,1)
                    return (args[0]&8191)-4096
                if kind=='clut':
                    if mutation&4:s(time,-2147483648);s(factor,32768,2)
                    return 0x1234ABCD
                return 0
            def hook(uc,address,size,data):
                if address not in hooks:return
                kind=hooks[address]
                args=[uc.reg_read(getattr(R,f'UC_MIPS_REG_A{i}')) for i in range(4)]
                if kind=='color':
                    put(args[1],bytes((17,63,201,0)))
                    values=(args[0],args[2])
                elif kind=='draw':
                    stack=uc.reg_read(R.UC_MIPS_REG_SP)
                    values=(tuple(get(args[0]+i,2) for i in (0,2,4)),
                            tuple(get(args[1]+i,2) for i in (0,2,4)),args[2],args[3],
                            *(get(stack+i) for i in (16,20,24,28)),read(get(stack+32),4))
                else:values=tuple(args[:{'rand':0,'sin':1,'clut':2}[kind]])
                result=callback(kind,values,get,set_,actual)
                for name in ('V1','A0','A1','A2','A3','T0','T1','T2','T3','T4','T5','T6','T7','T8','T9'):
                    uc.reg_write(getattr(R,'UC_MIPS_REG_'+name),0xCCCCCCCC)
                uc.reg_write(R.UC_MIPS_REG_V0,result&0xFFFFFFFF)
            m.hook_add(UC_HOOK_CODE,hook)
            for mode,t,variant,mutation in cases:
                case=(mode,t,variant,mutation)
                expected={a:0xA5 for start,size in regions for a in range(start,start+size)}
                def g(a,n=4):return int.from_bytes(bytes(expected[a+i] for i in range(n)),'little')
                def s(a,v,n=4):
                    for i,b in enumerate((v&((1<<(8*n))-1)).to_bytes(n,'little')):expected[a+i]=b
                for i,v in enumerate(((0,1,65535,1,2,3,4095),(32767,32768,65534,65535,32768,1,65535),
                                      (65535,0,32768,32768,65535,65535,0),(1,2,3,4,5,6,7))[variant]):s(state+2*i,v,2)
                s(time,t);s(selector,(0,4,4,7)[variant],2);s(flag,variant&1);s(factor,(0,1,65535,32768)[variant],2)
                for i in range(8):s(palette+2*i,(i*10000+32760)&65535,2)
                for start,size in regions:put(start,bytes(expected[a] for a in range(start,start+size)))
                events=[]
                def call(kind,*args):return callback(kind,tuple(args),g,s,events)
                result=0
                if mode==1:
                    for i in (0,2,4):s(state+i,g(state+i,2)+g(state+i+6,2),2)
                    random=call('rand');s(state+8,g(state+8,2)+2+(random&1),2)
                    result=int(signed(g(time))>=24)
                elif mode==2:
                    call('color',0x800E1694,g(time))
                    position=tuple(g(state+i,2) for i in (0,2,4))
                    rotation=(0,0,(g(state+12,2)+(g(time)<<5))&65535)
                    scale=call('sin',div(signed(g(time)<<10),24)&0xFFFFFFFF)+4096
                    sel=g(selector,2);y=g(palette+2*sel,2)+(4 if sel==4 and g(flag) else 0)
                    clut=call('clut',32,y)&65535
                    f=g(factor,2);f=f-65536 if f&32768 else f
                    texture=(div(signed(g(time)),3)*f+128)&0xFFFFFFFF
                    call('draw',position,rotation,scale,scale,texture,clut,1,128,bytes((17,63,201,0)))
                actual.clear()
                for name,v in (('A0',mode),('A1',state),('SP',sp),('GP',gp),('RA',stop)):
                    m.reg_write(getattr(R,'UC_MIPS_REG_'+name),v&0xFFFFFFFF)
                for i,r in enumerate(saved):m.reg_write(r,0xABCD0000+i)
                m.emu_start(base,stop,count=2000)
                self.assertEqual(actual,events,case)
                for start,size in regions:self.assertEqual(read(start,size),bytes(expected[a] for a in range(start,start+size)),case)
                self.assertEqual(m.reg_read(R.UC_MIPS_REG_V0),result,case)
                for name,v in (('PC',stop),('SP',sp),('GP',gp)):self.assertEqual(m.reg_read(getattr(R,'UC_MIPS_REG_'+name)),v,case)
                for i,r in enumerate(saved):self.assertEqual(m.reg_read(r),0xABCD0000+i,case)


if __name__ == '__main__':
    unittest.main()
