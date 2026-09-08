import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]


class GpuQueueDrainTests(unittest.TestCase):
    @unittest.skipUnless(shutil.which("cc"), "host C compiler unavailable")
    def test_polling_waiting_wraparound_and_timeouts(self):
        source = (ROOT / "src/main/gpu/dma_queue.c").read_text()
        source = ("extern unsigned int D_80095874;\n"
                  "extern volatile unsigned int D_80095878;\n"
                  "extern unsigned int *D_80095860, *D_80095854;\n"
                  "void Gpu_ResetDmaWaitTimer(void);\n"
                  "int Gpu_SetDisplayBuffer(void);\n"
                  "int Gpu_DmaTimeoutCheck(void);\n" +
                  source[source.index("static __inline__ unsigned int readGpuStatus"):])
        harness = source + r'''
#include <assert.h>
static unsigned int dma, gpu;
unsigned int *D_80095860 = &dma, *D_80095854 = &gpu;
unsigned int D_80095874;
volatile unsigned int D_80095878;
static int resets, dispatches, polls, timeoutAt, releaseAt, drain;
void Gpu_ResetDmaWaitTimer(void) { ++resets; }
int Gpu_SetDisplayBuffer(void) {
    ++dispatches;
    if (drain) D_80095878 = D_80095874;
    return 0;
}
int Gpu_DmaTimeoutCheck(void) {
    assert(++polls < 20);
    if (polls == timeoutAt) return -1;
    if (polls == releaseAt) { dma = 0; gpu = 0x04000000; }
    return 0;
}
static void setup(int count, int busy, int ready) {
    D_80095878 = 61;
    D_80095874 = (61 + count) & 63;
    dma = busy ? 0x01000000 : 0;
    gpu = ready ? 0x04000000 : 0;
    resets = dispatches = polls = timeoutAt = releaseAt = 0;
    drain = 1;
}
int main(void) {
    int count, busy, ready, mode;
    for (mode = -1; mode <= 2; ++mode) {
        if (!mode) continue;
        for (count = 0; count <= 63; ++count)
            for (busy = 0; busy <= 1; ++busy)
                for (ready = 0; ready <= 1; ++ready) {
                    setup(count, busy, ready);
                    assert(Gpu_DrainDmaQueue(mode) ==
                           (count ? count : busy || !ready ? 1 : 0));
                    assert(dispatches == !!count && polls == 0 && resets == 0);
                }
    }
    setup(0, 0, 1);
    assert(Gpu_DrainDmaQueue(0) == 0 && resets == 1 && polls == 0);
    setup(5, 0, 1);
    assert(Gpu_DrainDmaQueue(0) == 0 && dispatches == 1 && polls == 1);
    setup(5, 0, 1);
    drain = 0; timeoutAt = 3;
    assert(Gpu_DrainDmaQueue(0) == -1 && dispatches == 3 && polls == 3);
    for (busy = 0; busy <= 1; ++busy) {
        setup(0, busy, 0); releaseAt = 3;
        assert(Gpu_DrainDmaQueue(0) == 0 && polls == 3 && dispatches == 0);
        setup(0, busy, 0); timeoutAt = 2;
        assert(Gpu_DrainDmaQueue(0) == -1 && polls == 2);
    }
    setup(0, 1, 1); releaseAt = 2;
    assert(Gpu_DrainDmaQueue(0) == 0 && polls == 2);
    setup(0, 1, 1); timeoutAt = 2;
    assert(Gpu_DrainDmaQueue(0) == -1 && polls == 2);
    setup(5, 1, 0); releaseAt = 3;
    assert(Gpu_DrainDmaQueue(0) == 0 && dispatches == 1 && polls == 3);
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            exe = pathlib.Path(directory) / "queue-drain-test"
            result = subprocess.run(
                ["cc", "-std=gnu11", "-O2", "-x", "c", "-", "-o", str(exe)],
                input=harness, text=True, capture_output=True,
            )
            self.assertEqual(result.returncode, 0, result.stderr)
            subprocess.run([str(exe)], check=True, timeout=10)


if __name__ == "__main__":
    unittest.main()
