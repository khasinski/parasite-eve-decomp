"""Retail byte and machine-behavior checks for packed TIM uploads.

LoadImage is stubbed. Extreme pointer values exercise MIPS address arithmetic,
not a claim that out-of-object pointer arithmetic is portable ISO C.
"""
from pathlib import Path
import random
import shutil
import struct
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[2]


@unittest.skipUnless(shutil.which('mipsel-none-elf-ld') and (ROOT/'assets/USA/main.exe').is_file(), 'retail image/toolchain unavailable')
class GpuLoadTimAssetTests(unittest.TestCase):
    def test_retail_bytes_and_packet_model(self):
        try:
            from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN, UC_HOOK_CODE
            from unicorn import mips_const as R
        except ImportError:
            self.skipTest('unicorn unavailable')
        ENTRY,EXIT,STACK,ASSET,UPLOAD=0x8006E1C0,0x80010000,0x801F0000,0x80100000,0x8007506C
        source = 'src/main/gpu/misc13.c'
        retail=(ROOT/'assets/USA/main.exe').read_bytes()[0x5E9C0:0x5EB38]
        with tempfile.TemporaryDirectory() as directory:
         work=Path(directory)
         subprocess.run([str(ROOT/'tools/scripts/cc.sh'),str(ROOT/source),str(work/'test.o')],check=True,capture_output=True)
         (work/'test.ld').write_text('SECTIONS { .text 0x8006E1C0 : SUBALIGN(4) { *(.text) } /DISCARD/ : { *(.reginfo) *(.mdebug) *(.pdr) *(.MIPS.abiflags) } }\nLoadImage = 0x8007506C; g_Base32CharTable = 0x800930B4;\n')
         subprocess.run(['mipsel-none-elf-ld','-EL','-T',str(work/'test.ld'),str(work/'test.o'),'-o',str(work/'test.elf')],check=True,capture_output=True)
         subprocess.run(['mipsel-none-elf-objcopy','-O','binary','-j','.text',str(work/'test.elf'),str(work/'test.bin')],check=True,capture_output=True)
         compiled=(work/'test.bin').read_bytes()
        self.assertEqual(compiled, retail, 'Complete translation unit must match retail')
        rng=random.Random(ENTRY)
        for case in range(1024):
         words=[rng.getrandbits(32) for _ in range(5)]
         if case%4==0:words[1]&=0xFFFFFF
         if case%3==0:words[3]&=0xFF000000
         replacement=[rng.getrandbits(32) for _ in range(5)] if case%2 else words[:]
         if case%5==0:replacement[3]&=0xFF000000
         base=(0,0x80110000,0x7FFFFFFF,0xFFFFFFFF)[case%4]
         def packet(geometry,height,pointer):
          return struct.pack('<HHHH',(geometry>>10)&2047,geometry>>21,geometry&1023,height),pointer&0xFFFFFFFF
         expected=[packet(words[2],(words[1]>>24) or 256,base+(words[1]&0xFFFFFF))]
         if replacement[3]&0xFFFFFF:
          expected.append(packet(replacement[4],replacement[3]>>24,base+(replacement[1]&0xFFFFFF)+(replacement[3]&0xFFFFFF)))
         for name,body in (('retail',retail),('candidate',compiled)):
          m=Uc(UC_ARCH_MIPS,UC_MODE_MIPS32|UC_MODE_LITTLE_ENDIAN)
          m.mem_map(0,0x200000)
          def put(addr,data):m.mem_write(addr&0x1FFFFFFF,bytes(data))
          put(ENTRY,body)
          put(ASSET,struct.pack('<5I',*words))
          m.reg_write(R.UC_MIPS_REG_A0,ASSET)
          m.reg_write(R.UC_MIPS_REG_A1,base)
          m.reg_write(R.UC_MIPS_REG_SP,STACK)
          m.reg_write(R.UC_MIPS_REG_RA,EXIT)
          for i in range(8):m.reg_write(getattr(R,f'UC_MIPS_REG_S{i}'),0xABCD0000+i)
          events=[]
          def hook(machine,address,size,data):
           if address!=UPLOAD:return
           rect=machine.reg_read(R.UC_MIPS_REG_A0)
           pointer=machine.reg_read(R.UC_MIPS_REG_A1)
           events.append((bytes(machine.mem_read(rect&0x1FFFFFFF,8)),pointer))
           if len(events)==1:put(ASSET,struct.pack('<5I',*replacement))
           for reg in ('V1','A0','A1','A2','A3','T0','T1','T2','T3','T4','T5','T6','T7','T8','T9'):
            machine.reg_write(getattr(R,'UC_MIPS_REG_'+reg),0xCAFE0000)
           machine.reg_write(R.UC_MIPS_REG_V0,0xBADCAFE)
           machine.reg_write(R.UC_MIPS_REG_PC,machine.reg_read(R.UC_MIPS_REG_RA))
          m.hook_add(UC_HOOK_CODE,hook)
          m.emu_start(ENTRY,EXIT,count=1000)
          assert m.reg_read(R.UC_MIPS_REG_PC)==EXIT,(case,name,'return')
          assert m.reg_read(R.UC_MIPS_REG_V0)==0,(case,name,'value')
          assert m.reg_read(R.UC_MIPS_REG_SP)==STACK,(case,name,'stack')
          assert all(m.reg_read(getattr(R,f'UC_MIPS_REG_S{i}'))==0xABCD0000+i for i in range(8)),(case,name,'saved')
          assert events==expected,(case,name,events,expected)
          assert bytes(m.mem_read(ASSET&0x1FFFFFFF,20))==struct.pack('<5I',*replacement),(case,name,'asset modified')
