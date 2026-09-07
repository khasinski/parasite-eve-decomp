import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]


class Room350EffectPulseTests(unittest.TestCase):
    @unittest.skipUnless(shutil.which("cc"), "host C compiler unavailable")
    def test_pre_lookup_scale_and_post_lookup_shade(self):
        source = (ROOT / "src/overlays/room_m350/RoomEffect_PulsingPalettePair.c").read_text()
        source = source[:source.index('int func_801928D4')]
        source = source.replace('extern short D_800966EE[];', '')
        source = source.replace('extern int D_800966EC[];',
            'static int table[4096];\n#define D_800966EC table\n'
            '#define D_800966EE ((short *)table + 1)')
        harness = '#include <assert.h>\n#include <string.h>\n' + source + r'''
int D_800E27EC, D_800F3428;
unsigned short D_800F336C;
unsigned short D_800E1204[8];
static Particle effect;
static int position, nextPosition, lookups, draws, handleValue;
static int nextCounter, expectedScale, expectedShade;
int GetClut(int x, int y) {
    unsigned int index = ((unsigned int)(nextCounter - 1) << 7) & 0xF80;
    assert(lookups++ == 0 && draws == 0 && x == 48);
    assert(y == D_800E1204[D_800F336C] +
           (D_800F336C == 4 && D_800F3428 ? 4 : 0));
    D_800E27EC = nextCounter;
    /* Different high half catches accidental recomputation of pre-call scale. */
    table[index] = (int)(0x7FF00000U | (unsigned short)expectedShade);
    effect.position = (Vector *)&nextPosition;
    return handleValue;
}
void func_800CEE20(void *point, void *context, int a, int b, int c,
                  unsigned int handle, int flag, int shade, void *colors) {
    assert(lookups == 1 && draws++ == 0);
    assert(point == &nextPosition && context == &effect);
    assert(a == expectedScale && b == expectedScale && c == 110);
    assert(handle == (unsigned short)handleValue && flag == 1);
    assert(shade == expectedShade >> 5 && colors == &effect.color);
}
int main(void) {
    const int values[] = {-32768, -33, -1, 0, 32, 32767};
    Particle before;
    int counter, event, kind, special, h, s, i;
    for (counter = -16; counter <= 32; ++counter)
    for (event = -1; event <= 3; ++event) {
        if (event == 2) continue;
        D_800E27EC = counter; lookups = draws = 0;
        assert(func_801927A4(event, 0) == (event == 1 && counter >= 16));
        assert(lookups == 0 && draws == 0);
    }
    for (counter = 0; counter < 32; ++counter)
    for (kind = 0; kind < 8; ++kind) for (special = 0; special < 2; ++special)
    for (h = 0; h < 2; ++h) for (s = 0; s < 6; ++s) {
        unsigned int index = (((unsigned int)counter - 1) << 8) & 0x1F00;
        for (i = 0; i < 4096; ++i) table[i] = 0x12345678;
        for (i = 0; i < 8; ++i) D_800E1204[i] = h ? 65535 : i * 100;
        D_800E27EC = counter; nextCounter = counter + 1;
        D_800F336C = kind; D_800F3428 = special;
        D_800966EE[index] = values[s];
        expectedScale = values[s] * 4 + 2048; expectedShade = values[5-s];
        handleValue = h ? -1 : 0x12345;
        memset(&effect, 0xCC, sizeof(effect));
        effect.position = (Vector *)&position;
        memcpy(&before, &effect, sizeof(effect));
        before.position = (Vector *)&nextPosition;
        lookups = draws = 0;
        assert(func_801927A4(2, &effect) == 0);
        assert(lookups == 1 && draws == 1);
        assert(memcmp(&effect, &before, sizeof(effect)) == 0);
    }
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            exe = pathlib.Path(directory) / "room350-effect-pulse"
            result = subprocess.run(
                ["cc", "-std=gnu11", "-O2", "-fno-strict-aliasing",
                 "-x", "c", "-", "-o", str(exe)],
                input=harness, text=True, capture_output=True,
            )
            self.assertEqual(result.returncode, 0, result.stderr)
            subprocess.run([str(exe)], check=True, timeout=10)
