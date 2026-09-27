"""Independent bounded decimal-rendering model; no INT_MIN or power overflow.

Widths -2..10, finite sign strings, valid cursor-stack storage. Callback
mutations deliberately exercise live global reloads, including assert paths.
"""
from pathlib import Path
import itertools, struct, unittest

ROOT = Path(__file__).resolve().parents[2]


class DrawNumberRectTests(unittest.TestCase):
    def test_plain_source(self):
        source = (ROOT/'src/main/gpu/Draw_AllocTexturedRect.c').read_text()
        self.assertNotRegex(source, r'\b(?:asm|__asm__|INCLUDE_ASM|REGALLOC_BARRIER)\b')

    @unittest.skipUnless((ROOT/'assets/USA/main.exe').is_file() and
                         (ROOT/'build/USA/main.exe').is_file(), 'images unavailable')
    def test_decimal_rendering(self):
        try:
            from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN, UC_HOOK_CODE
            from unicorn import mips_const as R
        except ImportError:
            self.skipTest('unicorn unavailable')
        BASE, SIZE = 0x800602D0, 600
        X, Y, STACK = 0x8009D124, 0x8009D128, 0x8009D12C
        LO, HI, TEXT = 0x800A2270, 0x800A22B0, 0x80100020
        STOP, SP, GP = 0x80010000, 0x801F0000, 0x8009CD70
        offset = BASE - 0x8000F800
        retail, code = [(ROOT/p).read_bytes()[offset:offset+SIZE]
                        for p in ('assets/USA/main.exe', 'build/USA/main.exe')]
        self.assertEqual(len(code), SIZE)
        self.assertEqual(code, retail)
        hooks = {0x8005DC4C: 'lookup', 0x8005EED4: 'draw', 0x800527C0: 'assert'}
        values = (-2147483647, -1000000000, -101, -100, -10, -1, 0, 1, 9, 10, 99, 100, 101, 999999999, 2147483647)
        cases = list(itertools.product(values, range(-2,11), (None,b'',b'\x70',b'\x70\x00\xfe'), (0,7,8), (0,1,2,4,7)))
        regions = ((X-4,24), (LO-8,80))
        saved = [getattr(R, f'UC_MIPS_REG_S{i}') for i in range(8)] + [R.UC_MIPS_REG_FP]
        for label, body in (('retail', retail), ('candidate', code)):
            m = Uc(UC_ARCH_MIPS, UC_MODE_MIPS32 | UC_MODE_LITTLE_ENDIAN)
            m.mem_map(0, 0x200000)
            def put(a,b): m.mem_write(a & 0x1FFFFFFF, bytes(b))
            def get(a): return struct.unpack('<I', m.mem_read(a & 0x1FFFFFFF,4))[0]
            def set_(a,v): put(a,struct.pack('<I',v & 0xFFFFFFFF))
            put(BASE,body)
            for a in hooks: put(a,struct.pack('<II',0x03E00008,0))
            actual = []
            def callback(kind,arg,g,s,events):
                events.append((kind,arg,g(X),g(Y),g(STACK)))
                if kind == 'lookup':
                    if mutation & 1: s(X,g(X)+17); s(Y,g(Y)-13)
                    return TEXT if sign is not None else 0
                if kind == 'draw':
                    s(X,g(X)+9)
                    if mutation & 2: s(Y,g(Y)+3); s(STACK,LO+16)
                if kind == 'assert' and mutation & 4: s(STACK,LO)
                return 0
            def hook(uc,address,size,data):
                if address not in hooks: return
                result = callback(hooks[address],uc.reg_read(R.UC_MIPS_REG_A0),get,set_,actual)
                for name in ('V1','A0','A1','A2','A3','T0','T1','T2','T3','T4','T5','T6','T7','T8','T9'):
                    uc.reg_write(getattr(R,'UC_MIPS_REG_'+name),0xCCCCCCCC)
                uc.reg_write(R.UC_MIPS_REG_V0,result)
            m.hook_add(UC_HOOK_CODE,hook)
            for value,width,sign,depth,mutation in cases:
                case = (label,value,width,sign,depth,mutation)
                initial = {a:0xA5A5A5A5 for start,size in regions for a in range(start,start+size,4)}
                initial.update({X:0xFFFFFFFD,Y:101,STACK:LO+depth*8})
                for a,v in initial.items(): set_(a,v)
                text = b'\xa5'*8+(sign or b'')+b'\xff'+b'\x5a'*8
                put(TEXT-8,text)
                expected = dict(initial)
                events = []
                def g(a): return expected[a]
                def s(a,v): expected[a] = v & 0xFFFFFFFF
                def call(kind,arg): return callback(kind,arg,g,s,events)
                # Decimal-place model, independent of compiler division expansion.
                magnitude = abs(value)
                places = max(width-1,0)
                count = width
                if value < 0:
                    count -= 1
                    places -= 1
                    while places >= 0 and magnitude < 10**places:
                        call('draw',15)
                        places -= 1
                        count -= 1
                    call('lookup',0x70)
                    if sign is not None:
                        p = g(STACK)
                        if p < HI:
                            oldx,oldy = g(X),g(Y)
                            s(STACK,p+8); s(p,oldx); s(p+4,oldy)
                        else: call('assert',2)
                        for char in sign: call('draw',char)
                        p = g(STACK)
                        if p > LO:
                            oldx,oldy = g(p-8),g(p-4)
                            s(STACK,p-8); s(X,oldx); s(Y,oldy)
                        else: call('assert',3)
                    s(X,g(X)+9)
                for index in range(max(count,0)):
                    quotient = magnitude // 10**(places-index)
                    call('draw',15 if index < count-1 and quotient == 0 else quotient % 10)
                actual.clear()
                for name,v in (('A0',value),('A1',width),('SP',SP),('GP',GP),('RA',STOP)):
                    m.reg_write(getattr(R,'UC_MIPS_REG_'+name),v & 0xFFFFFFFF)
                for i,r in enumerate(saved): m.reg_write(r,0xABCD0000+i)
                m.emu_start(BASE,STOP,count=10000)
                assert actual == events, (case,actual,events)
                for a,v in expected.items(): assert get(a) == v, (case,hex(a),get(a),v)
                assert bytes(m.mem_read((TEXT-8)&0x1FFFFFFF,len(text))) == text, case
                assert m.reg_read(R.UC_MIPS_REG_PC) == STOP and m.reg_read(R.UC_MIPS_REG_SP) == SP and m.reg_read(R.UC_MIPS_REG_GP) == GP, case
                for i,r in enumerate(saved): assert m.reg_read(r) == 0xABCD0000+i, case
            print(label,'verified',len(cases),'cases',flush=True)


if __name__ == '__main__':
    unittest.main()
