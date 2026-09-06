import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]


class Room350EffectBallisticTests(unittest.TestCase):
    @unittest.skipUnless(shutil.which("cc"), "host C compiler unavailable")
    def test_motion_and_palette_rendering(self):
        source = (ROOT / "src/overlays/room_m350/RoomEffect_BallisticPaletteCallback.c").read_text()
        harness = '#include <assert.h>\n#include <string.h>\n' + source + r'''
int D_800E27EC, D_800F3428, D_800966EC[4096], D_8019A598[4];
unsigned short D_800F336C, D_800E1204[8];
short D_800F336A;
static short position[5];
static int lookups, draws, nextCounter, expectedScale, expectedShade, handleValue;
int GetClut(int x, int y) {
    unsigned int index = ((unsigned int)(nextCounter - 1) << 7) & 0xF80;
    assert(lookups++ == 0 && draws == 0 && x == 16);
    assert(y == D_800E1204[D_800F336C] +
           (D_800F336C == 4 && D_800F3428 ? 4 : 0));
    D_800E27EC = nextCounter; D_800F336A = expectedScale;
    D_800966EC[index] = (int)(0x76540000U | (unsigned short)expectedShade);
    return handleValue;
}
void func_800CEE20(void *point, int zero, int a, int b, int scale,
                  unsigned int handle, int flag, int shade, void *colors) {
    assert(lookups == 1 && draws++ == 0 && point == position && zero == 0);
    assert(a == 12288 && b == 12288);
    assert(scale == expectedScale * (((nextCounter - 1) >> 1) & 7) + 200);
    assert(handle == (unsigned short)handleValue && flag == 1);
    assert(shade == expectedShade >> 5 && colors == D_8019A598);
}
int main(void) {
    const short values[] = {-32768, -33, -1, 0, 32, 32767};
    short expected[5];
    int counter, event, y, v, a, kind, special, h, s, i;
    for (counter = -16; counter <= 32; ++counter)
    for (event = -1; event <= 3; ++event)
    for (y = 0; y < 6; ++y) for (v = 0; v < 6; ++v) for (a = 0; a < 6; ++a) {
        int result = event == 1 && counter >= 16;
        if (event == 2) continue;
        position[0] = 11; position[1] = values[y]; position[2] = -22;
        position[3] = values[v]; position[4] = values[a];
        memcpy(expected, position, sizeof(position));
        if (event == 1 && counter < 16) {
            expected[1] = (short)(position[1] + position[3]);
            expected[3] = (short)(position[3] - position[4]);
        }
        D_800E27EC = counter; lookups = draws = 0;
        assert(func_801968D8(event, position) == result);
        assert(lookups == 0 && draws == 0);
        assert(memcmp(position, expected, sizeof(position)) == 0);
    }
    for (counter = 0; counter < 32; ++counter)
    for (kind = 0; kind < 8; ++kind) for (special = 0; special < 2; ++special)
    for (h = 0; h < 2; ++h) for (s = 0; s < 6; ++s) {
        for (i = 0; i < 8; ++i) D_800E1204[i] = h ? 65535 : i * 100;
        D_800E27EC = counter; nextCounter = counter + 1;
        D_800F336C = kind; D_800F3428 = special; D_800F336A = 999;
        expectedScale = values[s]; expectedShade = values[5-s];
        handleValue = h ? -1 : 0x12345;
        memcpy(expected, position, sizeof(position));
        lookups = draws = 0;
        assert(func_801968D8(2, position) == 0);
        assert(lookups == 1 && draws == 1);
        assert(memcmp(position, expected, sizeof(position)) == 0);
    }
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            exe = pathlib.Path(directory) / "room350-effect-ballistic"
            result = subprocess.run(
                ["cc", "-std=gnu11", "-O2", "-x", "c", "-", "-o", str(exe)],
                input=harness, text=True, capture_output=True,
            )
            self.assertEqual(result.returncode, 0, result.stderr)
            subprocess.run([str(exe)], check=True, timeout=10)
