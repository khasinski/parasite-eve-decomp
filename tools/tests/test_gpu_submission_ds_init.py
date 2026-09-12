"""Exact linked-byte regressions for the GPU queue submission and DS read initialization."""
import hashlib
import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]

# Entire linked text ranges at their retail addresses.
CASES = [('main/Gpu_SwapDisplayBuffers',
  688,
  'b3686b34851b08fa3bb0097263caf59519056417b593606b7e7a59c155b1e508',
  'Gpu_ResetDmaWaitTimer = 0x800773D0;\n'
  'Gpu_DmaTimeoutCheck = 0x80077404;\n'
  'Gpu_SetDisplayBuffer = 0x80076EE4;\n'
  'D_80095874 = 0x80095874;\n'
  'D_80095878 = 0x80095878;\n'
  'SetIntrMask = 0x80073E10;\n'
  'D_8009574C = 0x8009574C;\n'
  'D_8009587C = 0x8009587C;\n'
  'D_80095860 = 0x80095860;\n'
  'D_80095854 = 0x80095854;\n'
  'DMACallback = 0x80073CF4;\n'
  'D_800BD030 = 0x800BD030;\n'
  'SECTIONS { .text 0x80076c34 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) '
  '*(.mdebug) } }'),
 ('cdrom/CdRom_InitDsReadSystem',
  324,
  '26f2de2fd362c72fda26ab8368fb82d03a4456e898d42a65d0c43764b608b7f1',
  'CdRom_IsDsReadSystemEnabled = 0x80080940;\n'
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
  'CdRom_InitDsCallbacks = 0x8007F994;\n'
  'Render_StepParticleNode = 0x8007E964;\n'
  'CdRom_SetSyncCallback = 0x8007FBCC;\n'
  'Render_PlayParticleEffect = 0x8007F88C;\n'
  'CdRom_SetReadyCallback = 0x8007FBD8;\n'
  'CdRom_DispatchDsReadyCallback = 0x8007F960;\n'
  'CdRom_SetDispatchCallback = 0x8007FBE4;\n'
  'CdRom_PollPendingDsRead = 0x8007F7E8;\n'
  'CdRom_SetPollCallback = 0x8007FBC0;\n'
  'DS_read_cbready = 0x800822BC;\n'
  'DsReadCallback = 0x80081E5C;\n'
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
