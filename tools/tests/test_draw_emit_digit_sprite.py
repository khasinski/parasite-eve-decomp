"""Retail bytes and packet model; exhausted-buffer paths stop at assertion call."""
from pathlib import Path
import random
import shutil
import struct
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[2]


@unittest.skipUnless(shutil.which('mipsel-none-elf-ld') and (ROOT/'assets/USA/main.exe').is_file() and (ROOT/'build/USA/main.elf').is_file(), 'retail/toolchain/build unavailable')
class DrawEmitDigitSpriteTests(unittest.TestCase):
    def test_preprocessed_unit_has_no_assembly(self):
        result = subprocess.run(
            [str(ROOT/'tools/psyq-gcc-2.7.2/cpp'), '-P', '-undef',
             '-D__GNUC__=2', '-D__OPTIMIZE__', '-Dmips', '-D__mips__',
             '-D__LITTLE_ENDIAN__', '-I'+str(ROOT/'include'),
             str(ROOT/'src/main/gpu/Draw_EmitDigitSprite.c')],
            check=True, capture_output=True, text=True)
        self.assertNotRegex(result.stdout, r'\b(?:asm|__asm__)\b')

    def test_retail_bytes_and_packet_model(self):
        try:
            from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN, UC_HOOK_CODE
            from unicorn import mips_const as R
        except ImportError:
            self.skipTest('unicorn unavailable')
        ENTRY, EXIT, STACK = 0x8005F874, 0x80010000, 0x801F0000
        ARENA, OT = 0x80100000, 0x80110000
        symbols = {}
        for line in subprocess.check_output(['mipsel-none-elf-nm', str(ROOT/'build/USA/main.elf')], text=True).splitlines():
            fields = line.split()
            if len(fields) == 3:
                symbols[fields[2]] = int(fields[0], 16)
        retail = (ROOT/'assets/USA/main.exe').read_bytes()[0x50074:0x5023C]
        source = ROOT/'src/main/gpu/Draw_EmitDigitSprite.c'
        with tempfile.TemporaryDirectory() as directory:
            work = Path(directory)
            subprocess.run([str(ROOT/'tools/scripts/cc.sh'),str(source),str(work/'test.o')],check=True,capture_output=True)
            needed = [line.split()[-1] for line in subprocess.check_output(['mipsel-none-elf-nm','-u',str(work/'test.o')],text=True).splitlines()]
            script = 'SECTIONS { .text 0x8005F874 : SUBALIGN(4) { *(.text) } /DISCARD/ : { *(.reginfo) *(.mdebug) *(.pdr) *(.MIPS.abiflags) } }\n'
            script += '\n'.join(f'{name} = 0x{symbols[name]:X};' for name in set(needed+['_gp']))
            (work/'test.ld').write_text(script)
            subprocess.run(['mipsel-none-elf-ld','-EL','-T',str(work/'test.ld'),str(work/'test.o'),'-o',str(work/'test.elf')],check=True,capture_output=True)
            subprocess.run(['mipsel-none-elf-objcopy','-O','binary','-j','.text',str(work/'test.elf'),str(work/'test.bin')],check=True,capture_output=True)
            compiled = (work/'test.bin').read_bytes()
        self.assertEqual(compiled, retail)
        rng = random.Random(ENTRY)
        for case in range(512):
            digit = (-2147483648, -1, 0, 1, 9, 10, 99, 2147483647)[(case // 4) % 8] if case < 64 else rng.randint(-2147483648, 2147483647)
            offset = (0, 0x3FD4, 0x3FD8, 0x3FDC)[case % 4]
            pointer = ARENA + offset
            failed = offset + 40 >= 0x4000
            initial = rng.randbytes(40)
            ot_word, primary, alternate = [rng.getrandbits(32) for _ in range(3)]
            select = (0, 1, -1)[case % 3]
            x, y = rng.getrandbits(32), rng.getrandbits(32)
            base_u, base_v = rng.randint(-1024, 1024), rng.randint(-1024, 1024)
            clut = rng.getrandbits(32)
            expected = bytearray(initial)
            expected_ot = ot_word
            if not failed:
                struct.pack_into('<I', expected, 0, 0x09000000 | (ot_word & 0xFFFFFF))
                struct.pack_into('<I', expected, 4, 0x2C000000 | ((alternate if select else primary) & 0xFFFFFF))
                for at, value in ((8,x), (10,y), (16,x+5), (18,y), (24,x), (26,y+7), (32,x+5), (34,y+7), (14,clut), (22,7)):
                    struct.pack_into('<H', expected, at, value & 0xFFFF)
                u, v = (base_u+(digit % 10)*5, base_v) if digit >= 0 else (88, 164)
                for at, value in ((12,u), (13,v), (20,u+5), (21,v), (28,u), (29,v+7), (36,u+5), (37,v+7)):
                    expected[at] = value & 255
                expected_ot = (ot_word & 0xFF000000) | (pointer & 0xFFFFFF)
            for body in (retail, compiled):
                machine = Uc(UC_ARCH_MIPS, UC_MODE_MIPS32 | UC_MODE_LITTLE_ENDIAN)
                machine.mem_map(0, 0x200000)
                def put(address, value): machine.mem_write(address & 0x1FFFFFFF, bytes(value))
                def word(address, value): put(address, struct.pack('<I', value & 0xFFFFFFFF))
                put(ENTRY, body)
                put(pointer, initial)
                word(OT, ot_word)
                values = dict(D_8009D100=pointer, D_8009D104=ARENA, D_8009D10C=select,
                              D_8009D110=primary, D_8009D114=alternate, D_8009D11C=OT,
                              D_8009D124=x, D_8009D128=y, g_DrawDigitFontBaseTexU=base_u,
                              g_DrawDigitFontBaseTexV=base_v, g_DrawDigitFontTpageClut=clut)
                for name, value in values.items(): word(symbols[name], value)
                machine.reg_write(R.UC_MIPS_REG_A0, digit & 0xFFFFFFFF)
                machine.reg_write(R.UC_MIPS_REG_GP, symbols['_gp'])
                machine.reg_write(R.UC_MIPS_REG_SP, STACK)
                machine.reg_write(R.UC_MIPS_REG_RA, EXIT)
                for i in range(8): machine.reg_write(getattr(R, f'UC_MIPS_REG_S{i}'), 0xABCD0000+i)
                events = []
                def hook(m, address, size, user):
                    if address == symbols['BoundsCheck_AssertStub']:
                        events.append(m.reg_read(R.UC_MIPS_REG_A0))
                        m.emu_stop()
                machine.hook_add(UC_HOOK_CODE, hook)
                machine.emu_start(ENTRY, EXIT, count=1000)
                assert events == ([1] if failed else []), (case, events)
                if not failed:
                    assert machine.reg_read(R.UC_MIPS_REG_PC) == EXIT
                    assert machine.reg_read(R.UC_MIPS_REG_SP) == STACK
                    for i in range(8): assert machine.reg_read(getattr(R, f'UC_MIPS_REG_S{i}')) == 0xABCD0000+i
                else:
                    assert machine.reg_read(R.UC_MIPS_REG_PC) == symbols['BoundsCheck_AssertStub']
                assert bytes(machine.mem_read(pointer & 0x1FFFFFFF, 40)) == expected, case
                assert bytes(machine.mem_read(OT & 0x1FFFFFFF, 4)) == struct.pack('<I', expected_ot), case
                values['D_8009D100'] = pointer if failed else pointer + 40
                for name, value in values.items():
                    assert bytes(machine.mem_read(symbols[name] & 0x1FFFFFFF, 4)) == struct.pack('<I', value & 0xFFFFFFFF), (case, name)
