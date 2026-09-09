import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]


class SpuInitTests(unittest.TestCase):
    @unittest.skipUnless(shutil.which("cc"), "host C compiler unavailable")
    def test_cold_warm_init_and_reverb_reset_order(self):
        source = (ROOT / "src/main/psyq/libspu/_SpuInit.c").read_text()
        harness = source + r'''
#include <assert.h>
unsigned short D_8009B3B8[24];
ReverbState D_8009B3A0;
int D_8009B390, D_8009B394, D_8009B398, D_8009B46C;
int D_8009B45C, D_8009B460, D_8009B464, D_8009B38C;
int D_8009B418, D_8009B388, D_8009B3B4, D_8009B3B0, D_8009B3E8;
static int phase, expectedMode;
static int *late[] = {&D_8009B45C, &D_8009B460, &D_8009B464, &D_8009B38C,
    &D_8009B418, &D_8009B388, &D_8009B3B4, &D_8009B3B0, &D_8009B3E8};
int ResetCallback(void) { assert(phase == 0); phase = 1; return 0; }
int _spu_init(int mode) { assert(phase == 1 && mode == expectedMode); phase = 2; return 0; }
void SpuStart(void) {
    int i;
    assert(phase == 2);
    for (i = 0; i < 24; ++i)
        assert(D_8009B3B8[i] == (expectedMode ? 0x100 + i : 0xC000));
    assert(D_8009B390 == 9 && D_8009B3A0.mode == 9);
    phase = 3;
}
void _spu_FsetRXX(u32 reg, u32 area, u32 flag) {
    int i;
    assert(phase == 3 && reg == 0xD1 && area == 0xF800 && flag == 0);
    assert(D_8009B390 == 0 && D_8009B394 == 0 && D_8009B398 == area);
    assert(D_8009B3A0.mode == 0 && D_8009B3A0.depth[0] == 0);
    assert(D_8009B3A0.depth[1] == 0 && D_8009B3A0.delay == 0);
    assert(D_8009B3A0.feedback == 0);
    for (i = 0; i < 9; ++i) assert(*late[i] == 9);
    phase = 4;
}
int main(void) {
    int mode, i;
    for (mode = -1; mode <= 2; ++mode) {
        expectedMode = mode; phase = 0;
        for (i = 0; i < 24; ++i) D_8009B3B8[i] = 0x100 + i;
        for (i = 0; i < 9; ++i) *late[i] = 9;
        D_8009B390 = D_8009B394 = D_8009B398 = 9;
        D_8009B46C = 0xF800;
        D_8009B3A0.mode = D_8009B3A0.delay = D_8009B3A0.feedback = 9;
        D_8009B3A0.depth[0] = D_8009B3A0.depth[1] = 9;
        _SpuInit(mode);
        assert(phase == 4 && D_8009B46C == 0xF800);
        for (i = 0; i < 9; ++i) assert(*late[i] == 0);
    }
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            exe = pathlib.Path(directory) / "spu-init-test"
            result = subprocess.run(
                ["cc", "-I", str(ROOT / "include"), "-include",
                 str(ROOT / "tools/tests/host_psyq.h"), "-std=gnu11", "-O2", "-x", "c", "-", "-o", str(exe)],
                input=harness, text=True, capture_output=True,
            )
            self.assertEqual(result.returncode, 0, result.stderr)
            subprocess.run([str(exe)], check=True, timeout=10)


if __name__ == "__main__":
    unittest.main()
