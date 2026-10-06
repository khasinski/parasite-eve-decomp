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
  'g_InitPadFlag = 0x8009B4AC;\nSECTIONS { .text 0x8007dea4 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) *(.mdebug) } }'),
 ('src/main/psyq/libpad/waitrc2.c',
  192,
  '1795d848e518e058eb629903c8f624ed60f2d7b4315c553f78207b376a3a9b92',
  'g_TimerTimeoutLimit = 0x800BD02C;\ng_TimerTimeoutStart = 0x800A76D0;\nSECTIONS { .text 0x80084fc4 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) *(.mdebug) } }'),
 ('src/main/psyq/libgpu/ext.c',
  672,
  '3a25ec7c5052da04717d07bcd8e6a892675b1e1652248db012e7694c95229a75',
  'LoadImage = 0x8007506c;\nGetTPage = 0x80077a64;\nGetClut = 0x80077aa4;\nGetVideoMode = 0x80074a28;\nSECTIONS { .text 0x80074774 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) *(.mdebug) } }'),
 ('src/main/psyq/libetc/vmode.c',
  36,
  'f59613af52070699e2282716b260db31b6dfda3e8b3f91918e52f1e56017d853',
  'g_VideoMode = 0x800956ec;\nSECTIONS { .text 0x80074a14 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) *(.mdebug) } }'),
 ('src/main/psyq/libds/dssys_3.c',
  620,
  '59abf3e84761312d06b02635cf0a85b60cd7ec0f1b6902273a1f47d998788c25',
  'CD_datasync = 0x8007BDDC;\nCD_debug = 0x8009AFC0;\nCD_getsector = 0x8007BF44;\nCD_getsector2 = 0x8007C044;\nCD_vol = 0x8007B964;\nDS_lastpos = 0x8007FC28;\nSECTIONS { .text 0x80080ac4 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) *(.mdebug) } }'),
 ('src/main/psyq/libmath/ferr.c',
  100,
  'b94427dfc0967b4aaeacb5750ae5b1e167af9b260ba712e561050f8c6d6ff179',
  'D_80094564 = 0x80094564;\nD_80094568 = 0x80094568;\nDeliverEvent = 0x80073a34;\nSECTIONS { .text 0x800739c4 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) *(.mdebug) } }'),
 ('src/main/psyq/libpad/padseqd.c',
  1140,
  'e080b43d7fdd01895057432b2eb8509d80c259ecb79f25aa77b5711f3411b504',
  'D_8009B728 = 0x8009B728;\n_padCmdParaMode = 0x80083E50;\n_padRecvAtLoadInfo = 0x80083644;\n_padSendAtLoadInfo = 0x800835C0;\ng_MemCardIsTransferActiveFn = 0x8009B740;\ng_MemCardResponseHandler = 0x8009B744;\ng_MemCardStateDispatchFn = 0x8009B73C;\nSECTIONS { .text 0x80084b44 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) *(.mdebug) } }'),
 ('src/main/psyq/libds/dsready_5.c',
  144,
  '62ba3a0ceb4b5644e6eedec6393f9c9e3d0e5bb35d16f0d21e2c42eaf3d6cad0',
  'DsReadyCallback = 0x800824C8;\nDsStartCallback = 0x800824DC;\nER_cbready = 0x80081E70;\ng_DsReadBusy = 0x8009B70C;\nSECTIONS { .text 0x8008227c : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) *(.mdebug) } }'),
 ('src/main/psyq/libgpu/sys.c',
  1952,
  'e659ffcf3ce6e4cb3fe25b7805924d60dd70ca6e77145b75856b26798698a36d',
  'D_800117E0 = 0x800117e0;\nD_80011800 = 0x80011800;\nD_80011814 = 0x80011814;\nD_80011840 = 0x80011840;\nD_80011854 = 0x80011854;\nD_80011870 = 0x80011870;\nD_80011884 = 0x80011884;\nD_80011898 = 0x80011898;\nD_800118A4 = 0x800118a4;\nD_800118B8 = 0x800118b8;\nD_800118BC = 0x800118bc;\nD_800118C8 = 0x800118c8;\nD_800118D4 = 0x800118d4;\nD_800118E0 = 0x800118e0;\nD_800118EC = 0x800118ec;\nD_80095704 = 0x80095704;\nD_80095744 = 0x80095744;\nD_80095748 = 0x80095748;\nD_8009574C = 0x8009574c;\nD_8009574E = 0x8009574e;\nD_800957CC = 0x800957cc;\nD_800957D8 = 0x800957d8;\nD_800957EC = 0x800957ec;\nDMACallback = 0x80073cf4;\ng_GpuCallbacks = 0x80095744;\ng_GpuDebugPrintf = 0x80095748;\ng_GraphDebug = 0x8009574e;\nGPU_cw = 0x80077a54;\nGpu_InitDmaQueue = 0x80077144;\nGPU_memset = 0x80077a28;\nprintf = 0x80071a74;\nResetCallback = 0x80073c94;\nSECTIONS { .text 0x80074a44 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) *(.mdebug) } }'),
 ('src/main/psyq/libgpu/Gpu_DmaVramTransfer.c',
  560,
  '1b9e23aebf87e63e62c9af90b41966ac61f63989f277c85096dd12dd16fb6215',
  'D_80095750 = 0x80095750;\nD_80095752 = 0x80095752;\nD_800A3300 = 0x800a3300;\nD_800A3328 = 0x800a3328;\nD_80095854 = 0x80095854;\n_param = 0x80076be0;\nGpu_StartDmaTransfer = 0x80076b98;\nSECTIONS { .text 0x80076434 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) *(.mdebug) } }'),
 ('src/main/psyq/libgpu/vram_transfer.c',
  1404,
  'b069b0298391dc7421f028439867be07d930888c34b377a970b9b6345998c25f',
  'D_80095750 = 0x80095750;\nD_80095752 = 0x80095752;\nD_80095850 = 0x80095850;\nD_80095854 = 0x80095854;\nD_800A3348 = 0x800a3348;\ng_GpuControlRegMirror = 0x800a3348;\ng_GpuDmaBcrPtr = 0x8009585c;\ng_GpuDmaChcrPtr = 0x80095860;\ng_GpuDmaMadrPtr = 0x80095858;\ng_GpuGp0Ptr = 0x80095850;\ng_GpuGp1Ptr = 0x80095854;\nGpu_DmaTimeoutCheck = 0x80077404;\nGpu_ResetDmaWaitTimer = 0x800773d0;\nSECTIONS { .text 0x80076664 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) *(.mdebug) } }')]

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
