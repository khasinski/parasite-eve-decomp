import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]


class Room350EffectTableScaleTests(unittest.TestCase):
    @unittest.skipUnless(shutil.which("cc"), "host C compiler unavailable")
    def test_threshold_and_post_lookup_fields(self):
        source = (ROOT / "src/overlays/room_m350/RoomEffect_TableTransform.c").read_text()
        source = source.split('int func_8019569C', 1)[0]
        source = source.replace('register int specialKind asm("$4");', 'int specialKind;')
        harness = '#include <assert.h>\n#include <string.h>\n' + source + r'''
int D_800E27EC, D_800F3428, D_800966EC[4096], D_8019A3D0[4];
unsigned short D_800F336C, D_800E1204[8];
unsigned char D_8019A4EC[32];
short D_800F336A;
static short position[4];
static int lookups, draws, nextCounter, expectedSize, expectedScale, expectedShade;
static unsigned int multiplier;
static int handleValue;
int GetClut(int x, int y) {
    unsigned int index = ((unsigned int)(nextCounter - 1) << 7) & 0xF80;
    assert(lookups++ == 0 && draws == 0 && x == 0);
    assert(y == D_800E1204[D_800F336C] +
           (D_800F336C == 4 && D_800F3428 ? 8 : 4));
    D_800E27EC = nextCounter; D_800F336A = expectedScale;
    D_8019A4EC[(nextCounter - 1) / 2] = multiplier;
    D_800966EC[index] = (int)(0x76540000U | (unsigned short)expectedShade);
    position[3] = expectedSize;
    return handleValue;
}
void func_800CEE20(void *point, int zero, int a, int b, int scale,
                  unsigned int handle, int flag, int shade, void *colors) {
    assert(lookups == 1 && draws++ == 0 && point == position && zero == 0);
    assert(a == expectedSize && b == expectedSize);
    assert(scale == expectedScale * (int)multiplier + 128);
    assert(handle == (unsigned short)handleValue && flag == 1);
    assert(shade == expectedShade >> 5 && colors == D_8019A3D0);
}
int main(void) {
    const short values[] = {-32768, -33, -1, 0, 32, 32767};
    const unsigned int weights[] = {0, 1, 127, 128, 255};
    short before[4];
    int counter, event, kind, special, h, s, w, i;
    for (counter = -16; counter <= 32; ++counter)
    for (event = -1; event <= 3; ++event) {
        if (event == 2) continue;
        D_800E27EC = counter; lookups = draws = 0;
        assert(func_80195564(event, (Particle *)0) == (event == 1 && counter >= 8));
        assert(lookups == 0 && draws == 0);
    }
    for (counter = 0; counter < 32; ++counter)
    for (kind = 0; kind < 8; ++kind) for (special = 0; special < 2; ++special)
    for (h = 0; h < 2; ++h) for (s = 0; s < 6; ++s) for (w = 0; w < 5; ++w) {
        for (i = 0; i < 8; ++i) D_800E1204[i] = h ? 65535 : i * 100;
        memset(D_8019A4EC, 37, sizeof(D_8019A4EC));
        D_800E27EC = counter; nextCounter = counter + 1;
        D_800F336C = kind; D_800F3428 = special; D_800F336A = 999;
        expectedScale = values[s]; expectedShade = values[5-s];
        expectedSize = values[(s+1)%6]; multiplier = weights[w];
        handleValue = h ? -1 : 0x12345;
        position[0] = 11; position[1] = -22; position[2] = 33; position[3] = 999;
        memcpy(before, position, sizeof(position)); before[3] = expectedSize;
        lookups = draws = 0;
        assert(func_80195564(2, (Particle *)position) == 0);
        assert(lookups == 1 && draws == 1);
        assert(memcmp(position, before, sizeof(position)) == 0);
    }
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            exe = pathlib.Path(directory) / "room350-effect-table-scale"
            result = subprocess.run(
                ["cc", "-std=gnu11", "-O2", "-x", "c", "-", "-o", str(exe)],
                input=harness, text=True, capture_output=True,
            )
            self.assertEqual(result.returncode, 0, result.stderr)
            subprocess.run([str(exe)], check=True, timeout=10)
