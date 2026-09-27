"""Task queue decoder against an independent bounded model and both full images.

Covers valid modes 0..4, countdown/gates, handler continuation and replacement
of current actor/node/cursor. Reserved modes and an initially empty queue are
not treated as valid inputs. Unicorn is not a hardware load-delay oracle.
"""
from pathlib import Path
import itertools
import struct
import unittest

ROOT = Path(__file__).resolve().parents[2]


class TaskRunQueueTests(unittest.TestCase):
    def test_plain_source(self):
        source = (ROOT/'src/main/task/Task_RunQueue.c').read_text()
        self.assertNotRegex(source, r'\b(?:asm|__asm__|INCLUDE_ASM|REGALLOC_BARRIER)\b')

    @unittest.skipUnless((ROOT/'assets/USA/main.exe').is_file() and
                         (ROOT/'build/USA/main.exe').is_file(), 'images unavailable')
    def test_queue(self):
        try:
            from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN, UC_HOOK_CODE
            from unicorn import mips_const as R
        except ImportError:
            self.skipTest('unicorn unavailable')
        images = [(ROOT/p).read_bytes() for p in ('assets/USA/main.exe', 'build/USA/main.exe')]
        base = 0x80017018
        offset = base - 0x8000F800
        self.assertEqual(images[0][offset:offset+0x27C], images[1][offset:offset+0x27C])
        self.assertEqual(images[0][0xE90:0xEA4], images[1][0xE90:0xEA4])
        syms = {name: int(name[2:], 16) for name in (
            'D_8009D300', 'D_8009CE00', 'D_8009D2F0', 'D_8009D254',
            'D_8009D1A0', 'D_800910A0', 'D_800A77F0', 'D_8009DF70', 'D_800B6A80')}
        nodes=0x80100000;actors=0x80101000;programs=(0x80110000,0x80111000,0x80112000)
        behaviors=(0,1,3,4,9,15)
        cases=[(f,e,g,a,t,6,5,b) for f,e,g,a,t,b in itertools.product((0,0x10,0x40,0x80),(0,0x1000),(0,0x100),(False,True),(0,1,2,0xFFFFFFFF),behaviors)]
        cases += [(0,0,0,True,1,n,mode,b) for n,mode,b in itertools.product((0,1,5,6,15),range(6),behaviors)]
        saved=[getattr(R,f'UC_MIPS_REG_S{i}') for i in range(8)]+[R.UC_MIPS_REG_FP]
        for candidate, image in enumerate(images):
            m=Uc(UC_ARCH_MIPS,UC_MODE_MIPS32|UC_MODE_LITTLE_ENDIAN);m.mem_map(0,0x200000)
            def put(a,v):m.mem_write(a&0x1FFFFFFF,bytes(v))
            def read(a,n):return bytes(m.mem_read(a&0x1FFFFFFF,n))
            def word(a,v):put(a,struct.pack('<I',v&0xFFFFFFFF))
            def getword(a):return struct.unpack('<I',read(a,4))[0]
            put(0x8000F800,image)
            hooks={0x801E0000+op*16:op for op in (3,4,5)}
            for address,op in hooks.items():
                put(address,struct.pack('<II',0x03E00008,0));word(syms['D_800910A0']+op*4,address)
            actual=[]
            def hook(uc,address,size,data):
                if address not in hooks:return
                pointers=tuple(getword(uc.reg_read(R.UC_MIPS_REG_A0)+i*4) for i in range(argc))
                actual.append((hooks[address],pointers))
                first=len(actual)==1
                if first:
                    if behavior&2:word(syms['D_8009CE00'],programs[2])
                    if behavior&4:word(syms['D_8009D300'],nodes+0x80)
                    if behavior&8:word(syms['D_8009D2F0'],actors+0x300)
                for reg in ('V1','A0','A1','A2','A3','T0','T1','T2','T3','T4','T5','T6','T7','T8','T9'):
                    uc.reg_write(getattr(R,'UC_MIPS_REG_'+reg),0xCC000000)
                uc.reg_write(R.UC_MIPS_REG_V0,int(first and bool(behavior&1)))
            m.hook_add(UC_HOOK_CODE,hook)
            for flags,entity_flags,global_flags,is_active,ticks,argc,pattern,behavior in cases:
                case=(candidate,flags,entity_flags,global_flags,is_active,ticks,argc,pattern,behavior)
                put(nodes,b'\xA5'*0xC0);put(actors,b'\x5A'*0x600)
                word(actors+0x98,entity_flags);word(actors+0x398,0)
                for i in range(3):
                    word(nodes+i*0x40,programs[min(i,2)]);word(nodes+i*0x40+8,flags if i==0 else 0x80)
                    word(nodes+i*0x40+0x10,ticks if i==0 else (1 if i==1 else 13))
                    word(nodes+i*0x40+0x24,nodes+0x40 if i==0 else 0)
                word(syms['D_8009D300'],nodes);word(syms['D_8009CE00'],0x8011F000)
                word(syms['D_8009D2F0'],actors);word(syms['D_8009D254'],actors if is_active else actors+0x300)
                word(syms['D_8009D1A0'],global_flags)
                records={};modes=[pattern if pattern<5 else i%5 for i in range(argc)]
                for program,op in zip(programs,(3,4,5)):
                    put(program,b'\xA5'*0x200)
                    for sequence in range(2):
                        address=program+sequence*(8+argc*4)
                        values=list(range(1,argc+1))
                        header=op|(argc<<13)|sum(mode<<(17+3*i) for i,mode in enumerate(modes[:5]))
                        tail=sum(mode<<(3*i) for i,mode in enumerate(modes[5:]))
                        put(address,struct.pack('<'+'I'*(2+argc),header,tail,*values))
                        records[address]=(op,modes,values,address+8+argc*4)
                expected_nodes=bytearray(read(nodes,0xC0));actor_guard=read(actors,0x600)
                program_guards={p:read(p,0x200) for p in programs}
                current=nodes;entity=actors;cursor=0x8011F000;expected=[]
                while current:
                    offset=current-nodes
                    f=struct.unpack_from('<I',expected_nodes,offset+8)[0]
                    ef=entity_flags if entity==actors else 0
                    allowed=not(f&0x50) and (not(ef&0x1000) or bool(f&0x80)) and (not(global_flags&0x100) or entity==(actors if is_active else actors+0x300) or bool(f&0x80))
                    t=struct.unpack_from('<I',expected_nodes,offset+0x10)[0]
                    if allowed and t:
                        t=(t-1)&0xFFFFFFFF;struct.pack_into('<I',expected_nodes,offset+0x10,t)
                        if t==0:
                            cursor=struct.unpack_from('<I',expected_nodes,offset)[0]
                            while True:
                                start=cursor;op,ms,values,cursor=records[start]
                                bases=(None,entity+0xAC,syms['D_800A77F0'],syms['D_8009DF70'],syms['D_800B6A80'])
                                args=tuple(start+8+i*4 if mode==0 else bases[mode]+v*4 for i,(mode,v) in enumerate(zip(ms,values)))
                                expected.append((op,args));first=len(expected)==1
                                if first:
                                    if behavior&2:cursor=programs[2]
                                    if behavior&4:current=nodes+0x80
                                    if behavior&8:entity=actors+0x300
                                if not(first and behavior&1):break
                            struct.pack_into('<I',expected_nodes,current-nodes,cursor)
                    current=struct.unpack_from('<I',expected_nodes,current-nodes+0x24)[0]
                actual.clear()
                for name,value in (('SP',0x801F0000),('RA',0x80010000),('GP',0x8009CD70)):
                    m.reg_write(getattr(R,'UC_MIPS_REG_'+name),value)
                for i,r in enumerate(saved):m.reg_write(r,0xABCD0000+i)
                m.emu_start(base,0x80010000,count=20000)
                assert actual==expected,(case,actual,expected)
                assert read(nodes,0xC0)==expected_nodes,case
                assert read(actors,0x600)==actor_guard and all(read(p,0x200)==guard for p,guard in program_guards.items()),case
                assert getword(syms['D_8009D300'])==0 and getword(syms['D_8009CE00'])==cursor and getword(syms['D_8009D2F0'])==entity,case
                assert m.reg_read(R.UC_MIPS_REG_PC)==0x80010000 and m.reg_read(R.UC_MIPS_REG_SP)==0x801F0000 and m.reg_read(R.UC_MIPS_REG_GP)==0x8009CD70,case
                for i,r in enumerate(saved):assert m.reg_read(r)==0xABCD0000+i,case
            print('candidate' if candidate else 'retail','verified',len(cases))


if __name__ == '__main__':
    unittest.main()
