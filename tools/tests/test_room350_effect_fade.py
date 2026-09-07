import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]


class Room350EffectFadeTests(unittest.TestCase):
    @unittest.skipUnless(shutil.which("cc"), "host C compiler unavailable")
    def test_motion_fade_threshold_and_palette(self):
        source = (ROOT / "src/overlays/room_m350/RoomEffect_FadingModel.c").read_text()
        source = source[:source.index('/* MASPSX_FLAGS: --expand-div */')]
        source = source.replace('asm("$2")', '').replace('asm("$4")', '')
        harness = '#include <assert.h>\n#include <stddef.h>\n#include <string.h>\n' + source + r'''
int D_800F3428;
unsigned short D_800F336C, D_800E1204[8];
short D_800F336A;
static Particle object;
static int lookups, draws, handleValue;
static short sizeValue, phaseValue, brightnessValue, scaleValue;
_Static_assert(sizeof(Particle) == 20, "particle size");
_Static_assert(offsetof(Particle, velocity) == 8, "velocity offset");
_Static_assert(offsetof(Particle, brightness) == 12, "brightness offset");
_Static_assert(offsetof(Particle, phase) == 16, "phase offset");
int GetClut(int x, int y) {
    assert(lookups++ == 0 && draws == 0 && x == 0);
    assert(y == D_800E1204[D_800F336C] +
           (D_800F336C == 4 && D_800F3428 ? 6 : 2));
    /* Stub mutations verify post-call reads, not real GetClut side effects. */
    object.size = sizeValue; object.phase = phaseValue;
    object.brightness = brightnessValue; D_800F336A = scaleValue;
    return handleValue;
}
void func_800CEE20(void *p, int zero, int a, int b, int scale,
                  unsigned int handle, int flag, int shade, void *colors) {
    assert(lookups == 1 && draws++ == 0 && p == &object && zero == 0);
    assert(a == sizeValue && b == sizeValue && scale == scaleValue * phaseValue);
    assert(handle == (unsigned short)handleValue && flag == 1);
    assert(shade == brightnessValue && colors == 0);
}
int main(void) {
    const short values[] = {-32768, -4097, -33, -1, 0, 32, 4097, 32767};
    const short counters[] = {-32768, -17, -1, 0, 1, 7, 8, 9, 15, 16, 17, 32767};
    int c, v, a, y, b, kind, special, h, s, m, i, event;
    Particle expected;
    for (c = 0; c < 12; ++c)
    for (v = 0; v < 8; ++v) for (a = 0; a < 8; ++a)
    for (y = 0; y < 8; ++y) for (b = 0; b < 8; ++b) {
        int phase = counters[c] >> 1;
        memset(&object, 0x5A, sizeof(object));
        object.counter = counters[c]; object.velocity = values[v];
        object.acceleration = values[a]; object.y = values[y];
        object.brightness = values[b]; expected = object;
        expected.phase = phase;
        if (phase < 8) {
            if (phase >= 4) expected.brightness = (short)(values[b] - 8);
            expected.counter = (short)(counters[c] + 1);
            expected.y = (short)(values[y] - (values[v] >> 1));
            expected.velocity = (short)(values[v] + values[a]);
        }
        lookups = draws = 0;
        assert(func_80199148(1, &object) == (phase >= 8));
        assert(memcmp(&object, &expected, sizeof(object)) == 0);
        assert(lookups == 0 && draws == 0);
    }
    for (event = -2; event <= 4; ++event) {
        if (event == 1 || event == 2) continue;
        lookups = draws = 0;
        assert(func_80199148(event, 0) == 0 && lookups == 0 && draws == 0);
    }
    for (kind = 0; kind < 8; ++kind) for (special = 0; special < 2; ++special)
    for (h = 0; h < 2; ++h) for (s = 0; s < 8; ++s) for (m = 0; m < 8; ++m) {
        for (i = 0; i < 8; ++i) D_800E1204[i] = h ? 65535 : i * 100;
        D_800F336C = kind; D_800F3428 = special;
        sizeValue = values[s]; phaseValue = values[m];
        brightnessValue = values[7-s]; scaleValue = values[7-m];
        handleValue = h ? -1 : 0x12345;
        memset(&object, 0, sizeof(object)); D_800F336A = 999;
        lookups = draws = 0;
        assert(func_80199148(2, &object) == 0);
        assert(lookups == 1 && draws == 1);
    }
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            exe = pathlib.Path(directory) / "room350-effect-fade"
            result = subprocess.run(
                ["cc", "-std=gnu11", "-O2", "-x", "c", "-", "-o", str(exe)],
                input=harness, text=True, capture_output=True,
            )
            self.assertEqual(result.returncode, 0, result.stderr)
            subprocess.run([str(exe)], check=True, timeout=10)
