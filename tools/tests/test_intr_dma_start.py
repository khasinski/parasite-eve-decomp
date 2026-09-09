import pathlib
import shutil
import subprocess
import tempfile
import unittest


ROOT = pathlib.Path(__file__).resolve().parents[2]


class IntrDmaStartTests(unittest.TestCase):
    @unittest.skipUnless(shutil.which("cc"), "host C compiler unavailable")
    def test_clears_handlers_installs_dma_trap_and_returns_registrar(self):
        source = (ROOT / "src/main/psyq/libapi/startIntrDMA.c").read_text()
        harness = source + r'''
#include <assert.h>
void handler(void) {}
void (*g_IntrDmaHandlerTable[8])(void);
static unsigned int dispatch;
unsigned int *g_IntrDmaDispatchPtr = &dispatch;
static int clear_calls, event, callback;
void memclrIntrDMA(int *ptr, int count) {
    int i;
    void (**handlers)(void) = (void (**)(void))ptr;
    assert(ptr == (int *)g_IntrDmaHandlerTable && count == 8);
    ++clear_calls;
    for (i = 0; i < count; ++i) handlers[i] = 0;
}
static void expected_trap(void) {}
void trapIntrDMA(void) { expected_trap(); }
PsyqInterruptHandler setIntrDMA(int channel, PsyqInterruptHandler handler) {
    (void)channel;
    return handler;
}
PsyqInterruptHandler InterruptCallback(int irq, PsyqInterruptHandler handler) {
    assert(irq == 3 && handler == trapIntrDMA);
    event = irq;
    callback = 1;
    return 0;
}
int main(void) {
    int i;
    for (i = 0; i < 8; ++i) g_IntrDmaHandlerTable[i] = handler;
    dispatch = 99;
    assert(startIntrDMA() == setIntrDMA);
    assert(clear_calls == 1 && dispatch == 0 && event == 3 && callback == 1);
    for (i = 0; i < 8; ++i) assert(g_IntrDmaHandlerTable[i] == 0);
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            exe = pathlib.Path(directory) / "intr-dma-start-test"
            result = subprocess.run(
                ["cc", "-I", str(ROOT / "include"), "-include",
                 str(ROOT / "tools/tests/host_psyq.h"), "-std=gnu11", "-O2", "-x", "c", "-", "-o", str(exe)],
                input=harness, text=True, capture_output=True,
            )
            self.assertEqual(result.returncode, 0, result.stderr)
            subprocess.run([str(exe)], check=True, timeout=10)


if __name__ == "__main__":
    unittest.main()
