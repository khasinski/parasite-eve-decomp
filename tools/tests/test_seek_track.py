"""Track lookup, cache invalidation and asynchronous loading against a model."""
from pathlib import Path
from importlib.util import find_spec
import itertools
import struct
import unittest

ROOT = Path(__file__).resolve().parents[2]
BASE, SIZE = 0x8006D2B8, 852
STATE, RECORD, BANK = 0x800B0CD8, 0x8009D180, 0x8009D184
BLOB, WORK, SLOT = 0x80100000, 0x80101000, 0x80102000
STOP, READ, COPY = 0x80086FF8, 0x8006CDA4, 0x80071A34


class SeekTrackTests(unittest.TestCase):
    @unittest.skipUnless((ROOT / 'assets/USA/main.exe').is_file() and
                         (ROOT / 'build/USA/main.exe').is_file() and
                         find_spec('unicorn'), 'images or unicorn unavailable')
    def test_model_and_exact_bytes(self):
        from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN, UC_HOOK_CODE
        from unicorn import mips_const as R
        offset = BASE - 0x8000F800
        retail, candidate = [(ROOT / p).read_bytes()[offset:offset + SIZE]
                             for p in ('assets/USA/main.exe', 'build/USA/main.exe')]
        self.assertEqual(candidate, retail)
        self.assertEqual(len(candidate), SIZE)
        cases = itertools.product((-1, 12, 127, 128, 65535), range(3), range(4),
                                  (0, 1), (0, -3), (0, 1, 2, 3), (0, 1, 7, 9),
                                  (0, 0x40, 0x80, 0xc0))
        cases = list(cases)
        ranges = ((STATE - 8, 0x1A8), (RECORD - 8, 24),
                  (BLOB, 0x400), (WORK - 8, 0x110), (SLOT - 8, 20))
        saved = [getattr(R, 'UC_MIPS_REG_S' + str(i)) for i in range(8)] + [R.UC_MIPS_REG_FP]
        for label, body in (('retail', retail), ('candidate', candidate)):
            m = Uc(UC_ARCH_MIPS, UC_MODE_MIPS32 | UC_MODE_LITTLE_ENDIAN)
            m.mem_map(0, 0x200000)
            def put(a, b): m.mem_write(a & 0x1fffffff, bytes(b))
            def read(a, n): return bytes(m.mem_read(a & 0x1fffffff, n))
            def ag(a, n=4): return int.from_bytes(read(a, n), 'little')
            def aset(a, v, n=4): put(a, (v & ((1 << (8*n)) - 1)).to_bytes(n, 'little'))
            def signed(v, bits): return v - (1 << bits) if v & (1 << (bits-1)) else v
            put(BASE, body)
            for a in (STOP, READ, COPY): put(a, struct.pack('<II', 0x03e00008, 0))
            actual_events, actual_context = [], {}
            def environment(call, args, g, s, events, context):
                events.append((call, args))
                if call == READ:
                    attempt = context.get('reads', 0)
                    context['reads'] = attempt + 1
                    # Retry, success and non-1 completion (including -1).
                    return (1 if attempt == 0 else 0) if scenario == 1 else -1
                if call == COPY:
                    destination, source, count = args
                    for i in range(count): s(destination+i, g(source+i, 1), 1)
                    if cache == 3:
                        s(SLOT, 1 - g(SLOT))
                        s(BANK, 0xff80, 2)
                if call == STOP and cache == 3:
                    s(STATE + 0x194, 0x80108000)
                return 0
            def hook(uc, address, size, data):
                if address not in (STOP, READ, COPY): return
                count = 6 if address == READ else 3 if address == COPY else 0
                args = tuple(uc.reg_read(getattr(R, 'UC_MIPS_REG_A'+str(i)))
                             if i < 4 else ag(uc.reg_read(R.UC_MIPS_REG_SP) + 0x10 + 4*(i-4))
                             for i in range(count))
                value = environment(address, args, ag, aset, actual_events, actual_context)
                for name in ('V0','V1','A0','A1','A2','A3','T0','T1','T2','T3','T4','T5','T6','T7','T8','T9'):
                    uc.reg_write(getattr(R, 'UC_MIPS_REG_'+name), 0xcccccccc)
                uc.reg_write(R.UC_MIPS_REG_V0, value & 0xffffffff)
            m.hook_add(UC_HOOK_CODE, hook)
            for key, records, cache, mode, group, blocking, scenario, flags in cases:
                regions = [(a, bytearray(b'\xa5' * n)) for a, n in ranges]
                def region(a, n):
                    for base, b in regions:
                        if base <= a and a+n <= base+len(b): return b, a-base
                    raise AssertionError(hex(a))
                def g(a, n=4):
                    b, i = region(a, n)
                    return int.from_bytes(b[i:i+n], 'little')
                def s(a, v, n=4):
                    b, i = region(a, n)
                    b[i:i+n] = (v & ((1 << (n*8))-1)).to_bytes(n, 'little')
                phase = scenario if scenario in (7, 9) else 0
                s(STATE, 0x12340000 | flags)
                s(STATE+0xF1, phase, 1)
                s(STATE+0x18C, BLOB); s(STATE+0x194, 0x80109000)
                s(STATE+0x128, WORK); s(STATE+0x12C, WORK+0x80)
                s(BLOB+4, 0x40); s(BLOB+0x70, (records << 22) | 0x100)
                for i in range(3):
                    entry = BLOB+0x100+12*i
                    s(entry, 0xab000007); s(entry+4, 0xcd000300)
                    s(entry+8, 0x8001 if records == 2 else 9, 2)
                    s(entry+10, key if i == 1 else 321, 2)
                for i in range(2):
                    s(STATE+0xDA+i, 0xff if cache == 3 and not records else 0x80+i, 1)
                    s(STATE+0xDC+2*i, key if cache in (i+1, 3) else 32+i, 1)
                s(SLOT, 0x12345678)
                s(RECORD, BLOB+0x100 if records else 0); s(BANK, 0x8001, 2)
                for i in range(16): s(BLOB+0x300+i, i*13+7, 1)
                for a, b in regions: put(a, b)
                actual_events.clear(); actual_context.clear()
                expected_events, context = [], {}
                def invoke(call, *args):
                    return environment(call, tuple(v & 0xffffffff for v in args), g, s,
                                       expected_events, context)
                def cached(i): return signed(g(STATE+0xDC+2*i, 1), 8) == key
                result = 0
                if phase == 0:
                    s(BANK, -1, 2); s(RECORD, BLOB+0x100)
                    matches = [i for i in range(records)
                               if g(BLOB+0x100+12*i+10, 2) == key]
                    if matches:
                        entry = BLOB+0x100+12*matches[0]
                        s(RECORD, entry); s(BANK, g(entry+8, 2), 2)
                    if g(BANK, 2) == 0xffff:
                        matches = [i for i in range(2) if cached(i)]
                        if matches:
                            s(RECORD, 0)
                            s(BANK, signed(g(STATE+0xDA+matches[0], 1), 8), 2)
                    if g(BANK, 2) == 0xffff:
                        s(SLOT, -1)
                    elif mode and (cached(0) or cached(1)):
                        index = 0 if cached(0) else 1
                        s(SLOT, -2 if g(STATE) & (0x40 << index) else index)
                    elif not mode:
                        for i in range(2):
                            if cached(i):
                                s(STATE+0xDC+2*i, -1, 1); s(STATE+0xDA+i, -1, 1)
                                s(STATE, g(STATE) & ~(0x40 << i)); s(SLOT, i)
                    else:
                        if not group: invoke(STOP)
                        s(STATE+0xF1, 7, 1); phase = 7
                if phase == 7:
                    while True:
                        ready = invoke(READ, 0, signed(g(BANK, 2), 16), 0,
                                       g(STATE+0x194), 0x21, blocking)
                        if ready != 1:
                            phase = 9; s(STATE+0xF1, 9, 1); break
                        result = 1
                        if not (blocking & 1): break
                if phase == 9:
                    s(SLOT, int(group != 0))
                    entry = g(RECORD)
                    if entry:
                        invoke(COPY, g(STATE+0x128+4*int(group != 0)),
                               g(STATE+0x18C)+(g(entry+4)&0xffffff), g(entry)&0xffffff)
                    result = 0
                    s(STATE+0xDC+2*g(SLOT), key, 1)
                    s(STATE+0xDA+g(SLOT), g(BANK, 2), 1); s(STATE+0xF1, 0, 1)
                for name, value in (('A0',key),('A1',mode),('A2',group),('A3',SLOT),
                                    ('SP',0x801f0000),('GP',0x8009CD70),('RA',0x80010000)):
                    m.reg_write(getattr(R, 'UC_MIPS_REG_'+name), value & 0xffffffff)
                aset(0x801f0010, blocking)
                for i, reg in enumerate(saved): m.reg_write(reg, 0xabcd0000+i)
                m.emu_start(BASE, 0x80010000, count=20000)
                case = (label, key, records, cache, mode, group, blocking, scenario, flags)
                self.assertEqual(m.reg_read(R.UC_MIPS_REG_V0), result, case)
                self.assertEqual(actual_events, expected_events, case)
                for a, b in regions: self.assertEqual(read(a, len(b)), b, case)
                self.assertEqual(m.reg_read(R.UC_MIPS_REG_PC), 0x80010000, case)
                self.assertEqual(m.reg_read(R.UC_MIPS_REG_SP), 0x801f0000, case)
                for i, reg in enumerate(saved): self.assertEqual(m.reg_read(reg), 0xabcd0000+i, case)


if __name__ == '__main__': unittest.main()
