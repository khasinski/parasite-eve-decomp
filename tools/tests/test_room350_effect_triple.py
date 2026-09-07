import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]


class Room350EffectTripleTests(unittest.TestCase):
    @unittest.skipUnless(shutil.which("cc"), "host C compiler unavailable")
    def test_signed_samples_palette_and_reload_order(self):
        source = (ROOT / "src/overlays/room_m350/RoomEffect_TripleScalePair.c").read_text()
        source = source[:source.index('int func_80193BCC')]
        source = source.replace('asm("$3")', '')
        harness = '#include <assert.h>\n#include <string.h>\n' + source + r'''
int D_800E27EC, D_800F3428, D_800966EC[4096];
unsigned short D_800F336C, D_800E1204[8];
short D_800F336A;
static int point, replacement, lookups, draws, nextCounter, expectedSize;
static int expectedScale, expectedShade, handleValue;
static void *object;
int GetClut(int x, int y) {
    unsigned int index = ((unsigned int)(nextCounter - 1) << 7) & 0xF80;
    assert(lookups++ == 0 && draws == 0 && x == 0);
    assert(y == D_800E1204[D_800F336C] +
           (D_800F336C == 4 && D_800F3428 ? 8 : 4));
    /* Deliberate stub mutations test reloads, not real GetClut side effects. */
    memset(D_800966EC, 0, sizeof(D_800966EC));
    D_800E27EC = nextCounter; D_800F336A = expectedScale;
    D_800966EC[index] = (int)(0x76540000U | (unsigned short)expectedShade);
    object = &replacement;
    return handleValue;
}
void func_800CEE20(void *p, int zero, int a, int b, int scale,
                  unsigned int handle, int flag, int shade, void *colors) {
    assert(lookups == 1 && draws++ == 0 && p == &replacement && zero == 0);
    assert(a == expectedSize && b == expectedSize);
    assert(scale == expectedScale * 6 + 128);
    assert(handle == (unsigned short)handleValue && flag == 1);
    assert(shade == expectedShade >> 5 && colors == 0);
}
int main(void) {
    const short values[] = {-32768, -33, -1, 0, 32, 32767};
    int counter, event, kind, special, h, s, i;
    for (counter = -16; counter <= 80; ++counter)
    for (event = -1; event <= 3; ++event) {
        if (event == 2) continue;
        D_800E27EC = counter; lookups = draws = 0;
        assert(func_80193A80(event, 0) == (event == 1 && counter >= 16));
        assert(lookups == 0 && draws == 0);
    }
    for (counter = -16; counter <= 80; ++counter)
    for (kind = 0; kind < 8; ++kind) for (special = 0; special < 2; ++special)
    for (h = 0; h < 2; ++h) for (s = 0; s < 6; ++s) {
        unsigned int offset = ((unsigned int)(counter - 1) << 8) & 0x3F00;
        memset(D_800966EC, 0, sizeof(D_800966EC));
        memcpy((char *)D_800966EC + offset, &values[s], sizeof(short));
        for (i = 0; i < 8; ++i) D_800E1204[i] = h ? 65535 : i * 100;
        D_800E27EC = counter; nextCounter = counter + 1;
        D_800F336C = kind; D_800F3428 = special; D_800F336A = 999;
        expectedSize = values[s] * 3;
        expectedScale = values[s]; expectedShade = values[5-s];
        handleValue = h ? -1 : 0x12345;
        object = &point; lookups = draws = 0;
        assert(func_80193A80(2, &object) == 0);
        assert(lookups == 1 && draws == 1);
    }
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            exe = pathlib.Path(directory) / "room350-effect-triple"
            result = subprocess.run(
                ["cc", "-std=gnu11", "-O2", "-x", "c", "-", "-o", str(exe)],
                input=harness, text=True, capture_output=True,
            )
            self.assertEqual(result.returncode, 0, result.stderr)
            subprocess.run([str(exe)], check=True, timeout=10)
