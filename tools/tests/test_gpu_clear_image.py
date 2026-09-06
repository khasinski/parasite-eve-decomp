import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]


class ClearImageTests(unittest.TestCase):
    @unittest.skipUnless(shutil.which("cc"), "host C compiler unavailable")
    def test_packet_paths_clamping_and_restore(self):
        source = (ROOT / "src/main/main/Gpu_DmaVramTransfer.c").read_text()
        harness = source + r'''
#include <assert.h>
#include <stdint.h>
short D_80095750, D_80095752;
unsigned int D_800A3300[12], D_800A3328[6];
static unsigned int gpuStatus;
unsigned int *D_80095854 = &gpuStatus;
static int params, starts, slow;
unsigned int _param(int parameter) {
    assert(starts == 0 && parameter == 3 + params);
    ++params;
    return 0x12300 + parameter;
}
void Gpu_StartDmaTransfer(unsigned int packet) {
    assert(packet == 0x800A3300 && starts == 0);
    assert(params == (slow ? 3 : 0));
    if (slow) assert(D_800A3328[3] == 0xE5012305);
    ++starts;
}
static unsigned int packed(short low, short high) {
    return (unsigned short)low | ((unsigned int)(unsigned short)high << 16);
}
int main(void) {
    static const short values[] = {-32768, -1, 0, 1, 63, 64, 65, 255, 320, 32767};
    static const unsigned int colors[] = {0, 0xFFFFFF, 0x12345678, 0x80000000, 0xFFFFFFFF};
    int mode, x, w, h, c, i;
    for (mode = 0; mode < 3; ++mode) {
        D_80095750 = mode == 0 ? 320 : mode == 1 ? 640 : 1024;
        D_80095752 = mode == 0 ? 240 : mode == 1 ? 480 : 512;
        gpuStatus = mode == 0 ? 0 : mode == 1 ? 0xFFFFFFFF : 0x12345678;
        for (x = 0; x < 10; ++x) for (w = 0; w < 10; ++w)
        for (h = 0; h < 10; ++h) for (c = 0; c < 5; ++c) {
            RECT rect = {values[x], values[(x + 3) % 10], values[w], values[h]};
            short expectedW = values[w] < 0 ? 0 : values[w] >= D_80095750
                ? D_80095750 - 1 : values[w];
            short expectedH = values[h] < 0 ? 0 : values[h] >= D_80095752
                ? D_80095752 - 1 : values[h];
            unsigned int color = colors[c];
            unsigned int drawMode = 0xE1000000 | (gpuStatus & 0x7FF) | ((color >> 31) << 10);
            slow = (values[x] & 63) || (expectedW & 63);
            params = starts = 0;
            for (i = 0; i < 12; ++i) D_800A3300[i] = 0xDEADBEEF;
            for (i = 0; i < 6; ++i) D_800A3328[i] = 0xDEADBEEF;
            assert(Gpu_DmaVramTransfer(&rect, color) == 0);
            assert(starts == 1 && rect.w == expectedW && rect.h == expectedH);
            assert(rect.x == values[x] && rect.y == values[(x + 3) % 10]);
            if (slow) {
                assert(D_800A3300[0] == (((uintptr_t)D_800A3328 & 0xFFFFFF) | 0x08000000));
                assert(D_800A3300[1] == 0xE3000000 && D_800A3300[2] == 0xE4FFFFFF);
                assert(D_800A3300[3] == 0xE5000000 && D_800A3300[4] == 0xE6000000);
                assert(D_800A3300[5] == drawMode);
                assert(D_800A3300[6] == ((color & 0xFFFFFF) | 0x60000000));
                assert(D_800A3300[7] == packed(rect.x, rect.y));
                assert(D_800A3300[8] == packed(expectedW, expectedH));
                assert(D_800A3328[0] == 0x03FFFFFF);
                assert(D_800A3328[1] == 0xE3012303 && D_800A3328[2] == 0xE4012304);
            } else {
                assert(D_800A3300[0] == 0x05FFFFFF && D_800A3300[1] == 0xE6000000);
                assert(D_800A3300[2] == drawMode);
                assert(D_800A3300[3] == ((color & 0xFFFFFF) | 0x02000000));
                assert(D_800A3300[4] == packed(rect.x, rect.y));
                assert(D_800A3300[5] == packed(expectedW, expectedH));
            }
            for (i = slow ? 9 : 6; i < 12; ++i) assert(D_800A3300[i] == 0xDEADBEEF);
            for (i = slow ? 4 : 0; i < 6; ++i) assert(D_800A3328[i] == 0xDEADBEEF);
        }
    }
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            exe = pathlib.Path(directory) / "clear-image-test"
            result = subprocess.run(
                ["cc", "-std=gnu11", "-O2", "-fno-strict-aliasing",
                 "-Wno-pointer-to-int-cast", "-I", str(ROOT / "include"),
                 "-I", str(ROOT / "tools/m2c"), "-x", "c", "-", "-o", str(exe)],
                input=harness, text=True, capture_output=True,
            )
            self.assertEqual(result.returncode, 0, result.stderr)
            subprocess.run([str(exe)], check=True, timeout=10)


if __name__ == "__main__":
    unittest.main()
