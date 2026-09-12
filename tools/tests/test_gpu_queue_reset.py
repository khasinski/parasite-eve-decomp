import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]


class GpuQueueResetTests(unittest.TestCase):
    @unittest.skipUnless(shutil.which("cc"), "host C compiler unavailable")
    def test_modes_masks_buffers_and_query(self):
        source = (ROOT / "src/main/gpu/dma_queue.c").read_text()
        source = source[:source.index("void Gpu_ResetDmaWaitTimer(void);")]
        harness = source + r'''
#include <assert.h>
#include <string.h>
static u32 gp1, chcr, control;
u32 *g_GpuGp1Ptr = &gp1, *g_GpuDmaChcrPtr = &chcr;
u32 *g_GpuDmaControlRegPtr = &control;
u32 g_GpuDmaQueueHead, D_80095884;
volatile u32 g_GpuDmaQueueTail;
unsigned char D_800A3348[256];
GpuQueueEntry D_800BD030[64];
static int masks, clears, queries, currentMode;
int SetIntrMask(int mask) {
    if (++masks == 1) assert(mask == 0);
    else {
        assert(masks == 2 && mask == 0x1357);
        assert(g_GpuDmaQueueHead == 0 && g_GpuDmaQueueTail == 0);
    }
    return 0x1357;
}
void GPU_memset(unsigned char *dst, int value, int count) {
    assert(masks == 1 && value == 0);
    assert(chcr == 0x401 && gp1 == 0 && control == 0x812);
    if (++clears == 1) assert(dst == D_800A3348 && count == 256);
    else assert(clears == 2 && dst == (unsigned char *)D_800BD030 && count == 6144);
    memset(dst, value, count);
}
int Gpu_QueryStatus(int mode) {
    assert(masks == 2 && mode == currentMode && (mode & 7) == 0);
    ++queries;
    return 73;
}
int main(void) {
    int mode, i;
    for (mode = -8; mode < 24; ++mode) {
        int kind = mode & 7;
        int full = kind == 0 || kind == 5;
        int soft = kind == 1 || kind == 3;
        currentMode = mode;
        masks = clears = queries = 0;
        gp1 = chcr = 0xDEADBEEF;
        control = 0x12;
        g_GpuDmaQueueHead = 17;
        g_GpuDmaQueueTail = 63;
        memset(D_800A3348, 0xA5, sizeof(D_800A3348));
        memset(D_800BD030, 0xA5, sizeof(D_800BD030));
        assert(Gpu_InitDmaQueue(mode) == (kind == 0 ? 73 : 0));
        assert(masks == 2 && clears == (full ? 2 : 0));
        assert(queries == (kind == 0) && D_80095884 == 0x1357);
        assert(chcr == (full || soft ? 0x401 : 0xDEADBEEF));
        assert(control == (full || soft ? 0x812 : 0x12));
        assert(gp1 == (full ? 0 : soft ? 0x01000000 : 0xDEADBEEF));
        for (i = 0; i < sizeof(D_800A3348); ++i)
            assert(D_800A3348[i] == (full ? 0 : 0xA5));
        for (i = 0; i < sizeof(D_800BD030); ++i)
            /* Host pointers may widen entries; the retail clear is exactly 6144 bytes. */
            assert(((unsigned char *)D_800BD030)[i] == (full && i < 6144 ? 0 : 0xA5));
    }
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            exe = pathlib.Path(directory) / "queue-reset-test"
            result = subprocess.run(
                ["cc", "-I", str(ROOT / "include"), "-std=gnu11", "-O2", "-x", "c", "-", "-o", str(exe)],
                input=harness, text=True, capture_output=True,
            )
            self.assertEqual(result.returncode, 0, result.stderr)
            subprocess.run([str(exe)], check=True, timeout=10)


if __name__ == "__main__":
    unittest.main()
