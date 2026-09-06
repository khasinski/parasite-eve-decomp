import pathlib
import shutil
import subprocess
import tempfile
import unittest


ROOT = pathlib.Path(__file__).resolve().parents[2]


class GpuOtcTests(unittest.TestCase):
    @unittest.skipUnless(shutil.which("cc"), "host C compiler unavailable")
    def test_setup_completion_and_timeout(self):
        source = (ROOT / "src/main/gpu/Gpu_SendOtChain.c").read_text()
        self.assertEqual(source.count(' asm("$3")'), 1)
        # Keep the empty barriers; remove only the MIPS register name.
        source = source.replace(' asm("$3")', "")
        harness = source + r'''
#include <assert.h>
#include <stdint.h>
static volatile unsigned int priority, control, address, length;
volatile unsigned int *D_80095870 = &priority;
volatile unsigned int *D_8009586C = &control;
volatile unsigned int *D_80095864 = &address;
volatile unsigned int *D_80095868 = &length;
static unsigned int table[64];
static int expectedCount, resets, polls, completeAt, timeoutAt;
void Gpu_ResetDmaWaitTimer(void) {
    ++resets;
    assert(priority == (0x12345678U | 0x08000000U));
    assert(control == 0x11000002);
    assert(length == (unsigned int)expectedCount);
    assert(address == (unsigned int)(uintptr_t)&table[expectedCount - 1]);
    if (completeAt == 0) control &= ~0x01000000U;
}
int Gpu_DmaTimeoutCheck(void) {
    assert(resets == 1);
    assert(control & 0x01000000);
    assert(++polls <= 10);
    if (polls == timeoutAt) return 1;
    if (polls == completeAt) control &= ~0x01000000U;
    return 0;
}
static void check(int count, int completion, int timeout) {
    int i;
    expectedCount = count;
    resets = polls = 0;
    completeAt = completion;
    timeoutAt = timeout;
    priority = 0x12345678;
    control = address = length = 0xFFFFFFFF;
    for (i = 0; i < 64; ++i) table[i] = 0xAABBCC00U + i;
    assert(Gpu_SendOtChain(table, count) == (timeout ? -1 : count));
    assert(resets == 1);
    assert(polls == (timeout ? timeout : completion));
    assert(!!(control & 0x01000000) == !!timeout);
    /* Hardware linking is not simulated; C itself must not touch the table. */
    for (i = 0; i < 64; ++i) assert(table[i] == 0xAABBCC00U + i);
}
int main(void) {
    check(1, 0, 0);
    check(8, 1, 0);
    check(64, 7, 0);
    check(8, 9, 3);
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            exe = pathlib.Path(directory) / "otc-test"
            result = subprocess.run(
                ["cc", "-std=gnu11", "-O2", "-Wno-pointer-to-int-cast",
                 "-x", "c", "-", "-o", str(exe)],
                input=harness, text=True, capture_output=True,
            )
            self.assertEqual(result.returncode, 0, result.stderr)
            subprocess.run([str(exe)], check=True, timeout=10)


if __name__ == "__main__":
    unittest.main()
