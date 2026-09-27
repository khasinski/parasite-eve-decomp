"""RAM model of draw/display initialization and callback mutation order."""
from pathlib import Path
from importlib.util import find_spec
import hashlib
import random
import struct
import unittest

ROOT = Path(__file__).resolve().parents[2]
SYMBOLS = {
    'Draw_InitBuffers': 0x8005E588,
    '_gp': 0x8009CD70,
    'g_DrawBufferFrontBases': 0x800A21F4,
    'g_RenderFrontBufferBase': 0x800B0E50,
    'g_RenderBackBufferBase': 0x800B0E54,
    'g_DrawBufferOtBases': 0x800A21F0,
    'D_800A2268': 0x800A2268,
    'D_800A226C': 0x800A226C,
    'g_OtBufferTable': 0x800B0E38,
    'g_RenderOtBufferBaseAlt': 0x800B0E3C,
    'g_TextCursorX': 0x8009D124,
    'g_TextCursorY': 0x8009D128,
    'g_TextCursorStackPtr': 0x8009D12C,
    'g_TextCursorStackBottom': 0x800A2270,
    'g_DrawGradientBlendColor': 0x8009D130,
    'g_DrawPresentImage': 0x8009D134,
    'SetDefDrawEnv': 0x80074924,
    'SetDefDispEnv': 0x800749D8,
    'Draw_SetColor': 0x8005E968,
    'Draw_SetFontVariant': 0x8005F844,
}


