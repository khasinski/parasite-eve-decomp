"""Voice selection differential model: real recursion, controlled audio callbacks."""
from pathlib import Path
import itertools
import random
import re
import struct
import unittest

ROOT = Path(__file__).resolve().parents[2]


class SpuManageVoicesTests(unittest.TestCase):
    def test_function_is_plain_typed_c(self):
        source = (ROOT/'src/main/akao/Akao_NestedTrack.c').read_text()
        match = re.search(r'void Spu_ManageVoices\([^)]*\)\s*\{',source)
        self.assertIsNotNone(match)
        body = source[match.start():source.index('\n#include',match.start())]
        body = re.sub(r'/\*.*?\*/|//[^\n]*','',body,flags=re.S)
        self.assertNotRegex(body,r'\b(?:asm|__asm__|INCLUDE_ASM|CC_POSTPASS)\b')
        self.assertIn('field->key_on_mask',body)

    @unittest.skipUnless((ROOT/'assets/USA/main.exe').is_file() and
                         (ROOT/'build/USA/main.exe').is_file(), 'retail/build unavailable')
    def test_retail_and_built_selection(self):
        try:
            from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN, UC_HOOK_CODE
            from unicorn import mips_const as R
        except ImportError:
            self.skipTest('unicorn unavailable')
        symbols = {
            'g_AkaoVoiceChannelTable': 0x800BC000,
            'SeqOp_DeactivateVoice': 0x8008F1B0,
            'Seq_MarkTrack34MaskDirty': 0x80089960,
            'Seq_MarkTrack38MaskDirty': 0x80089B28,
            'Seq_MarkTrack3CMaskDirty': 0x80089CF0,
            'g_SpuActiveVoiceMask': 0x800BCD50,
            'g_SpuPendingKeyOffMask': 0x800BCD5C,
            'g_AkaoVoiceUpdateFlags': 0x8009D2C4,
        }
        entry = base = 0x8008A400
        stop,stack = 0x80010000,0x801F0000
        table = symbols['g_AkaoVoiceChannelTable']
        names = ('SeqOp_DeactivateVoice','Seq_MarkTrack34MaskDirty','Seq_MarkTrack38MaskDirty','Seq_MarkTrack3CMaskDirty')
        callbacks = {symbols[name]:name for name in names}
        globals_list = [symbols[name] for name in ('g_SpuActiveVoiceMask','g_SpuPendingKeyOffMask','g_AkaoVoiceUpdateFlags')]
        retail = (ROOT/'assets/USA/main.exe').read_bytes()[entry-0x8000F800:entry-0x8000F800+0x350]
        compiled = (ROOT/'build/USA/main.exe').read_bytes()[entry-0x8000F800:entry-0x8000F800+0x350]
        self.assertEqual(len(retail),0x350)
        self.assertEqual(compiled,retail)
        saved = [getattr(R,f'UC_MIPS_REG_S{i}') for i in range(8)]+[R.UC_MIPS_REG_GP,R.UC_MIPS_REG_FP]
        rng = random.Random(entry)
        saw_recursion = False
        callback_coverage = set()
        cases = itertools.product((0,1,5,10,0xFFFF),(0,1,0x1000,0x40000000,0x80000000,0x80000001),(0,0x1000,0x555000,0xFFF000),(False,True))
        for case,(voice_id,control,active,mutate) in enumerate(cases):
            initial = bytearray(rng.randbytes(0xD50+0x20))
            for i in range(12):
                offset = 0x10+i*0x11C
                for field,value in ((0x28,(0,1,5,10,0xFFFF)[i%5]),(0x2C,(0,1,0x1000)[i%3]),
                                    (0x38,(0,0x100000,0x200000,0x300000)[i%4]),(0x50,(0,1,0x7FFFFFFF,0x80000000,0xFFFFFFFF)[i%5])):
                    struct.pack_into('<I',initial,offset+field,value)
            global_values = [active,rng.getrandbits(32),rng.getrandbits(32)]
            outputs = []
            for address,body in ((entry,retail),(base,compiled)):
                m = Uc(UC_ARCH_MIPS,UC_MODE_MIPS32|UC_MODE_LITTLE_ENDIAN)
                m.mem_map(0,0x200000)
                def put(addr,data): m.mem_write(addr&0x1FFFFFFF,bytes(data))
                def word(addr,value): put(addr,struct.pack('<I',value&0xFFFFFFFF))
                put(address,body)
                put(table-0x10,initial)
                for addr,value in zip(globals_list,global_values): word(addr,value)
                for addr in callbacks: put(addr,struct.pack('<III',0,0x03E00008,0))
                events = []
                entries = [0]
                def hook(machine,pc,size,user):
                    if pc == entry: entries[0] += 1
                    name = callbacks.get(pc-4)
                    if name:
                        callback_coverage.add(name)
                        if name=='SeqOp_DeactivateVoice':
                            ptr,mask = [machine.reg_read(getattr(R,'UC_MIPS_REG_'+reg)) for reg in ('A0','A1')]
                            assert table<=ptr<table+0xD50 and (ptr-table)%0x11C==0,(case,ptr)
                            events.append((name,ptr,mask))
                            if mutate: word(ptr+0x38,0xBADCAFE)
                        else: events.append((name,))
                        if mutate:
                            for addr in globals_list:
                                old = int.from_bytes(machine.mem_read(addr&0x1FFFFFFF,4),'little')
                                word(addr,old^0x55555555)
                        for reg in ('V0','V1','A0','A1','A2','A3','T0','T1','T2','T3','T4','T5','T6','T7','T8','T9'):
                            machine.reg_write(getattr(R,'UC_MIPS_REG_'+reg),0xDEADCAFE)
                m.hook_add(UC_HOOK_CODE,hook)
                for name,value in (('A0',voice_id),('A1',control),('SP',stack),('RA',stop)):
                    m.reg_write(getattr(R,'UC_MIPS_REG_'+name),value)
                for i,reg in enumerate(saved): m.reg_write(reg,0xABCD0000+i)
                m.emu_start(entry,stop,count=10000)
                assert m.reg_read(R.UC_MIPS_REG_PC)==stop and m.reg_read(R.UC_MIPS_REG_SP)==stack,case
                for i,reg in enumerate(saved): assert m.reg_read(reg)==0xABCD0000+i,case
                saw_recursion |= entries[0] > 1
                if voice_id == 0xFFFF:
                    assert entries[0] == 1 and not events, case
                outputs.append((events,bytes(m.mem_read((table-0x10)&0x1FFFFFFF,len(initial))),[bytes(m.mem_read(addr&0x1FFFFFFF,4)) for addr in globals_list],entries[0]))
            assert outputs[0]==outputs[1],case
        assert saw_recursion and callback_coverage == set(names)


if __name__ == '__main__': unittest.main()

