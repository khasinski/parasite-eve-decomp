"""Retail-linked GPU timeout code and scripted MMIO/callback model."""
from pathlib import Path
import re
import shutil
import struct
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[2]


class GpuTimeoutRetailTests(unittest.TestCase):
    def test_source_has_no_assembly(self):
        source = (ROOT/'src/main/gpu/gpu3.c').read_text()
        source = re.sub(r'/\*.*?\*/|//[^\n]*', '', source, flags=re.S)
        self.assertNotRegex(source, r'\b(?:asm|__asm__|INCLUDE_ASM|CC_POSTPASS)\b')

    @unittest.skipUnless(shutil.which('mipsel-none-elf-ld') and
                         (ROOT/'build/USA/main.elf').is_file() and
                         (ROOT/'assets/USA/main.exe').is_file(), 'retail/toolchain unavailable')
    def test_bytes_and_timeout_model(self):
        try:
            from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN, UC_HOOK_CODE, UC_HOOK_MEM_READ, UC_HOOK_MEM_WRITE
            from unicorn import mips_const as R
        except ImportError:
            self.skipTest('unicorn unavailable')
        symbols = {}
        for line in subprocess.check_output(['mipsel-none-elf-nm',str(ROOT/'build/USA/main.elf')],text=True).splitlines():
            fields = line.split()
            if len(fields) == 3: symbols[fields[2]] = int(fields[0],16)
        base, entry, stop, stack = 0x800773D0, 0x80077404, 0x80010000, 0x801F0000
        retail = (ROOT/'assets/USA/main.exe').read_bytes()[0x67BD0:0x67DE8]
        with tempfile.TemporaryDirectory() as directory:
            work = Path(directory)
            subprocess.run([str(ROOT/'tools/scripts/cc.sh'),str(ROOT/'src/main/gpu/gpu3.c'),str(work/'test.o')],check=True,capture_output=True)
            needed = [line.split()[-1] for line in subprocess.check_output(['mipsel-none-elf-nm','-u',str(work/'test.o')],text=True).splitlines()]
            script = f'SECTIONS {{ .text 0x{base:X} : SUBALIGN(4) {{ *(.text) }} /DISCARD/ : {{ *(.reginfo) *(.mdebug) *(.pdr) *(.MIPS.abiflags) }} }}\n'
            (work/'test.ld').write_text(script+'\n'.join(f'{name} = 0x{symbols[name]:X};' for name in needed))
            subprocess.run(['mipsel-none-elf-ld','-EL','-T',str(work/'test.ld'),str(work/'test.o'),'-o',str(work/'test.elf')],check=True,capture_output=True)
            subprocess.run(['mipsel-none-elf-objcopy','-O','binary','-j','.text',str(work/'test.elf'),str(work/'test.bin')],check=True)
            compiled = (work/'test.bin').read_bytes()
        self.assertEqual(compiled,retail)
        gp1, chcr, madr, control = 0x180000,0x180004,0x180008,0x18000C
        head_addr = symbols['g_GpuDmaQueueHead'] & 0x1FFFFFFF
        tail_addr = symbols['g_GpuDmaQueueTail'] & 0x1FFFFFFF
        for now in (-1,0,100,101):
            for counter in (-1,0,0xF0000,0xF0001):
                for head,tail in ((0,0),(3,63),(0xFFFFFFFF,17)):
                    for mutate in (False,True):
                        expired = now > 100 or counter > 0xF0000
                        new_head = (head+7)&0xFFFFFFFF if mutate else head
                        expected = [('vsync',0xFFFFFFFF)]
                        if expired:
                            expected += [('gp1',0xAABBCCDD),('head',new_head),('tail',tail),('madr',0x12345000),('gp1',0x11223344),('chcr',0x01000401),
                                         ('printf',symbols['D_80011988'],(new_head-tail)&63,0x11223344,0x01000401,0x12345000),('mask',0),
                                         ('write_tail',0),('tail',0),('write_head',0),('write_chcr',0x401),('control',0x20),('write_control',0x820),
                                         ('write_gp1',0x02000000),('write_gp1',0x01000000),('mask',0x1357)]
                        for body in (retail,compiled):
                            m = Uc(UC_ARCH_MIPS,UC_MODE_MIPS32|UC_MODE_LITTLE_ENDIAN)
                            m.mem_map(0,0x200000)
                            def put(address,data): m.mem_write(address&0x1FFFFFFF,bytes(data))
                            def word(address,value): put(address,struct.pack('<I',value&0xFFFFFFFF))
                            def read_word(address): return struct.unpack('<I',m.mem_read(address&0x1FFFFFFF,4))[0]
                            put(base,body)
                            for name in ('VSync','printf','SetIntrMask'):
                                put(symbols[name],struct.pack('<III',0,0x03E00008,0))
                            for name,address in (('g_GpuGp1Ptr',gp1),('g_GpuDmaChcrPtr',chcr),('g_GpuDmaMadrPtr',madr),('g_GpuDmaControlRegPtr',control)):
                                word(symbols[name],0x80000000+address)
                            for address,value in ((gp1,0),(chcr,0x01000401),(madr,0x12345000),(control,0x20),(head_addr,head),(tail_addr,tail),
                                                  (symbols['g_GpuDmaTimeoutDeadline'],100),(symbols['g_GpuDmaWaitLoopCounter'],counter),(symbols['D_80095884'],0x2468)):
                                word(address,value)
                            m.reg_write(R.UC_MIPS_REG_SP,stack)
                            m.reg_write(R.UC_MIPS_REG_RA,stop)
                            for i in range(8): m.reg_write(getattr(R,f'UC_MIPS_REG_S{i}'),0xABCD0000+i)
                            events, gpu_reads = [],[0]
                            labels = {head_addr:'head',tail_addr:'tail',gp1:'gp1',chcr:'chcr',madr:'madr',control:'control'}
                            def read_hook(machine,access,address,size,value,data):
                                physical = address&0x1FFFFFFF
                                if physical not in labels: return
                                if physical == gp1:
                                    word(address,0xAABBCCDD if gpu_reads[0]==0 else 0x11223344)
                                    if gpu_reads[0]==0 and mutate: word(head_addr,new_head)
                                    gpu_reads[0] += 1
                                events.append((labels[physical],read_word(address)))
                            def write_hook(machine,access,address,size,value,data):
                                physical = address&0x1FFFFFFF
                                if physical in labels: events.append(('write_'+labels[physical],value))
                            def code_hook(machine,address,size,data):
                                args = [machine.reg_read(getattr(R,f'UC_MIPS_REG_A{i}')) for i in range(4)]
                                if address == symbols['VSync']+4:
                                    events.append(('vsync',args[0])); result = now
                                elif address == symbols['printf']+4:
                                    events.append(('printf',*args,read_word(machine.reg_read(R.UC_MIPS_REG_SP)+16))); result = 0
                                elif address == symbols['SetIntrMask']+4:
                                    events.append(('mask',args[0])); result = 0x1357
                                else: return
                                for name in ('V0','V1','A0','A1','A2','A3','T0','T1','T2','T3','T4','T5','T6','T7','T8','T9'):
                                    machine.reg_write(getattr(R,'UC_MIPS_REG_'+name),0xDEADCAFE)
                                machine.reg_write(R.UC_MIPS_REG_V0,result&0xFFFFFFFF)
                            m.hook_add(UC_HOOK_MEM_READ,read_hook)
                            m.hook_add(UC_HOOK_MEM_WRITE,write_hook)
                            m.hook_add(UC_HOOK_CODE,code_hook)
                            m.emu_start(entry,stop,count=1000)
                            self.assertEqual(events,expected,(now,counter,head,tail,mutate))
                            self.assertEqual(m.reg_read(R.UC_MIPS_REG_PC),stop)
                            self.assertEqual(m.reg_read(R.UC_MIPS_REG_V0),0xFFFFFFFF if expired else 0)
                            self.assertEqual(read_word(symbols['g_GpuDmaWaitLoopCounter']),(counter if now>100 else counter+1)&0xFFFFFFFF)
                            self.assertEqual(read_word(symbols['D_80095884']),0x1357 if expired else 0x2468)
                            self.assertEqual(m.reg_read(R.UC_MIPS_REG_SP),stack)
                            for i in range(8): self.assertEqual(m.reg_read(getattr(R,f'UC_MIPS_REG_S{i}')),0xABCD0000+i)


if __name__ == '__main__':
    unittest.main()
