import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]


class Room350EffectGateTests(unittest.TestCase):
    @unittest.skipUnless(shutil.which("cc"), "host C compiler unavailable")
    def test_gate_position_and_signed_table_value(self):
        source = (ROOT / "src/overlays/room_m350/RoomEffect_Threshold4Callback.c").read_text()
        harness = '#include <assert.h>\n' + source + r'''
int D_800E27EC, D_8019A614[2];
short D_800966EE[8192];
static int calls, expectedShade, coordinates[3];
void func_800D004C(short *point, int a, int b, int c, int d, int e, int f,
                  int *first, int *second, int shade, int flag) {
    int i;
    assert(calls++ == 0);
    for (i = 0; i < 3; ++i) assert(point[i] == (short)coordinates[i]);
    assert(a == 512 && b == 512 && c == 16 && d == 0);
    assert(e == expectedShade && f == expectedShade);
    assert(first == D_8019A614 && second == D_8019A614 + 1);
    assert(shade == expectedShade >> 5 && flag == 1);
}
int main(void) {
    const int values[] = {-32768, -1025, -33, -32, -1, 0, 1, 31, 32, 32767};
    const int points[][3] = {{0, 1, -1}, {32767, -32768, 65535},
                            {65536, -65537, 1234567}, {-99, 500, 700}};
    int *position = coordinates;
    int event, counter, t, p, i;
    for (counter = -16; counter <= 64; ++counter) {
        D_800E27EC = counter;
        for (event = -2; event < 5; ++event) {
            if (event == 2) continue;
            calls = 0;
            assert(func_8019A134(event, 0) == (event == 1 && counter >= 4));
            assert(calls == 0);
        }
    }
    for (counter = 1; counter <= 32; ++counter)
    for (t = 0; t < 10; ++t) for (p = 0; p < 4; ++p) {
        for (i = 0; i < 8192; ++i) D_800966EE[i] = 12345;
        D_800E27EC = counter;
        expectedShade = values[t];
        D_800966EE[((counter - 1) << 9) & 0x1E00] = expectedShade;
        for (i = 0; i < 3; ++i) coordinates[i] = points[p][i];
        calls = 0;
        assert(func_8019A134(2, &position) == 0);
        assert(calls == 1);
    }
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            exe = pathlib.Path(directory) / "room350-effect-gate"
            result = subprocess.run(
                ["cc", "-std=gnu11", "-O2", "-x", "c", "-", "-o", str(exe)],
                input=harness, text=True, capture_output=True,
            )
            self.assertEqual(result.returncode, 0, result.stderr)
            subprocess.run([str(exe)], check=True, timeout=10)