def check_model(images):
    from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN, UC_HOOK_CODE
    from unicorn import mips_const as R
    symbols = SYMBOLS
    entry, stop, stack = symbols['Draw_InitBuffers'], 0x80010000, 0x801F0000
    region,size = 0x8009C000,0x15000
    base = symbols['g_DrawBufferFrontBases']
    callbacks = ('SetDefDrawEnv','SetDefDispEnv','Draw_SetColor','Draw_SetFontVariant')
    rng = random.Random(entry)
    count = 0
    for case in range(64):
        initial = rng.randbytes(size)
        for mutate in (False,True):
            expected = bytearray(initial)
            def model_put(address,data): expected[address-region:address-region+len(data)] = data
            def model_word(name,value): model_put(symbols[name],struct.pack('<I',value&0xFFFFFFFF))
            def model_read(name): return struct.unpack_from('<I',expected,symbols[name]-region)[0]
            events = []
            def effect(name,args,put,word):
                if name in ('SetDefDrawEnv','SetDefDispEnv'):
                    length = 0x5C if name=='SetDefDrawEnv' else 0x14
                    put(args[0],bytes(((i+case*7+args[2])&255) for i in range(length)))
                if mutate:
                    word('g_TextCursorX',0x12000000+case)
                    word('g_TextCursorY',0x34000000+case)
                    word('g_DrawGradientBlendColor',0x56000000+case)
                    word('g_DrawPresentImage',0x78000000+case)
            def call(name,*args):
                events.append((name,args,hashlib.sha256(expected).digest()))
                effect(name,args,model_put,model_word)
            model_word('g_DrawBufferFrontBases',model_read('g_RenderFrontBufferBase'))
            model_word('D_800A226C',model_read('g_RenderBackBufferBase'))
            model_word('g_DrawBufferOtBases',model_read('g_OtBufferTable'))
            model_word('D_800A2268',model_read('g_RenderOtBufferBaseAlt'))
            call('SetDefDrawEnv',base-0x74,0,0,320,224)
            call('SetDefDrawEnv',base+4,0,224,320,224)
            for address,value in ((0x800A2210,1),(0x800A2198,1),(0x800A2199,0),(0x800A219A,0),(0x800A219B,0),(0x800A2211,0),(0x800A2212,0),(0x800A2213,0)):
                model_put(address,bytes([value]))
            call('SetDefDispEnv',base-0x18,0,224,320,224)
            call('SetDefDispEnv',base+0x60,0,0,320,224)
            for offset,value in ((0x6A,8),(-0xE,8),(0x6E,224),(-0xA,224)):
                model_put(base+offset,struct.pack('<H',value))
            model_word('g_TextCursorY',0)
            model_word('g_TextCursorX',0)
            model_word('g_TextCursorStackPtr',symbols['g_TextCursorStackBottom'])
            call('Draw_SetColor',0x808080)
            model_word('g_DrawGradientBlendColor',0)
            call('Draw_SetFontVariant',0)
            model_word('g_DrawPresentImage',0)
            for label, body in images:
                m = Uc(UC_ARCH_MIPS,UC_MODE_MIPS32|UC_MODE_LITTLE_ENDIAN)
                m.mem_map(0,0x200000)
                def put(address,data): m.mem_write(address&0x1FFFFFFF,bytes(data))
                def word(name,value): put(symbols[name],struct.pack('<I',value&0xFFFFFFFF))
                put(entry,body)
                put(region,initial)
                put(stack - 96, b'\xC3' * 128)
                for name in callbacks: put(symbols[name],struct.pack('<III',0,0x03E00008,0))
                for name,value in (('RA',stop),('SP',stack),('GP',symbols['_gp'])):
                    m.reg_write(getattr(R,'UC_MIPS_REG_'+name),value)
                for i in range(8): m.reg_write(getattr(R,f'UC_MIPS_REG_S{i}'),0xABCD0000+i)
                m.reg_write(R.UC_MIPS_REG_FP, 0xABCD0008)
                observed = []
                def hook(machine,address,length,data):
                    matches = [name for name in callbacks if address==symbols[name]+4]
                    if not matches: return
                    name = matches[0]
                    args = tuple(machine.reg_read(getattr(R,f'UC_MIPS_REG_A{i}')) for i in range(4 if name.startswith('SetDef') else 1))
                    if name.startswith('SetDef'):
                        sp = machine.reg_read(R.UC_MIPS_REG_SP)
                        args += struct.unpack('<I',machine.mem_read((sp+16)&0x1FFFFFFF,4))
                    observed.append((name,args,hashlib.sha256(bytes(machine.mem_read(region&0x1FFFFFFF,size))).digest()))
                    effect(name,args,put,word)
                    for reg in ('V0','V1','A0','A1','A2','A3','T0','T1','T2','T3','T4','T5','T6','T7','T8','T9'):
                        machine.reg_write(getattr(R,'UC_MIPS_REG_'+reg),0xDEADCAFE)
                m.hook_add(UC_HOOK_CODE,hook)
                m.emu_start(entry,stop,count=1000)
                assert observed==events,(case,mutate,observed,events)
                assert bytes(m.mem_read(region&0x1FFFFFFF,size))==bytes(expected),(case,mutate)
                assert m.reg_read(R.UC_MIPS_REG_PC)==stop
                assert m.reg_read(R.UC_MIPS_REG_SP)==stack
                assert m.reg_read(R.UC_MIPS_REG_GP)==symbols['_gp']
                assert m.reg_read(R.UC_MIPS_REG_FP)==0xABCD0008
                assert bytes(m.mem_read((stack - 96) & 0x1FFFFFFF, 56))==b'\xC3'*56
                assert bytes(m.mem_read(stack & 0x1FFFFFFF, 32))==b'\xC3'*32
                for i in range(8): assert m.reg_read(getattr(R,f'UC_MIPS_REG_S{i}'))==0xABCD0000+i
            count += 1
    return count


class DrawInitBuffersTests(unittest.TestCase):
    @unittest.skipUnless((ROOT / 'assets/USA/main.exe').is_file() and
                         (ROOT / 'build/USA/main.exe').is_file() and find_spec('unicorn'),
                         'images or unicorn unavailable')
    def test_exact_bytes_and_model(self):
        offset = SYMBOLS['Draw_InitBuffers'] - 0x8000F800
        images = [(name, (ROOT / name).read_bytes()[offset:offset + 348])
                  for name in ('assets/USA/main.exe', 'build/USA/main.exe')]
        self.assertEqual(images[0][1], images[1][1])
        self.assertEqual(check_model(images), 128)


if __name__ == '__main__':
    unittest.main()
