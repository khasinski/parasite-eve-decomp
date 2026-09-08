import pathlib
import shutil
import subprocess
import tempfile
import unittest


ROOT = pathlib.Path(__file__).resolve().parents[2]


class DrawEnvBuilderTests(unittest.TestCase):
    @unittest.skipUnless(shutil.which("cc"), "host C compiler unavailable")
    def test_packet_commands_and_background_paths(self):
        source = r'''
#include <assert.h>
#include <string.h>
#include "src/main/gpu/drawenv.c"
GpuDebugState D_8009574C;
unsigned int Gpu_BuildDrawAreaTopLeftCmd(short x, short y) { return 1; }
unsigned int Gpu_BuildDrawAreaBottomRightCmd(short x, short y) { return 2; }
unsigned int Gpu_BuildDrawOffsetCmd(short x, short y) { return 3; }
unsigned int Gpu_BuildDrawModeCmd(int a, int b, int c) { return 4; }
unsigned int Gpu_BuildTexWindowCmd(RECT *rect) { return 5; }
static unsigned int pair(short x, short y) {
    return (unsigned short)x | ((unsigned int)(unsigned short)y << 16);
}
static void check(int back, int background, short x, short w, short h) {
    DRAWENV env;
    union { DR_ENV packet; unsigned int words[32]; } output;
    unsigned int i, opcode;
    short width = w < 0 ? 0 : w > 1023 ? 1023 : w;
    short height = h < 0 ? 0 : h > 511 ? 511 : h;
    int fast = back && !(x & 63) && !(width & 63);
    memset(&env, 0, sizeof(env));
    memset(&output, 0xA5, sizeof(output));
    env.clip.x = x;
    env.clip.y = 31;
    env.clip.w = w;
    env.clip.h = h;
    env.ofs[0] = 7;
    env.ofs[1] = 11;
    env.isbg = background;
    env.r0 = 0x12;
    env.g0 = 0x34;
    env.b0 = 0x56;
    D_8009574C.width = 1024;
    D_8009574C.height = 512;
    if (back) Gpu_SetDrawEnvBack(&output.packet, &env);
    else Gpu_SetDrawEnvFront(&output.packet, &env);
    assert((output.words[0] & 0xFFFFFF) == 0xA5A5A5);
    assert((output.words[0] >> 24) == (background ? 9 : 6));
    for (i = 1; i <= 5; i++) assert(output.words[i] == i);
    assert(output.words[6] == 0xE6000000);
    if (background) {
        opcode = fast ? 0x02000000 : 0x60000000;
        assert(output.words[7] == (opcode | 0x563412));
        assert(output.words[8] == pair(fast ? x : x - 7, fast ? 31 : 20));
        assert(output.words[9] == pair(width, height));
    }
    for (i = background ? 10 : 7; i < 32; i++)
        assert(output.words[i] == 0xA5A5A5A5);
}
int main(void) {
    int back;
    for (back = 0; back < 2; back++) {
        check(back, 0, 64, 128, 64);
        check(back, 1, 64, 128, 64);
        check(back, 1, 65, 128, 64);
        check(back, 1, 64, 129, 64);
        check(back, 1, 64, -1, -2);
        check(back, 1, 64, 2048, 1024);
        check(back, 1, -64, 64, 0);
    }
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            exe = pathlib.Path(directory) / "drawenv-test"
            result = subprocess.run(
                ["cc", "-std=gnu11", "-O2", "-fno-strict-aliasing",
                 "-I", str(ROOT), "-I", str(ROOT / "include"),
                 "-x", "c", "-", "-o", str(exe)],
                input=source, text=True, capture_output=True,
            )
            self.assertEqual(result.returncode, 0, result.stderr)
            subprocess.run([str(exe)], check=True, timeout=10)


if __name__ == "__main__":
    unittest.main()
