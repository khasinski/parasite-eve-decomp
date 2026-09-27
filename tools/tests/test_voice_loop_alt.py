"""Backward voice-loop plain-function gate and callback differential coverage."""
from pathlib import Path
import itertools
import random
import struct
import unittest

ROOT = Path(__file__).resolve().parents[2]


class VoiceLoopAltTests(unittest.TestCase):
    def test_backward_loop_function_has_no_asm(self):
        source = (ROOT/'src/main/akao/voice_pitch.c').read_text()
        body = source.split('void Akao_SetVoiceLoopAddrAlt',1)[1].split('extern int g_AkaoPitchPeriodTable',1)[0]
        self.assertNotRegex(body,r'\b(?:asm|__asm__|INCLUDE_ASM|CC_POSTPASS)\b')

    @unittest.skipUnless((ROOT/'assets/USA/main.exe').is_file() and
                         (ROOT/'build/USA/main.exe').is_file(), 'retail/build unavailable')
    def test_retail_and_built_backward_loop(self):
        try:
            from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN, UC_HOOK_CODE
            from unicorn import mips_const as R
        except ImportError:
            self.skipTest('unicorn unavailable')
        # Entire TU exact gate, but only backward search is modeled here.
        # Lookup still has register pins: this is not a fully clean TU.
        base = 0x8008E4E8
        offset,size = base-0x8000F800,0x3E8
        retail = (ROOT/'assets/USA/main.exe').read_bytes()[offset:offset+size]
        compiled = (ROOT/'build/USA/main.exe').read_bytes()[offset:offset+size]
        self.assertEqual(len(retail),size)
        self.assertEqual(retail,compiled)
        retail_syms = candidate_syms = {
            'Akao_SetVoiceLoopAddrAlt':0x8008E664,
            'g_AkaoInstrumentTable':0x800B2900,
            'g_AkaoCurTrack':0x8009D2C8,
            'SeqOp_SetVoiceInstrument':0x8008F0D0,
        }
        stop,stack = 0x80010000,0x801F0000
        saved = [getattr(R,f'UC_MIPS_REG_S{i}') for i in range(8)]+[R.UC_MIPS_REG_GP,R.UC_MIPS_REG_FP]
        rng = random.Random(base)

        track,ranges,state = 0x80100020,0x80110020,0x80120000
        instrument_addr = retail_syms['g_AkaoInstrumentTable']
        callback = retail_syms['SeqOp_SetVoiceInstrument']
        callback_counts = set()
        for case,(length,arg,mode,match,mutate) in enumerate(itertools.product((0,1,3,7),(0,1,20,64,128,255,0xFFFFFFFF),(0,0x100),range(4),(False,True))):
            initial = bytearray(rng.randbytes(0x160))
            table = bytearray(rng.randbytes(0x100))
            notes = [0x10+i*7 for i in range(length)]+[0x80]
            for i,note in enumerate(notes):
                table[0x20+i*8] = note
                table[0x20+i*8+1] = 10+i*20
                table[0x20+i*8+2] = 20+i*20
            table[0x18] = 0x1F
            selected_note = (0xFFFF,notes[0],notes[-1]+(0x30 if mode else 0),0x1F)[match]
            struct.pack_into('<I',initial,0x20+0x18,ranges)
            struct.pack_into('<H',initial,0x20+0x5A,selected_note)
            instruments = rng.randbytes(0x5000)
            outputs = []
            for body,syms in ((retail,retail_syms),(compiled,candidate_syms)):
                m = Uc(UC_ARCH_MIPS,UC_MODE_MIPS32|UC_MODE_LITTLE_ENDIAN)
                m.mem_map(0,0x200000)
                def put(addr,data): m.mem_write(addr&0x1FFFFFFF,bytes(data))
                def read(addr,size): return bytes(m.mem_read(addr&0x1FFFFFFF,size))
                def word(addr,value): put(addr,struct.pack('<I',value&0xFFFFFFFF))
                put(base,body)
                put(track-0x20,initial)
                put(ranges-0x20,table)
                put(instrument_addr,instruments)
                word(retail_syms['g_AkaoCurTrack'],state)
                word(state,mode)
                put(callback,struct.pack('<III',0,0x03E00008,0))
                events = []
                def hook(machine,pc,size,user):
                    if pc == callback+4:
                        args = tuple(machine.reg_read(getattr(R,'UC_MIPS_REG_'+reg)) for reg in ('A0','A1','A2'))
                        assert args[0] == track and instrument_addr <= args[1] < instrument_addr+len(instruments),(case,args)
                        assert args[2] == int.from_bytes(read(args[1],4),'little'),(case,args)
                        events.append((args,read(track-0x20,len(initial))))
                        if mutate:
                            word(track+0xF4,0x13572468)
                            put(track+0x10E,b'\x55\x66')
                            # Callback may affect table fields subsequently copied.
                            for i in range(-1,length+1):
                                put(ranges+i*8+3,b'\x33\x44\x55\x66')
                        for reg in ('V0','V1','A0','A1','A2','A3','T0','T1','T2','T3','T4','T5','T6','T7','T8','T9'):
                            machine.reg_write(getattr(R,'UC_MIPS_REG_'+reg),0xDEADCAFE)
                m.hook_add(UC_HOOK_CODE,hook)
                for reg,val in (('A0',track),('A1',arg),('SP',stack),('RA',stop)):
                    m.reg_write(getattr(R,'UC_MIPS_REG_'+reg),val)
                for i,reg in enumerate(saved): m.reg_write(reg,0xABCD0000+i)
                m.emu_start(syms['Akao_SetVoiceLoopAddrAlt'],stop,count=2000)
                assert m.reg_read(R.UC_MIPS_REG_PC)==stop and m.reg_read(R.UC_MIPS_REG_SP)==stack,case
                for i,reg in enumerate(saved): assert m.reg_read(reg)==0xABCD0000+i,case
                assert read(instrument_addr,len(instruments))==instruments,case
                assert read(state,4)==struct.pack('<I',mode),case
                assert read(retail_syms['g_AkaoCurTrack'],4)==struct.pack('<I',state),case
                assert read(track-0x20,0x20)==initial[:0x20] and read(track+0x11C,0x24)==initial[0x13C:],case
                assert len(events)<=1,case
                callback_counts.add(len(events))
                outputs.append((events,read(track-0x20,len(initial)),read(ranges-0x20,len(table))))
            assert outputs[0]==outputs[1],(case,length,arg,mode,match,mutate)
        assert callback_counts == {0,1},callback_counts



if __name__ == '__main__': unittest.main()
