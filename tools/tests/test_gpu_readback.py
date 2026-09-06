import pathlib
import shutil
import subprocess
import tempfile
import unittest


ROOT = pathlib.Path(__file__).resolve().parents[2]


class GpuReadbackTests(unittest.TestCase):
    @unittest.skipUnless(shutil.which("cc"), "host C compiler unavailable")
    def test_two_waits_cpu_prefix_dma_blocks_and_timeouts(self):
        source = (ROOT / "src/main/gpu/Gpu_FlushDisplayBuffer.c").read_text()
        self.assertEqual(source.count(' asm("$2")'), 1)
        source = source.replace(' asm("$2")', "")
        harness = source + r'''
#include <assert.h>
#include <stdint.h>
#define SENTINEL 0xDEADBEEFU
#define SAMPLE 0x56781234U
short D_80095750 = 1024, D_80095752 = 512;
static unsigned int gp0, gp1, madr, bcr, chcr;
unsigned int *D_80095850 = &gp0;
unsigned int *D_80095854 = &gp1;
unsigned int *D_80095858 = &madr;
unsigned int *D_8009585C = &bcr;
unsigned int *D_80095860 = &chcr;
static unsigned int pixels[262144], expectedDimensions;
static int resets, commandPolls, dataPolls, commandReady, dataReady;
static int commandTimeout, dataTimeout;
void Gpu_ResetDmaWaitTimer(void) { ++resets; }
int Gpu_DmaTimeoutCheck(void) {
    assert(resets == 1);
    assert(madr == SENTINEL && bcr == SENTINEL && chcr == SENTINEL);
    if (gp0 == SENTINEL) {
        assert(++commandPolls < 10);
        assert(gp1 == 0);
        if (commandPolls == commandTimeout) return -1;
        if (commandPolls == commandReady) gp1 = 0x04000000;
    } else {
        assert(gp0 == expectedDimensions && gp1 == 0x04000000);
        assert(++dataPolls < 10);
        if (dataPolls == dataTimeout) return -1;
        if (dataPolls == dataReady) {
            gp1 = 0x08000000;
            gp0 = SAMPLE;
        }
    }
    return 0;
}
static void check(int w, int h, int expectedW, int expectedH,
                  int cmdReady, int readReady, int cmdTimeout, int readTimeout) {
    RECT rect = {-7, 19, w, h};
    int words = (expectedW * expectedH + 1) / 2;
    int prefix = words % 16, blocks = words / 16;
    int failed = words <= 0 || cmdTimeout || readTimeout;
    int i;
    expectedDimensions = expectedW | ((unsigned int)expectedH << 16);
    commandReady = cmdReady;
    dataReady = readReady;
    commandTimeout = cmdTimeout;
    dataTimeout = readTimeout;
    resets = commandPolls = dataPolls = 0;
    gp0 = madr = bcr = chcr = SENTINEL;
    gp1 = cmdReady ? 0 : 0x04000000;
    for (i = 0; i < 262144; ++i) pixels[i] = SENTINEL;
    assert(Gpu_FlushDisplayBuffer(&rect, pixels) == (failed ? -1 : 0));
    assert(resets == 1 && rect.x == -7 && rect.y == 19);
    assert(rect.w == expectedW && rect.h == expectedH);
    assert(commandPolls == (words <= 0 ? 0 : cmdTimeout ? cmdTimeout : cmdReady));
    assert(dataPolls == (words <= 0 || cmdTimeout ? 0 :
                         readTimeout ? readTimeout : readReady));
    /* Only CPU reads execute in this mock; it does not emulate DMA or GP0 streaming. */
    for (i = 0; i < 262144; ++i) {
        assert(pixels[i] == (!failed && i < prefix ? SAMPLE : SENTINEL));
    }
    if (failed) {
        assert(madr == SENTINEL && bcr == SENTINEL && chcr == SENTINEL);
        assert(gp0 == (words <= 0 || cmdTimeout ? SENTINEL : expectedDimensions));
    } else if (blocks) {
        assert(gp1 == 0x04000003);
        assert(madr == (unsigned int)(uintptr_t)(pixels + prefix));
        assert(bcr == ((unsigned int)blocks << 16 | 16));
        assert(chcr == 0x01000200);
    } else {
        assert(gp1 == 0x08000000);
        assert(madr == SENTINEL && bcr == SENTINEL && chcr == SENTINEL);
    }
}
int main(void) {
    check(0, 4, 0, 4, 0, 1, 0, 0);
    check(-1, 4, 0, 4, 0, 1, 0, 0);
    check(8, -3, 8, 0, 0, 1, 0, 0);
    check(1, 1, 1, 1, 0, 1, 0, 0);
    check(3, 3, 3, 3, 2, 3, 0, 0);
    check(30, 1, 30, 1, 0, 1, 0, 0);
    check(32, 1, 32, 1, 0, 1, 0, 0);
    check(34, 1, 34, 1, 3, 2, 0, 0);
    check(31, 3, 31, 3, 0, 1, 0, 0);
    check(2000, 1000, 1024, 512, 0, 1, 0, 0);
    check(34, 1, 34, 1, 8, 1, 2, 0);
    check(34, 1, 34, 1, 0, 8, 0, 2);
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            exe = pathlib.Path(directory) / "readback-test"
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
