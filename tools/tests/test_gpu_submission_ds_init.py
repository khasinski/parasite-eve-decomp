"""Exact linked-byte regressions for the GPU queue submission and DS read initialization."""
import hashlib
import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]

# Entire linked text ranges at their retail addresses.
CASES = [('psyq/libgpu/dma_queue',
  1948,
  '5839825b129edc7ba194d09ceaca9b4fa944307bc502c4438fa366c907a4c73c',
  'D_8009574C = 0x8009574c;\n'
  'D_80095854 = 0x80095854;\n'
  'D_80095860 = 0x80095860;\n'
  'D_80095874 = 0x80095874;\n'
  'D_80095878 = 0x80095878;\n'
  'D_8009587C = 0x8009587c;\n'
  'D_80095880 = 0x80095880;\n'
  'D_80095884 = 0x80095884;\n'
  'D_800A3348 = 0x800a3348;\n'
  'D_800BD030 = 0x800bd030;\n'
  'DMACallback = 0x80073cf4;\n'
  'g_GpuDmaChcrPtr = 0x80095860;\n'
  'g_GpuDmaControlRegPtr = 0x80095870;\n'
  'g_GpuDmaQueueHead = 0x80095874;\n'
  'g_GpuDmaQueueTail = 0x80095878;\n'
  'g_GpuGp1Ptr = 0x80095854;\n'
  'Gpu_DmaTimeoutCheck = 0x80077404;\n'
  'GPU_memset = 0x80077a28;\n'
  'Gpu_QueryStatus = 0x80077548;\n'
  'Gpu_ResetDmaWaitTimer = 0x800773d0;\n'
  'SetIntrMask = 0x80073e10;\n'
  'SECTIONS { .text 0x80076c34 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }'),
 ('psyq/libds/DsInit',
  592,
  '71cb6c36483078eb7a6513f2ac8d01d326f1a29003be05a55e261046ffa14825',
  'DS_system_active = 0x80080940;\n'
  'D_800B8AB0 = 0x800B8AB0;\n'
  'D_800A3510 = 0x800A3510;\n'
  'D_800A3515 = 0x800A3515;\n'
  'D_800A3525 = 0x800A3525;\n'
  'D_800A3535 = 0x800A3535;\n'
  'g_CdDsReadQueue = 0x800A3540;\n'
  'CQ_clear_queue = 0x8007E594;\n'
  'D_800A3604 = 0x800A3604;\n'
  'D_800A3600 = 0x800A3600;\n'
  'g_CdPendingReadCount = 0x800A3608;\n'
  'D_800A3610 = 0x800A3610;\n'
  'D_800A3690 = 0x800A3690;\n'
  'DS_init = 0x8007F994;\n'
  'CQ_sync_system = 0x8007E964;\n'
  'DS_sync_callback = 0x8007FBCC;\n'
  'CQ_ready_system = 0x8007F88C;\n'
  'DS_ready_callback = 0x8007FBD8;\n'
  'LIBDS_DSSYS_2_text_13CC = 0x8007F960;\n'
  'DS_start_callback = 0x8007FBE4;\n'
  'CQ_vsync_system = 0x8007F7E8;\n'
  'DS_vsync_callback = 0x8007FBC0;\n'
  'ER_clear = 0x800822BC;\n'
  'DsReadySystemMode = 0x80081E5C;\n'
  'DS_stop = 0x800808BC;\n'
  'DS_restart = 0x80080930;\n'
  'SECTIONS { .text 0x8007ec14 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }')]


class GpuSubmissionDsInitTests(unittest.TestCase):
    @unittest.skipUnless(shutil.which("mipsel-none-elf-ld"), "MIPS binutils unavailable")
    def test_all_linked_bytes_match_retail(self):
        for name, size, digest, script in CASES:
            with self.subTest(function=name), tempfile.TemporaryDirectory() as directory:
                work = pathlib.Path(directory)
                obj, elf, data = work / "code.o", work / "code.elf", work / "code.bin"
                subprocess.run(["tools/scripts/cc.sh", f"src/main/{name}.c", str(obj)],
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
