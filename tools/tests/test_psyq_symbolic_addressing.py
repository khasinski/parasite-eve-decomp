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

CASES = [('src/main/pad/SetInitPadFlag.c',
  12,
  'ca0784dda126a1b868badf58f4bd44bd84e74b24a56a401ce8b0a61727f7dfe6',
  'g_InitPadFlag = 0x8009B4AC;\n'
  'SECTIONS { .text 0x8007dea4 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }'),
 ('src/main/util/Timer_StartTimeout.c',
  32,
  '6c5f1e66aa6c297b2aba4b3f46bfe2c7dcc0a5388227831c9d8b0748b638a10b',
  'g_TimerTimeoutLimit = 0x800bd02c;\n'
  'D_800A76D0 = 0x800a76d0;\n'
  'SECTIONS { .text 0x80084fc4 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }'),
 ('src/main/gpu/libgpu_sys.c',
  708,
  '00f88a6906518b3da0c10f4b454ad4f7dff6d07a6751c1d7d9e6c9f103cadb64',
  'LoadImage = 0x8007506c;\n'
  'GetTPage = 0x80077a64;\n'
  'GetClut = 0x80077aa4;\n'
  'g_VideoMode = 0x800956ec;\n'
  'SECTIONS { .text 0x80074774 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }'),
 ('src/main/cdrom/cd_tail.c',
  536,
  '309628923767c77ba53c659fff5acbec885e9a9eaba554ae2020803b3eef364a',
  'CD_vol = 0x8007b964;\n'
  'CD_getsector2 = 0x8007bf44;\n'
  'CD_getsector = 0x8007c044;\n'
  'CD_datasync = 0x8007bddc;\n'
  'g_CdReadCallback = 0x8009afc0;\n'
  'SECTIONS { .text 0x80080ac4 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }'),
 ('src/main/event/Evt_Deliver.c',
  100,
  'b94427dfc0967b4aaeacb5750ae5b1e167af9b260ba712e561050f8c6d6ff179',
  'D_80094564 = 0x80094564;\n'
  'D_80094568 = 0x80094568;\n'
  'DeliverEvent = 0x80073a34;\n'
  'SECTIONS { .text 0x800739c4 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }'),
 ('src/main/memcard/MemCard_SetCallbackVtable.c',
  52,
  '9f58f31b9a795eeb97b583930590eef95892a1ba629c367c21ddde091ea87eb1',
  'func_80084B78 = 0x80084b78;\n'
  'g_MemCardStateDispatchFn = 0x8009b73c;\n'
  'CardObj_IsTransferActive = 0x80084f8c;\n'
  'g_MemCardIsTransferActiveFn = 0x8009b740;\n'
  'LIBPAD_PADSEQD_text_108 = 0x80084c4c;\n'
  'g_MemCardResponseHandler = 0x8009b744;\n'
  'SECTIONS { .text 0x80084b44 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }'),
 ('src/main/cdrom/misc9.c',
  64,
  '3e9c23ff8921bfb43accf28fb43951d8b2614ae49b3c09a313efd642eb50d3b3',
  'LIBDS_DSREADY_text_FC = 0x80081e70;\n'
  'DsSyncCallback = 0x800824c8;\n'
  'g_DsReadBusy = 0x8009b70c;\n'
  'SECTIONS { .text 0x8008227c : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }'),
 ('src/main/gpu/Gpu_WriteControlAndMirror.c',
  36,
  'b8abe6ded14177484d809d36a89c33d27b17363b810f25ffcfc4da6f8b41a272',
  'g_GpuGp1Ptr = 0x80095854;\n'
  'D_800A3348 = 0x800a3348;\n'
  'SECTIONS { .text 0x80076b20 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }'),
 ('src/main/cdrom/CdRom_ResetDsReadSystem.c',
  268,
  '3280fd8e2aaa2af9897f49d8430b7040c41e1b434af50a3b65500c0ccb196a32',
  'CdRom_AbortCmd = 0x800808bc;\n'
  'D_800B8AB0 = 0x800b8ab0;\n'
  'D_800A3510 = 0x800a3510;\n'
  'D_800A3515 = 0x800a3515;\n'
  'D_800A3525 = 0x800a3525;\n'
  'D_800A3535 = 0x800a3535;\n'
  'g_CdDsReadQueue = 0x800a3540;\n'
  'CQ_clear_queue = 0x8007e594;\n'
  'D_800A3604 = 0x800a3604;\n'
  'D_800A3600 = 0x800a3600;\n'
  'g_CdPendingReadCount = 0x800a3608;\n'
  'D_800A3610 = 0x800a3610;\n'
  'D_800A3690 = 0x800a3690;\n'
  'DS_read_cbready = 0x800822bc;\n'
  'DsReadCallback = 0x80081e5c;\n'
  'CdRom_EnableDsReadSystem = 0x80080930;\n'
  'SECTIONS { .text 0x8007ed58 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }'),
 ('src/main/main/Render_InitEntityPool.c',
  372,
  '255d8a0293b214dabba1d7fca566f2a9df422640a04b1930a82e8eb5d7319bd8',
  'D_800117E0 = 0x800117e0;\n'
  'D_80095704 = 0x80095704;\n'
  'D_8009574C = 0x8009574c;\n'
  'printf = 0x80071a74;\n'
  'GPU_memset = 0x80077a28;\n'
  'ResetCallback = 0x80073c94;\n'
  'D_80095744 = 0x80095744;\n'
  'GPU_cw = 0x80077a54;\n'
  'Gpu_InitDmaQueue = 0x80077144;\n'
  'D_800957CC = 0x800957cc;\n'
  'D_800957D8 = 0x800957d8;\n'
  'D_8009574E = 0x8009574e;\n'
  'D_80011800 = 0x80011800;\n'
  'D_80095748 = 0x80095748;\n'
  'SECTIONS { .text 0x80074a44 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }'),
 ('src/main/main/Gpu_DmaVramTransfer.c',
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
 ('src/main/psyq/libgpu/sys_display.c',
  1212,
  'b57688fa82c976a5d8550dd6868bd6e0b9f00bd7b64e08c9ce08abbe7fa2b823',
  'g_GraphDebug = 0x8009574e;\n'
  'D_80011870 = 0x80011870;\n'
  'g_GpuDebugPrintf = 0x80095748;\n'
  'GPU_memset = 0x80077a28;\n'
  'g_GpuCallbacks = 0x80095744;\n'
  'D_8009574C = 0x8009574c;\n'
  'D_80011884 = 0x80011884;\n'
  'D_80095748 = 0x80095748;\n'
  'D_80095744 = 0x80095744;\n'
  'D_80011898 = 0x80011898;\n'
  'D_800118B8 = 0x800118b8;\n'
  'D_800118A4 = 0x800118a4;\n'
  'D_800118BC = 0x800118bc;\n'
  'D_800118C8 = 0x800118c8;\n'
  'D_800118D4 = 0x800118d4;\n'
  'D_800118E0 = 0x800118e0;\n'
  'D_800118EC = 0x800118ec;\n'
  'D_800957EC = 0x800957ec;\n'
  'SECTIONS { .text 0x80074d28 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }')]

# The same GNU addressing also removes the CPU-ASM window in gpu2.c.
CASES.append(('src/main/gpu/gpu2.c',
 84,
 'c8e3bc6a8ef0c53b908507260b670e6f2cc196a51b8cd4bd155ed16c7d498c57',
 'g_GpuControlRegMirror = 0x800a3348;\n'
 'g_GpuGp1Ptr = 0x80095854;\n'
 'g_GpuGp0Ptr = 0x80095850;\n'
 'SECTIONS { .text 0x80076b44 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
 '*(.mdebug) } }'))

# GNU as 2.8.1 preserves the SDK checked-division expansion without CPU ASM.
CASES.append(('src/main/gte/Gte_SetDepthParams.c',
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
