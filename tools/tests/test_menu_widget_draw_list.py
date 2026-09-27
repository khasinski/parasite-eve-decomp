"""List rendering against a bounded independent callback/state model.

Requires a valid owner, finite child lists, an aligned arena with room and
nonoverflowing signed viewport arithmetic. Call boundaries for row rendering,
GPU packets, easing and highlights are modeled, not physical GPU behavior.
"""
from pathlib import Path
import itertools
import struct
import unittest

ROOT = Path(__file__).resolve().parents[2]


class MenuWidgetDrawListTests(unittest.TestCase):
    def test_plain_source(self):
        source = (ROOT/'src/main/menu/MenuWidget_DrawList.c').read_text()
        self.assertNotRegex(source, r'\b(?:asm|__asm__|INCLUDE_ASM|REGALLOC_BARRIER)\b')

    @unittest.skipUnless((ROOT/'assets/USA/main.exe').is_file() and
                         (ROOT/'build/USA/main.exe').is_file(), 'images unavailable')
    def test_list(self):
        try:
            from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN, UC_HOOK_CODE
            from unicorn import mips_const as R
        except ImportError:
            self.skipTest('unicorn unavailable')
        images = [(ROOT/p).read_bytes() for p in ('assets/USA/main.exe', 'build/USA/main.exe')]
        base = 0x800638D8
        offset = base - 0x8000F800
        self.assertEqual(images[0][offset:offset+0x458], images[1][offset:offset+0x458])
        syms = {
            'SetDrawArea': 0x80075B84, 'MenuWidget_DrawListRow': 0x800634D4,
            'Draw_AllocPrimWithMask': 0x8006374C, 'MenuWidget_EaseNodePosition': 0x80065260,
            'Draw_RemapStatusFlags': 0x8005E038, 'Draw_AllocColorTriGradient': 0x800622BC,
            'BoundsCheck_AssertStub': 0x800527C0,
        }
        NODE=0x80100000;PARENT=NODE+0x100;OTHER=NODE+0x200;POPUP=NODE+0x300
        X=0x8009D124;Y=X+4;STACK=X+8;LO=0x800A2270;HI=0x800A22B0
        CURSOR=0x8009D100;ARENA=0x80110000;OT=0x80120000;HEAD=0x8009D154;ACTIVE=0x8009D15C
        DRAW=0x801E0000
        hooks={syms[n]:(k,c) for n,k,c in (
            ('SetDrawArea','area',2),('MenuWidget_DrawListRow','row',4),
            ('Draw_AllocPrimWithMask','mask',1),('MenuWidget_EaseNodePosition','ease',1),
            ('Draw_RemapStatusFlags','remap',0),('Draw_AllocColorTriGradient','gradient',4),
            ('BoundsCheck_AssertStub','assert',1))}
        cases=[(*c,0,0) for c in itertools.product((0,2),(-3,0,3),(-1,0,1,2),(0,1),(0,128),(0,1),(0,32),(0,1),(0,3))]
        cases += [(2,adjust,1,1,0,1,32,1,2,depth,mutation)
                  for adjust,depth,mutation in itertools.product((-5,0,5),(0,7,8),(1,2,4,8,16,32))]
        regions=((NODE-8,0x410),(LO-8,0x60),(0x8009D0E0,0x90),(ARENA-8,0x30),(OT,8))
        saved=[getattr(R,f'UC_MIPS_REG_S{i}') for i in range(8)]+[R.UC_MIPS_REG_FP]
        def signed(v):return v if v<0x80000000 else v-0x100000000
        def half(v):
            v=signed(v)
            return v//2 if v>=0 else -((-v)//2)
        for candidate, image in enumerate(images):
            m=Uc(UC_ARCH_MIPS,UC_MODE_MIPS32|UC_MODE_LITTLE_ENDIAN);m.mem_map(0,0x200000)
            def put(a,v):m.mem_write(a&0x1FFFFFFF,bytes(v))
            def read(a,n):return bytes(m.mem_read(a&0x1FFFFFFF,n))
            def get(a):return struct.unpack('<I',read(a,4))[0]
            def word(a,v):put(a,struct.pack('<I',v&0xFFFFFFFF))
            put(0x8000F800,image)
            for a in hooks:put(a,struct.pack('<II',0x03E00008,0))
            def callback(kind,args,g,s,events):
                events.append((kind,args,g(X),g(Y),g(STACK)))
                if kind=='area':
                    s(ARENA,0x02ABCDEF);s(ARENA+4,0xE3000123);s(ARENA+8,0xE4000456)
                    if mutation&1:s(0x8009D11C,OT+4);s(PARENT+0x3C,99)
                elif kind=='row':
                    s(X,g(X)+7);s(Y,g(Y)+g(NODE+0x40))
                    if mutation&2:
                        s(NODE+0x38,1);s(NODE+0x60,-2);s(PARENT+0x3C,77)
                elif kind=='mask':
                    if mutation&4:s(NODE+0x80,OTHER);s(X,g(X)+10)
                elif kind=='ease':
                    if mutation&8:
                        s(NODE+0x60,0);s(NODE+0x40,9);s(NODE+0x44,0);s(NODE+0x48,1)
                elif kind=='remap':
                    if mutation&16:s(ACTIVE,OTHER);s(NODE+0x3C,23)
                    return status
                elif kind=='gradient':
                    if mutation&32:s(X,500);s(Y,700);s(STACK,LO+16)
                elif kind=='assert':
                    s(STACK,LO)
                return 0
            actual=[]
            def hook(uc,address,size,data):
                if address not in hooks:return
                kind,n=hooks[address]
                args=tuple(uc.reg_read(getattr(R,'UC_MIPS_REG_A'+str(i))) for i in range(n))
                if kind=='area':args=(args[0],struct.unpack('<4H',read(args[1],8)))
                result=callback(kind,args,get,word,actual)
                for name in ('V1','A0','A1','A2','A3','T0','T1','T2','T3','T4','T5','T6','T7','T8','T9'):
                    uc.reg_write(getattr(R,'UC_MIPS_REG_'+name),0xCCCCCCCC)
                uc.reg_write(R.UC_MIPS_REG_V0,result)
            m.hook_add(UC_HOOK_CODE,hook)
            for rows,adjust,selection,focused,flags,enabled,status,page,child,depth,mutation in cases:
                case=(candidate,rows,adjust,selection,focused,flags,enabled,status,page,child,depth,mutation)
                initial={a:0xA5A5A5A5 for start,size in regions for a in range(start,start+size,4)}
                initial.update({HEAD:OTHER,OTHER:PARENT,PARENT:0,PARENT+0x3C:7,
                    NODE+0x34:2,NODE+0x38:rows,NODE+0x3C:12,NODE+0x40:8,
                    NODE+0x44:selection,NODE+0x48:selection+1,NODE+0x5C:1,
                    NODE+0x60:adjust,NODE+0x64:flags,NODE+0x80:POPUP,
                    X:0xFFFFFFFD,Y:101,STACK:LO+depth*8,CURSOR:ARENA,
                    CURSOR+4:ARENA,CURSOR+8:page,0x8009D11C:OT,0x8009D0E8:enabled,
                    ACTIVE:NODE if focused else OTHER,OT:0xAA654321,OT+4:0xBB123456})
                for n in (PARENT,OTHER):
                    for c in range(4):initial[n+8+c*4]=0
                initial[PARENT+8+child*4]=NODE
                for a,v in initial.items():word(a,v)
                expected={a:v&0xFFFFFFFF for a,v in initial.items()};events=[]
                def g(a):return expected[a]
                def s(a,v):expected[a]=v&0xFFFFFFFF
                def call(kind,*args):return callback(kind,args,g,s,events)
                def push():
                    p=g(STACK)
                    if p<HI:s(STACK,p+8);s(p,g(X));s(p+4,g(Y))
                    else:call('assert',2)
                def pop():
                    p=g(STACK)
                    if p>LO:
                        x,y=g(p-8),g(p-4);s(STACK,p-8);s(X,x);s(Y,y)
                    else:call('assert',3)
                draw_cursor=g(PARENT+0x3C)
                s(0x8009D164,g(NODE+0x3C));s(0x8009D168,g(NODE+0x40));s(CURSOR,ARENA+12)
                call('area',ARENA,(0,224 if page else 0,320,224))
                ot=g(0x8009D11C);s(ARENA,(g(ARENA)&0xFF000000)|(g(ot)&0xFFFFFF));s(ot,(g(ot)&0xFF000000)|(ARENA&0xFFFFFF))
                push();s(Y,g(Y)+g(NODE+0x60))
                if signed(g(NODE+0x60))>0:
                    s(Y,g(Y)-g(NODE+0x40));call('row',NODE,DRAW,0xFFFFFFFF,draw_cursor)
                row=0
                while row<signed(g(NODE+0x38)):
                    call('row',NODE,DRAW,row,draw_cursor);row+=1
                    assert row<8,case
                if signed(g(NODE+0x60))<0:call('row',NODE,DRAW,row,draw_cursor)
                pop();call('mask',NODE);call('ease',g(NODE+0x80))
                offset=signed(g(NODE+0x60))
                if offset>0:s(NODE+0x60,max(0,offset-half(g(NODE+0x40))))
                elif offset<0:s(NODE+0x60,min(0,offset+half(g(NODE+0x40))))
                if g(NODE+0x60)==0 and signed(g(NODE+0x44))>=0 and signed(g(NODE+0x48))>=signed(g(NODE+0x5C)) and signed(g(NODE+0x48))<signed(g(NODE+0x5C))+signed(g(NODE+0x38)):
                    push();s(X,g(X)+g(NODE+0x3C)*g(NODE+0x44));s(Y,g(Y)+g(NODE+0x40)*(g(NODE+0x48)-g(NODE+0x5C)))
                    highlight=0
                    if not(g(NODE+0x64)&128) and g(ACTIVE)==NODE and g(0x8009D0E8):
                        highlight=int(bool(call('remap')&32))
                    call('gradient',g(NODE+0x3C),g(NODE+0x40),highlight,int(g(ACTIVE)==NODE))
                    pop()
                actual.clear()
                for name,value in (('A0',NODE),('A1',DRAW),('SP',0x801F0000),('GP',0x8009CD70),('RA',0x80010000)):
                    m.reg_write(getattr(R,'UC_MIPS_REG_'+name),value)
                for i,r in enumerate(saved):m.reg_write(r,0xABCD0000+i)
                m.emu_start(base,0x80010000,count=20000)
                assert actual==events,(case,actual,events)
                for start,size in regions:
                    assert read(start,size)==b''.join(struct.pack('<I',expected[a]) for a in range(start,start+size,4)),(case,hex(start))
                assert all(m.reg_read(r)==0xABCD0000+i for i,r in enumerate(saved)),case
                assert (m.reg_read(R.UC_MIPS_REG_SP),m.reg_read(R.UC_MIPS_REG_GP),m.reg_read(R.UC_MIPS_REG_PC))==(0x801F0000,0x8009CD70,0x80010000),case
            print('candidate' if candidate else 'retail','verified',len(cases))


if __name__ == '__main__':
    unittest.main()
