import pathlib
import shutil
import subprocess
import tempfile
import unittest


ROOT = pathlib.Path(__file__).resolve().parents[2]


class GpuPacketInitTests(unittest.TestCase):
    @unittest.skipUnless(shutil.which("cc"), "host C compiler unavailable")
    def test_draw_mask_and_mode_packets(self):
        source = (ROOT / "src/main/gpu/packet_init.c").read_text()
        harness = source + r'''
#include <assert.h>
static int mode_calls, window_calls;
int Gpu_BuildDrawModeCmd(int a, int b, int c) {
    assert(a == 7 && b == 9 && c == 0x5678);
    ++mode_calls;
    return 0xE1005678;
}
int Gpu_BuildTexWindowCmd(int a) {
    assert(a == 11);
    ++window_calls;
    return 0xE2000011;
}
int main(void) {
    unsigned char storage[12];
    GpuCmdPacket *packet = (GpuCmdPacket *)storage;
    int mask, flag;
    for (mask = 0; mask <= 1; ++mask) for (flag = 0; flag <= 1; ++flag) {
        *(unsigned int *)storage = 0xDEADBEEF;
        SetDrawMask(storage, mask, flag);
        assert(storage[3] == 2);
        assert(*(unsigned int *)(storage + 4) ==
               (mask ? 0xE6000002 : 0xE6000000) | flag);
        assert(*(unsigned int *)(storage + 8) == 0);
    }
    for (mask = 0; mask <= 1; ++mask) {
        Gpu_InitDrawMaskSetPacket(storage, mask);
        assert(storage[3] == 2);
        assert(*(unsigned int *)(storage + 4) ==
               (mask ? 0xE6000001 : 0xE6000000));
        assert(*(unsigned int *)(storage + 8) == 0);
    }
    mode_calls = window_calls = 0;
    Gpu_InitDrawModeTexWindowPacket(packet, 7, 9, 0x12345678, 11);
    assert(storage[3] == 2 && packet->field4 == 0xE1005678);
    assert(packet->field8 == 0xE2000011 && mode_calls == 1 && window_calls == 1);
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            exe = pathlib.Path(directory) / "packet-init-test"
            result = subprocess.run(
                ["cc", "-std=gnu11", "-O2", "-I", str(ROOT / "include"),
                 "-x", "c", "-", "-o", str(exe)],
                input=harness, text=True, capture_output=True,
            )
            self.assertEqual(result.returncode, 0, result.stderr)
            subprocess.run([str(exe)], check=True, timeout=10)


if __name__ == "__main__":
    unittest.main()
