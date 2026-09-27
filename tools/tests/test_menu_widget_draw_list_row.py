"""Independent menu row callback, cursor, and shift behavior model."""
from importlib.util import find_spec
from pathlib import Path
import itertools
import struct
import subprocess
import unittest


class MenuWidgetDrawListRowTests(unittest.TestCase):
    @unittest.skipUnless((Path(__file__).resolve().parents[2] / "assets/USA/main.exe").is_file() and
                         (Path(__file__).resolve().parents[2] / "build/USA/main.exe").is_file() and
                         find_spec("unicorn"), "images or unicorn unavailable")
    def test_exact_bytes_and_state_model(self):
        from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN, UC_HOOK_CODE
        from unicorn import mips_const as R
        ROOT=Path(__file__).resolve().parents[2];base=0x800634D4
        syms={}
        for line in subprocess.check_output(['mipsel-none-elf-nm',str(ROOT/'build/USA/main.elf')],text=True).splitlines():
            fields=line.split()
            if len(fields)==3:
                try:syms[fields[2]]=int(fields[0],16)
                except ValueError:pass
        code=(ROOT/'build/USA/main.exe').read_bytes()[base-0x8000F800:base-0x8000F800+0x278]
        retail=(ROOT/'assets/USA/main.exe').read_bytes()[0x800:]
        assert code == retail[base-0x80010000:base-0x80010000+0x278]
        NODE=0x80100000;X=0x8009D124;Y=X+4;STACK=X+8;DIM=0x8009D10C;LO=0x800A2270;HI=0x800A22B0
        SELECT=0x801E0000;DRAW=0x801E0020
        hooks={SELECT:('select',1),DRAW:('draw',1),syms['VSync']:('vsync',1),syms['Draw_AllocColorTri']:('tri',3),syms['BoundsCheck_AssertStub']:('assert',1)}
        cases=[(*c,3) for c in itertools.product((-1,0,1,3),range(4),(False,True),(-1,0,2),(0,1),(0,8),(0,7,8),(0,1,2,4,8,16))]
        cases += [(3,3,True,r,1,8,0,0,stride) for r,stride in itertools.product((-2,-1,0,31,32),(-1,1,32))]
        def signed(v):return v if v<0x80000000 else v-0x100000000
        regions=((NODE-8,0xA0),(LO-8,0x60),(0x8009D100,0x40))
        saved=[getattr(R,f'UC_MIPS_REG_S{i}') for i in range(8)]+[R.UC_MIPS_REG_FP]
        for candidate in (False,True):
            m=Uc(UC_ARCH_MIPS,UC_MODE_MIPS32|UC_MODE_LITTLE_ENDIAN);m.mem_map(0,0x200000)
            def put(a,v):m.mem_write(a&0x1FFFFFFF,bytes(v))
            def read(a,n):return bytes(m.mem_read(a&0x1FFFFFFF,n))
            def get(a):return struct.unpack('<I',read(a,4))[0]
            def set_(a,v):put(a,struct.pack('<I',v&0xFFFFFFFF))
            put(0x80010000,retail)
            if candidate:put(base,code)
            for a in hooks:put(a,struct.pack('<II',0x03E00008,0))
            def callback(kind,args,g,s,events):
                events.append((kind,args,g(X),g(Y),g(DIM),g(NODE+0x74)))
                if kind=='select':
                    if mutation&1:
                        s(NODE+0x5C,2);s(NODE+0x74,0x5A5A5A5A);s(NODE+0x8C,0);s(X,g(X)+7)
                    return 0 if selector==1 else (1 if selector==2 else args[0]&1)
                if kind=='draw' and mutation&2:
                    s(NODE+0x34,2);s(NODE+0x48,row+g(NODE+0x5C));s(NODE+0x44,1);s(Y,g(Y)+11)
                if kind=='vsync':
                    if mutation&4:s(NODE+0x3C,17);s(NODE+0x40,9)
                    return tick
                if kind=='tri' and mutation&8:
                    s(X,g(X)+100);s(Y,g(Y)+200);s(STACK,LO+16)
                if kind=='assert' and mutation&16:s(STACK,LO)
                return 0
            actual=[]
            def hook(uc,address,size,data):
                if address not in hooks:return
                kind,n=hooks[address]
                args=tuple(uc.reg_read(getattr(R,'UC_MIPS_REG_A'+str(i))) for i in range(n))
                result=callback(kind,args,get,set_,actual)
                for name in ('V1','A0','A1','A2','A3','T0','T1','T2','T3','T4','T5','T6','T7','T8','T9'):
                    uc.reg_write(getattr(R,'UC_MIPS_REG_'+name),0xCCCCCCCC)
                uc.reg_write(R.UC_MIPS_REG_V0,result)
            m.hook_add(UC_HOOK_CODE,hook)
            for columns,selector,draw,row,focus,tick,depth,mutation,stride in cases:
                case=(candidate,columns,selector,draw,row,focus,tick,depth,mutation,stride)
                initial={a:0xA5A5A5A5 for start,size in regions for a in range(start,start+size,4)}
                initial.update({NODE+0x34:columns&0xFFFFFFFF,NODE+0x3C:12,NODE+0x40:8,NODE+0x44:0,NODE+0x48:(row+1)&0xFFFFFFFF,NODE+0x4C:0,NODE+0x50:(row+1)&0xFFFFFFFF,NODE+0x54:stride&0xFFFFFFFF,NODE+0x5C:1,NODE+0x74:0xA55A5AA5,NODE+0x8C:SELECT if selector else 0,X:0xFFFFFFFD,Y:101,STACK:LO+depth*8,DIM:0x55})
                for a,v in initial.items():set_(a,v)
                expected=dict(initial);events=[]
                def g(a):return expected[a]
                def s(a,v):expected[a]=v&0xFFFFFFFF
                def call(kind,*args):return callback(kind,tuple(v&0xFFFFFFFF for v in args),g,s,events)
                index=(g(NODE+0x54)*(g(NODE+0x5C)+row))&0xFFFFFFFF;bit=1<<(index&31)
                p=g(STACK)
                if p<HI:s(STACK,p+8);s(p,g(X));s(p+4,g(Y))
                else:call('assert',2)
                s(X,g(X)+2);s(Y,g(Y)+2);x=0
                while x<signed(g(NODE+0x34)):
                    enabled=1
                    if g(NODE+0x8C):
                        enabled=call('select',index);index=(index+1)&0xFFFFFFFF
                        s(NODE+0x74,(g(NODE+0x74)&~bit)|(bit if enabled else 0));bit=(bit<<1)&0xFFFFFFFF
                    dim=int(not enabled or (focus and (x!=g(NODE+0x44) or ((row+g(NODE+0x5C))&0xFFFFFFFF)!=g(NODE+0x48))))
                    s(DIM,dim)
                    if draw:call('draw',g(NODE+0x34)*(g(NODE+0x5C)+row)+x)
                    ticks=call('vsync',-1)
                    if ticks&8 and x==g(NODE+0x4C) and ((row+g(NODE+0x5C))&0xFFFFFFFF)==g(NODE+0x50):
                        s(X,g(X)-2);s(Y,g(Y)-2);call('tri',g(NODE+0x3C),g(NODE+0x40),0);s(X,g(X)+2);s(Y,g(Y)+2)
                    s(X,g(X)+g(NODE+0x3C));x+=1
                    assert x<8,case
                p=g(STACK)
                if p>LO:
                    oldx,oldy=g(p-8),g(p-4);s(STACK,p-8);s(X,oldx);s(Y,oldy)
                else:call('assert',3)
                s(Y,g(Y)+g(NODE+0x40))
                actual.clear()
                for name,value in (('A0',NODE),('A1',DRAW if draw else 0),('A2',row&0xFFFFFFFF),('A3',focus),('SP',0x801F0000),('GP',0x8009CD70),('RA',0x80010000)):
                    m.reg_write(getattr(R,'UC_MIPS_REG_'+name),value)
                for i,r in enumerate(saved):m.reg_write(r,0xABCD0000+i)
                m.emu_start(base,0x80010000,count=20000)
                assert actual==events,(case,actual,events)
                for a,v in expected.items():assert get(a)==v,(case,hex(a),hex(get(a)),hex(v))
                assert m.reg_read(R.UC_MIPS_REG_PC)==0x80010000 and m.reg_read(R.UC_MIPS_REG_SP)==0x801F0000 and m.reg_read(R.UC_MIPS_REG_GP)==0x8009CD70,case
                for i,r in enumerate(saved):assert m.reg_read(r)==0xABCD0000+i,case
            assert len(cases) == 6927
