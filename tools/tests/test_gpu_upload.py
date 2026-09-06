import pathlib
import shutil
import subprocess
import tempfile
import unittest


ROOT = pathlib.Path(__file__).resolve().parents[2]


class GpuUploadTests(unittest.TestCase):
    @unittest.skipUnless(shutil.which("cc"), "host C compiler unavailable")
    def test_clamps_cpu_prefix_dma_blocks_and_timeout(self):
        source = (ROOT / "src/main/gpu/Gpu_WaitDmaComplete.c").read_text()
        for annotation in (' asm("$21")', ' asm("$3")'):
            self.assertEqual(source.count(annotation), 1)
            source = source.replace(annotation, "")
        harness = source + r'''
#include <assert.h>
#include <stdint.h>
short D_80095750 = 1024, D_80095752 = 512;
static unsigned int gp0, gp1, madr, bcr, chcr;
unsigned int *D_80095850 = &gp0;
unsigned int *D_80095854 = &gp1;
unsigned int *D_80095858 = &madr;
unsigned int *D_8009585C = &bcr;
unsigned int *D_80095860 = &chcr;
static unsigned int pixels[262144];
static int resets, polls, readyAt, timeoutAt;
void Gpu_ResetDmaWaitTimer(void) { ++resets; }
int Gpu_DmaTimeoutCheck(void) {
    assert(resets == 1);
    assert(++polls < 10);
    assert(gp0 == 0xDEADBEEF && chcr == 0xDEADBEEF);
    if (polls == timeoutAt) return -1;
    if (polls == readyAt) gp1 = 0x04000000;
    return 0;
}
static void check(int w, int h, int expectedW, int expectedH,
                  int ready, int timeout) {
    RECT rect = {-7, 19, w, h};
    int words = (expectedW * expectedH + 1) / 2;
    int prefix = words % 16;
    int blocks = words / 16;
    int fail = words <= 0 || timeout;
    unsigned int expectedGp0;
    readyAt = ready;
    timeoutAt = timeout;
    resets = polls = 0;
    gp0 = madr = bcr = chcr = 0xDEADBEEF;
    gp1 = ready ? 0 : 0x04000000;
    assert(Gpu_WaitDmaComplete(&rect, pixels) == (fail ? -1 : 0));
    assert(resets == 1 && rect.x == -7 && rect.y == 19);
    assert(rect.w == expectedW && rect.h == expectedH);
    assert(polls == (words <= 0 ? 0 : timeout ? timeout : ready));
    if (fail) {
        assert(gp0 == 0xDEADBEEF && madr == 0xDEADBEEF);
        assert(bcr == 0xDEADBEEF && chcr == 0xDEADBEEF);
        return;
    }
    /* This register mock observes final state, not the full GP0 write stream. */
    expectedGp0 = prefix ? pixels[prefix - 1] :
                  (unsigned int)expectedW | ((unsigned int)expectedH << 16);
    assert(gp0 == expectedGp0);
    if (blocks) {
        assert(gp1 == 0x04000002);
        assert(madr == (unsigned int)(uintptr_t)(pixels + prefix));
        assert(bcr == ((unsigned int)blocks << 16 | 16));
        assert(chcr == 0x01000201);
    } else {
        assert(gp1 == 0x04000000);
        assert(madr == 0xDEADBEEF && bcr == 0xDEADBEEF && chcr == 0xDEADBEEF);
    }
}
int main(void) {
    int i;
    for (i = 0; i < 262144; ++i) pixels[i] = 0x12340000U + i;
    check(0, 4, 0, 4, 0, 0);
    check(-1, 4, 0, 4, 0, 0);
    check(8, -3, 8, 0, 0, 0);
    check(1, 1, 1, 1, 0, 0);
    check(3, 3, 3, 3, 2, 0);
    check(30, 1, 30, 1, 0, 0);
    check(32, 1, 32, 1, 0, 0);
    check(34, 1, 34, 1, 3, 0);
    check(31, 3, 31, 3, 0, 0);
    check(2000, 1000, 1024, 512, 0, 0);
    check(34, 1, 34, 1, 8, 2);
    for (i = 0; i < 262144; ++i) assert(pixels[i] == 0x12340000U + i);
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            exe = pathlib.Path(directory) / "upload-test"
            result = subprocess.run(
                ["cc", "-std=gnu11", "-O2", "-fno-strict-aliasing",
                 "-Wno-pointer-to-int-cast", "-I", str(ROOT / "include"),
                 "-x", "c", "-", "-o", str(exe)],
                input=harness, text=True, capture_output=True,
            )
            self.assertEqual(result.returncode, 0, result.stderr)
            subprocess.run([str(exe)], check=True, timeout=10)


if __name__ == "__main__":
    unittest.main()
