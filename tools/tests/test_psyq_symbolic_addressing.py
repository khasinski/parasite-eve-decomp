"""Whole retail TU byte regressions for symbolic-addressing fixes.

Link addresses and SHA256 digests are frozen from the USA retail code ranges;
these tests need no extracted game assets or pre-existing build products.
"""
import hashlib
import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]

CASES = [('src/main/psyq/libapi/pad.c',
  28,
  '426f43966384e9ddbd0dacb3bb5c754b92ba875ffdeaee50173767cc5927ce7a',
  'g_InitPadFlag = 0x8009B4AC;\n'
  'SECTIONS { .text 0x8007dea4 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }'),
 ('src/main/psyq/libpad/waitrc2.c',
  192,
  '1795d848e518e058eb629903c8f624ed60f2d7b4315c553f78207b376a3a9b92',
  'g_TimerTimeoutLimit = 0x800BD02C;\n'
  'g_TimerTimeoutStart = 0x800A76D0;\n'
  'SECTIONS { .text 0x80084fc4 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }'),
 ('src/main/psyq/libgpu/ext.c',
  672,
  '3a25ec7c5052da04717d07bcd8e6a892675b1e1652248db012e7694c95229a75',
  'LoadImage = 0x8007506c;\n'
  'GetTPage = 0x80077a64;\n'
  'GetClut = 0x80077aa4;\n'
  'GetVideoMode = 0x80074a28;\n'
  'SECTIONS { .text 0x80074774 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }'),
 ('src/main/psyq/libetc/vmode.c',
  36,
  'f59613af52070699e2282716b260db31b6dfda3e8b3f91918e52f1e56017d853',
  'g_VideoMode = 0x800956ec;\n'
  'SECTIONS { .text 0x80074a14 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }'),
 ('src/main/cdrom/cd_tail.c',
  536,
  '309628923767c77ba53c659fff5acbec885e9a9eaba554ae2020803b3eef364a',
  'CD_vol = 0x8007b964;\n'
  'CD_getsector = 0x8007bf44;\n'
  'CD_getsector2 = 0x8007c044;\n'
  'CD_datasync = 0x8007bddc;\n'
  'CD_debug = 0x8009afc0;\n'
  'SECTIONS { .text 0x80080ac4 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }'),
 ('src/main/psyq/libmath/ferr.c',
  100,
  'b94427dfc0967b4aaeacb5750ae5b1e167af9b260ba712e561050f8c6d6ff179',
  'D_80094564 = 0x80094564;\n'
  'D_80094568 = 0x80094568;\n'
  'DeliverEvent = 0x80073a34;\n'
  'SECTIONS { .text 0x800739c4 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }'),
 ('src/main/psyq/libpad/padseqd.c',
  52,
  '9f58f31b9a795eeb97b583930590eef95892a1ba629c367c21ddde091ea87eb1',
  'CardObj_IsTransferActive = 0x80084F8C;\n'
  'func_80084B78 = 0x80084B78;\n'
  'g_MemCardIsTransferActiveFn = 0x8009B740;\n'
  'g_MemCardResponseHandler = 0x8009B744;\n'
  'g_MemCardStateDispatchFn = 0x8009B73C;\n'
  'LIBPAD_PADSEQD_text_108 = 0x80084C4C;\n'
  'SECTIONS { .text 0x80084b44 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }'),
 ('src/main/psyq/libds/CdRom_SeekDoneCallback.c',
  64,
  '3e9c23ff8921bfb43accf28fb43951d8b2614ae49b3c09a313efd642eb50d3b3',
  'LIBDS_DSREADY_text_FC = 0x80081e70;\n'
  'DsSyncCallback = 0x800824c8;\n'
  'g_DsReadBusy = 0x8009b70c;\n'
  'SECTIONS { .text 0x8008227c : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }'),
 ('src/main/psyq/libgpu/sys.c',
  1952,
  'e659ffcf3ce6e4cb3fe25b7805924d60dd70ca6e77145b75856b26798698a36d',
  'D_800117E0 = 0x800117e0;\n'
  'D_80011800 = 0x80011800;\n'
  'D_80011814 = 0x80011814;\n'
  'D_80011840 = 0x80011840;\n'
  'D_80011854 = 0x80011854;\n'
  'D_80011870 = 0x80011870;\n'
  'D_80011884 = 0x80011884;\n'
  'D_80011898 = 0x80011898;\n'
  'D_800118A4 = 0x800118a4;\n'
  'D_800118B8 = 0x800118b8;\n'
  'D_800118BC = 0x800118bc;\n'
  'D_800118C8 = 0x800118c8;\n'
  'D_800118D4 = 0x800118d4;\n'
  'D_800118E0 = 0x800118e0;\n'
  'D_800118EC = 0x800118ec;\n'
  'D_80095704 = 0x80095704;\n'
  'D_80095744 = 0x80095744;\n'
  'D_80095748 = 0x80095748;\n'
  'D_8009574C = 0x8009574c;\n'
  'D_8009574E = 0x8009574e;\n'
  'D_800957CC = 0x800957cc;\n'
  'D_800957D8 = 0x800957d8;\n'
  'D_800957EC = 0x800957ec;\n'
  'DMACallback = 0x80073cf4;\n'
  'g_GpuCallbacks = 0x80095744;\n'
  'g_GpuDebugPrintf = 0x80095748;\n'
  'g_GraphDebug = 0x8009574e;\n'
  'GPU_cw = 0x80077a54;\n'
  'Gpu_InitDmaQueue = 0x80077144;\n'
  'GPU_memset = 0x80077a28;\n'
  'printf = 0x80071a74;\n'
  'ResetCallback = 0x80073c94;\n'
  'SECTIONS { .text 0x80074a44 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }'),
 ('src/main/psyq/libgpu/Gpu_DmaVramTransfer.c',
  560,
  '1b9e23aebf87e63e62c9af90b41966ac61f63989f277c85096dd12dd16fb6215',
  'D_80095750 = 0x80095750;\n'
  'D_80095752 = 0x80095752;\n'
  'D_800A3300 = 0x800a3300;\n'
  'D_800A3328 = 0x800a3328;\n'
  'D_80095854 = 0x80095854;\n'
  '_param = 0x80076be0;\n'
  'Gpu_StartDmaTransfer = 0x80076b98;\n'
  'SECTIONS { .text 0x80076434 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }'),
 ('src/main/psyq/libgpu/vram_transfer.c',
  1404,
  'b069b0298391dc7421f028439867be07d930888c34b377a970b9b6345998c25f',
  'D_80095750 = 0x80095750;\n'
  'D_80095752 = 0x80095752;\n'
  'D_80095850 = 0x80095850;\n'
  'D_80095854 = 0x80095854;\n'
  'D_800A3348 = 0x800a3348;\n'
  'g_GpuControlRegMirror = 0x800a3348;\n'
  'g_GpuDmaBcrPtr = 0x8009585c;\n'
  'g_GpuDmaChcrPtr = 0x80095860;\n'
  'g_GpuDmaMadrPtr = 0x80095858;\n'
  'g_GpuGp0Ptr = 0x80095850;\n'
  'g_GpuGp1Ptr = 0x80095854;\n'
  'Gpu_DmaTimeoutCheck = 0x80077404;\n'
  'Gpu_ResetDmaWaitTimer = 0x800773d0;\n'
  'SECTIONS { .text 0x80076664 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }')]

