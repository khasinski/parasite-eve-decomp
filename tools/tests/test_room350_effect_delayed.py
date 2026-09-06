import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]


class Room350EffectDelayedTests(unittest.TestCase):
    @unittest.skipUnless(shutil.which("cc"), "host C compiler unavailable")
    def test_delay_saved_size_and_reloaded_shade(self):
        source = (ROOT / "src/overlays/room_m350/RoomEffect_DelayedTallPaletteCallback.c").read_text()
        source = source.replace('extern short D_800966EE[];',
            'static union { int alignment; short values[8194]; } table;\n'
            '#define D_800966EE (table.values + 1)')
        harness = '#include <assert.h>\n#include <string.h>\n' + source + r'''
int D_800E27EC, D_800F3428, D_8019A3D0[4];
unsigned short D_800F336C, D_800E1204[8];
short D_800F336A;
static short object[5];
static int lookups, draws, handleValue, savedFrame, sizeValue, shadeValue;
static short vectorY, scaleValue, objectScale;
int GetClut(int x, int y) {
    unsigned int index = ((unsigned int)savedFrame << 7) & 0x1F80;
    assert(lookups++ == 0 && draws == 0 && x == 0);
    assert(y == D_800E1204[D_800F336C] +
           (D_800F336C == 4 && D_800F3428 ? 4 : 0));
    /* Stub mutations distinguish captured size/vector from later reads. */
    D_800E27EC += 3; D_800F336A = scaleValue;
    D_800966EE[index] = shadeValue;
    object[3] = 123; object[4] = objectScale;
    return handleValue;
}
void func_800CEE20(void *p, void *v, int a, int b, int scale,
                  unsigned int handle, int flag, int shade, void *colors) {
    short *vector = v;
    assert(lookups == 1 && draws++ == 0 && p == object);
    assert(vector[0] == 1024 && vector[1] == vectorY && vector[2] == 1024 && vector[3] == 1);
    assert(a == sizeValue && b == sizeValue * 2);
    assert(scale == scaleValue * objectScale + 64);
    assert(handle == (unsigned short)handleValue && flag == 1);
    assert(shade == shadeValue >> 5 && colors == D_8019A3D0);
}
int main(void) {
    const short values[] = {-32768, -33, -1, 0, 32, 32767};
    int counter, event, kind, special, h, s, m, i;
    assert(sizeof(SignedHalf) == 4);
    for (counter = -16; counter <= 80; ++counter)
    for (event = -1; event <= 3; ++event) {
        if (event == 2 && counter >= 2) continue;
        D_800E27EC = counter; lookups = draws = 0;
        assert(func_80195218(event, 0) == (event == 1 && counter >= 16));
        assert(lookups == 0 && draws == 0);
    }
    for (counter = 2; counter <= 80; ++counter)
    for (kind = 0; kind < 8; ++kind) for (special = 0; special < 2; ++special)
    for (h = 0; h < 2; ++h) for (s = 0; s < 6; ++s) for (m = 0; m < 6; ++m) {
        unsigned int index = ((unsigned int)(counter - 2) << 7) & 0x1F80;
        for (i = 0; i < 8; ++i) D_800E1204[i] = h ? 65535 : i * 100;
        memset(&table, 0x5A, sizeof(table));
        D_800E27EC = counter; savedFrame = counter - 2;
        D_800F336C = kind; D_800F3428 = special; D_800F336A = 999;
        sizeValue = values[s]; shadeValue = values[5-s];
        scaleValue = values[s]; objectScale = values[m]; vectorY = values[5-m];
        D_800966EE[index] = sizeValue;
        handleValue = h ? -1 : 0x12345;
        object[0] = 11; object[1] = 22; object[2] = 33;
        object[3] = vectorY; object[4] = 999;
        lookups = draws = 0;
        assert(func_80195218(2, object) == 0);
        assert(lookups == 1 && draws == 1);
        assert(object[0] == 11 && object[1] == 22 && object[2] == 33);
    }
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            exe = pathlib.Path(directory) / "room350-effect-delayed"
            result = subprocess.run(
                ["cc", "-std=gnu11", "-O2", "-x", "c", "-", "-o", str(exe)],
                input=harness, text=True, capture_output=True,
            )
            self.assertEqual(result.returncode, 0, result.stderr)
            subprocess.run([str(exe)], check=True, timeout=10)
