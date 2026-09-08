import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]


class GpuImage2Tests(unittest.TestCase):
    @unittest.skipUnless(shutil.which("cc"), "host C compiler unavailable")
    def test_transfer_dispatch_and_move_packet(self):
        unit = (ROOT / "src/main/gpu/dma_transfer.c").read_text()
        prefix_end = unit.index("int LoadImage2")
        store_marker = unit.index("extern char D_800118E0")
        move_marker = unit.index("extern char D_800118EC")
        dispatch_marker = unit.index("extern char D_80011928")
        source = (unit[:prefix_end] + unit[prefix_end:store_marker] +
                  unit[store_marker:move_marker] + unit[move_marker:dispatch_marker])
        self.assertEqual(source.count(' asm("$2")'), 3)
        harness = source.replace(' asm("$2")', "") + r'''
#include <assert.h>
char D_800119BC[] = "load", D_800118E0[] = "store", D_800118EC[] = "move";
static volatile unsigned int dma;
static unsigned int gpu, data[16];
volatile unsigned int *g_GpuDmaChcrPtr = &dma;
unsigned int *g_GpuGp1Ptr = &gpu;
int g_GpuDmaTimeoutDeadline, g_GpuDmaWaitLoopCounter;
unsigned int D_800957EC[3];
static GpuCallbacks table;
GpuCallbacks *D_80095744 = &table;
static RECT rect;
static int kind, phase, polls, idleAt, readyAt, timeoutAt;
void checkRECT(char *name, RECT *r) {
    assert(phase == 0 && r == &rect);
    assert(name == (kind == 0 ? D_800119BC : kind == 1 ? D_800118E0 : D_800118EC));
    phase = 1;
}
int VSync(int mode) { assert(phase == 1 && mode == -1); phase = 2; return 42; }
int Gpu_DmaTimeoutCheck(void) {
    assert(phase == 2 && ++polls < 10);
    assert(g_GpuDmaTimeoutDeadline == 282 && g_GpuDmaWaitLoopCounter == 0);
    if (polls == timeoutAt) return -1;
    if (polls >= idleAt) dma = 0;
    if (polls >= readyAt) gpu = 0x04000000;
    return 0;
}
void Gpu_RestoreDmaCallback(void) { assert(0); }
void DMACallback(int channel, void (*callback)(void)) {
    assert(phase == 2 && channel == 2 && callback == Gpu_RestoreDmaCallback);
    assert(dma == 0 && gpu == 0x04000000);
    phase = 3;
}
static int transfer(RECT *r, unsigned int *p) {
    assert(phase == 3 && r == &rect && p == data);
    phase = 4;
    return -77;
}
static int move(void *packet) {
    assert(phase == 3 && packet == (char *)D_800957EC - 8);
    assert(D_800957EC[0] == ((unsigned int)(unsigned short)rect.y << 16 |
                            (unsigned short)rect.x));
    assert(D_800957EC[1] == 0x2345FFFE);
    assert(D_800957EC[2] == ((unsigned int)(unsigned short)rect.h << 16 |
                            (unsigned short)rect.w));
    phase = 4;
    return -77;
}
static void check(int idle, int ready, int timeout, int w, int h) {
    int result, i, invalid = kind == 2 && (!w || !h);
    rect.x = -3; rect.y = 9; rect.w = w; rect.h = h;
    phase = polls = 0; idleAt = idle; readyAt = ready; timeoutAt = timeout;
    dma = idle ? 0x01000000 : 0; gpu = ready ? 0 : 0x04000000;
    for (i = 0; i < 3; ++i) D_800957EC[i] = 0xDEADBEEF;
    result = kind == 0 ? LoadImage2(&rect, data) : kind == 1 ?
        StoreImage2(&rect, data) : MoveImage2(&rect, -2, 0x12345);
    assert(result == (timeout || invalid ? -1 : 0));
    assert(phase == (timeout ? 2 : invalid ? 3 : 4));
    assert(polls == (timeout ? timeout : idle > ready ? idle : ready));
    if (timeout || invalid || kind != 2)
        for (i = 0; i < 3; ++i) assert(D_800957EC[i] == 0xDEADBEEF);
}
int main(void) {
    table.u20.load = transfer; table.u1c.store = transfer; table.u18.moveImage = move;
    for (kind = 0; kind < 3; ++kind) {
        check(0, 0, 0, 5, 7); check(2, 0, 0, 5, 7);
        check(0, 3, 0, 5, 7); check(2, 4, 0, 5, 7);
        check(8, 0, 2, 5, 7); check(0, 8, 2, 5, 7);
        check(0, 0, 0, 0, 7); check(0, 0, 0, 5, 0);
        check(0, 0, 0, -1, -2);
    }
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            exe = pathlib.Path(directory) / "image2-test"
            result = subprocess.run(
                ["cc", "-std=gnu11", "-O2", "-fno-strict-aliasing",
                 "-I", str(ROOT / "include"), "-x", "c", "-", "-o", str(exe)],
                input=harness, text=True, capture_output=True,
            )
            self.assertEqual(result.returncode, 0, result.stderr)
            subprocess.run([str(exe)], check=True, timeout=10)


if __name__ == "__main__":
    unittest.main()