# GNU as 2.8.1 preserves the SDK checked-division expansion without CPU ASM.
CASES.append(('src/main/psyq/libgte/fog_02.c',
 260,
 'd889825ceeab879eec23a3ab2bd7a3c2f01de7a49cd6aff48e9ce203cfbca58f',
 'SetDQA = 0x80078fac;\n'
 'SetDQB = 0x80078fb8;\n'
 'SECTIONS { .text 0x80077e64 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
 '*(.mdebug) } }'))


class PsyqSymbolicAddressingTests(unittest.TestCase):
    @unittest.skipUnless(shutil.which("mipsel-none-elf-ld"), "MIPS binutils unavailable")
    def test_all_linked_bytes_match_retail(self):
        for source, size, digest, script in CASES:
            with self.subTest(source=source), tempfile.TemporaryDirectory() as directory:
                work = pathlib.Path(directory)
                obj, elf, data = work / "code.o", work / "code.elf", work / "code.bin"
                subprocess.run(["tools/scripts/cc.sh", source, str(obj)],
                               cwd=ROOT, check=True, capture_output=True)
                linker_script = work / "code.ld"
                linker_script.write_text(script)
                subprocess.run(["mipsel-none-elf-ld", "-T", str(linker_script),
                                str(obj), "-o", str(elf)], check=True)
                subprocess.run(["mipsel-none-elf-objcopy", "-O", "binary",
                                "--only-section=.text", str(elf), str(data)], check=True)
                code = data.read_bytes()
                self.assertEqual(len(code), size)
                self.assertEqual(hashlib.sha256(code).hexdigest(), digest)
