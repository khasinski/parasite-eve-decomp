import pathlib
import shutil
import subprocess
import tempfile
import unittest


ROOT = pathlib.Path(__file__).resolve().parents[2]


class GpuQueueTests(unittest.TestCase):
    @unittest.skipUnless(shutil.which("cc"), "host C compiler unavailable")
    def test_state_and_callback_order(self):
        source = (ROOT / "src/main/gpu/SetGraphQueue.c").read_text()
        self.assertEqual(source.count(' asm("$17")'), 1)
        # Only the MIPS register allocation annotation is removed for host tests.
        source = source.replace(' asm("$17")', "")
        harness = source + r'''
#include <assert.h>
GpuDebugState D_8009574C;
char D_80011840[] = "queue";
static int events, expectedOld, expectedNew, mutateOnPrint;
static int reset(int mode) {
    assert(mode == 1);
    assert(D_8009574C.queueState.queue == expectedOld);
    events = events * 10 + 2;
    return 0;
}
static void print(char *message, int argument) {
    assert(message == D_80011840);
    assert(argument == expectedNew);
    events = events * 10 + 1;
    if (mutateOnPrint) D_8009574C.queueState.queue = expectedNew;
}
static GpuCallbacks callbacks;
GpuCallbacks *D_80095744 = &callbacks;
void (*D_80095748)() = print;
void DMACallback(int channel, void *callback) {
    assert(channel == 2 && callback == 0);
    assert(D_8009574C.queueState.queue == (unsigned char)expectedNew);
    events = events * 10 + 3;
}
static void check(int old, int mode, int debug, int mutate, int expectedEvents) {
    D_8009574C.queueState.queue = expectedOld = old;
    D_8009574C.queueState.debugLevel = debug;
    expectedNew = mode;
    mutateOnPrint = mutate;
    events = 0;
    assert(SetGraphQueue(mode) == old);
    assert(events == expectedEvents);
    assert(D_8009574C.queueState.queue == (unsigned char)mode);
}
int main(void) {
    callbacks.reset = reset;
    check(0, 0, 0, 0, 0);
    check(0, 1, 0, 0, 23);
    check(1, 1, 2, 0, 1);
    check(1, 0, 2, 0, 123);
    check(255, 0, 1, 0, 23);
    check(0, 300, 0, 0, 23);
    check(0, 1, 2, 1, 1);
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            exe = pathlib.Path(directory) / "queue-test"
            result = subprocess.run(
                ["cc", "-std=gnu11", "-O2", "-I", str(ROOT / "include"),
                 "-x", "c", "-", "-o", str(exe)],
                input=harness, text=True, capture_output=True,
            )
            self.assertEqual(result.returncode, 0, result.stderr)
            subprocess.run([str(exe)], check=True, timeout=10)


if __name__ == "__main__":
    unittest.main()
