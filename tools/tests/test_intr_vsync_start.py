import pathlib
import shutil
import subprocess
import tempfile
import unittest


ROOT = pathlib.Path(__file__).resolve().parents[2]


class IntrVSyncStartTests(unittest.TestCase):
    @unittest.skipUnless(shutil.which("cc"), "host C compiler unavailable")
    def test_clears_handlers_installs_vsync_trap_and_returns_registrar(self):
        source = (ROOT / "src/main/psyq/libapi/startIntrVSync.c").read_text()
        harness = source + r'''
#include <assert.h>
void handler(void) {}
void (*g_IntrVSyncCallbackTable[8])(void);
volatile int g_VSyncCount;
static int timer_mode;
int *D_800956B0 = &timer_mode;
static int clear_calls, event, callback;
void memclrIntrVSync(void *ptr, int count) {
    int i;
    void (**handlers)(void) = (void (**)(void))ptr;
    assert(ptr == g_IntrVSyncCallbackTable && count == 8);
    ++clear_calls;
    for (i = 0; i < count; ++i) handlers[i] = 0;
}
void trapIntrVSync(void) {}
void setIntrVSync(unsigned int index, void (*handler)(void)) {
    (void)index;
    (void)handler;
}
void InterruptCallback(int irq, void (*handler)(void)) {
    assert(irq == 0 && handler == trapIntrVSync);
    event = irq;
    callback = 1;
}
int main(void) {
    int i;
    for (i = 0; i < 8; ++i) g_IntrVSyncCallbackTable[i] = handler;
    g_VSyncCount = 99;
    timer_mode = 0;
    assert(startIntrVSync() == setIntrVSync);
    assert(clear_calls == 1 && timer_mode == 0x107 && g_VSyncCount == 0);
    assert(event == 0 && callback == 1);
    for (i = 0; i < 8; ++i) assert(g_IntrVSyncCallbackTable[i] == 0);
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            exe = pathlib.Path(directory) / "intr-vsync-start-test"
            result = subprocess.run(
                ["cc", "-std=gnu11", "-O2", "-x", "c", "-", "-o", str(exe)],
                input=harness, text=True, capture_output=True,
            )
            self.assertEqual(result.returncode, 0, result.stderr)
            subprocess.run([str(exe)], check=True, timeout=10)


if __name__ == "__main__":
    unittest.main()
