import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]


class Room350EffectSineFadeTests(unittest.TestCase):
    @unittest.skipUnless(shutil.which("cc"), "host C compiler unavailable")
    def test_wave_fade_and_two_render_passes(self):
        source = (ROOT / "src/overlays/room_m350/RoomEffect_SineFadeCallback.c").read_text()
        source = source.replace(' asm("$4")', '').replace(' asm("$3")', '')
        harness = '#include <assert.h>\n#include <stddef.h>\n#include <stdint.h>\n#include <string.h>\n' + source + r'''
Actor *D_800F32D0;
int D_800E27EC, D_800F3428;
unsigned short D_800F336C, D_800E1204[65536];
int D_8019A738[4], D_8019A734[4];
static Actor actors[2];
static Instance instances[2];
static Particle particle, expected;
static int beforeFrame, afterWaveFrame, afterRenderFrame, waveResult, amplitudeAfterWave;
static int waves, cluts, firstDraws, secondDraws, kind, palette, special;
static short renderSize, renderBrightness;
static unsigned short handle;
_Static_assert(sizeof(Vector) == 8 && sizeof(Particle) == 24, "effect layout");
_Static_assert(offsetof(Particle, controls) == 8, "controls offset");
_Static_assert(offsetof(Particle, brightness) == 16, "brightness offset");
_Static_assert(offsetof(Particle, size) == 18, "size offset");
_Static_assert(offsetof(Particle, amplitude) == 20, "amplitude offset");
_Static_assert(offsetof(Actor, instance) == 8, "instance pointer offset");
_Static_assert(offsetof(Instance, worldY) == 0x200, "world height offset");
static int signedHalf(uint32_t value) {
    value &= 65535;
    return value < 32768 ? (int)value : (int)value - 65536;
}
static int signedWord(uint32_t value) {
    return value <= INT32_MAX ? (int)value : (int)((int64_t)value - 4294967296LL);
}
static int sizeReference(int wave, int amplitude) {
    uint32_t low = (uint32_t)((uint64_t)(uint32_t)wave * (uint32_t)amplitude);
    int64_t product = signedWord(low);
    int64_t shifted = product >= 0 ? product / 4096 : -((-product + 4095) / 4096);
    return signedHalf((uint32_t)shifted);
}
int rsin(int angle) {
    assert(waves++ == 0 && angle == signedWord((uint32_t)beforeFrame << 4));
    /* Mutations test post-helper reads, not actual rsin side effects. */
    D_800E27EC = afterWaveFrame;
    particle.amplitude = amplitudeAfterWave;
    return waveResult;
}
unsigned short GetClut(int x, int y) {
    int add = kind == 4 && special ? 7 : 3;
    assert(cluts++ == 0 && waves == 0 && firstDraws == 0 && x == 0 && y == palette + add);
    particle.size = renderSize; particle.brightness = renderBrightness;
    D_800F336C = 65535; D_800F3428 = 0;
    return handle;
}
void func_800CEE20(void *position, void *controls, int w, int h, int tile,
                  unsigned int clut, int flag, int brightness, void *colors) {
    assert(firstDraws++ == 0 && cluts == 1 && secondDraws == 0);
    assert(position == &particle && controls == particle.controls);
    assert(w == renderSize && h == renderSize && tile == 128 && clut == handle);
    assert(flag == 1 && brightness == renderBrightness && colors == 0);
    particle.position.x = -32768; particle.position.z = 32767;
    particle.size = -1234; particle.brightness = -99;
    D_800F32D0 = &actors[1]; D_800E27EC = afterRenderFrame;
    expected = particle;
}
void func_800D004C(void *p, int w, int h, int mode, void *t, int sx, int sy,
                  void *colorsA, void *colorsB, int brightness, int flag) {
    Vector *position = p, *texture = t;
    assert(secondDraws++ == 0 && firstDraws == 1);
    assert(position->x == -32768 && position->y == signedHalf(instances[1].worldY));
    assert(position->z == 32767);
    /* position.pad is deliberately uninitialized and must not be inspected. */
    assert(w == 384 && h == 384 && mode == 10 && sx == -1234 && sy == -1234);
    assert(texture->x == 1024 && texture->y == signedHalf((uint32_t)afterRenderFrame * 192));
    assert(texture->z == 0 && texture->pad == 1);
    assert(colorsA == D_8019A738 && colorsB == D_8019A734 && brightness == -99 && flag == 1);
}
static void reset(void) {
    waves = cluts = firstDraws = secondDraws = 0;
    memset(&particle, 0x5A, sizeof(particle));
    D_800E27EC = beforeFrame; D_800F32D0 = &actors[0];
    D_800F336C = kind; D_800F3428 = special; D_800E1204[kind] = palette;
}
static void runUpdate(int brightness) {
    int faded = afterWaveFrame >= 8 ? signedHalf((uint32_t)brightness - 8) : brightness;
    reset(); particle.brightness = brightness; expected = particle;
    expected.amplitude = amplitudeAfterWave;
    expected.size = sizeReference(waveResult, amplitudeAfterWave);
    expected.brightness = faded;
    assert(func_801996BC(1, &particle) == (afterWaveFrame >= 8 && faded < 16));
    assert(waves == 1 && cluts == 0 && firstDraws == 0 && secondDraws == 0);
    assert(memcmp(&particle, &expected, sizeof(particle)) == 0);
}
int main(void) {
    const int frames[] = {-1, 0, 7, 8, 9, 32768, INT32_MIN, INT32_MAX};
    const int waveValues[] = {-4096, -1, 0, 4096, INT32_MIN, INT32_MAX};
    const int kinds[] = {0, 3, 4, 5, 255, 32768, 65535};
    const int flags[] = {0, 1, -1};
    const int values[] = {-32768, -1, 0, 1, 15, 16, 23, 24, 128, 32767};
    const unsigned short rows[] = {0, 1, 255, 32768, 65535};
    int i, j, k, l, m, n, value, event;
    actors[0].instance = &instances[0]; actors[1].instance = &instances[1];
    instances[0].worldY = 123; instances[1].worldY = 100000;
    /* Every signed amplitude, including product wrap and negative arithmetic shifts. */
    for (i = 0; i < 6; ++i) for (value = -32768; value <= 32767; ++value) {
        beforeFrame = frames[i]; afterWaveFrame = 7; waveResult = waveValues[i];
        amplitudeAfterWave = value; runUpdate(128);
    }
    /* Every brightness for both sides of the frame threshold, including halfword wrap. */
    for (i = 0; i < 8; ++i) for (value = -32768; value <= 32767; ++value) {
        beforeFrame = frames[(i + 1) % 8]; afterWaveFrame = frames[i];
        waveResult = -4096; amplitudeAfterWave = 32767; runUpdate(value);
    }
    for (i = 0; i < 7; ++i) for (j = 0; j < 3; ++j) for (k = 0; k < 5; ++k)
    for (l = 0; l < 10; ++l) for (m = 0; m < 10; ++m) for (n = 0; n < 8; ++n) {
        kind = kinds[i]; special = flags[j]; palette = rows[k]; handle = rows[(k + 1) % 5];
        renderSize = values[l]; renderBrightness = values[m]; afterRenderFrame = frames[n];
        reset(); assert(func_801996BC(2, &particle) == 0);
        assert(waves == 0 && cluts == 1 && firstDraws == 1 && secondDraws == 1);
        assert(memcmp(&particle, &expected, sizeof(particle)) == 0);
    }
    for (event = -3; event <= 4; ++event) {
        if (event == 1 || event == 2) continue;
        reset(); expected = particle;
        assert(func_801996BC(event, &particle) == 0 && waves == 0 && cluts == 0);
        assert(memcmp(&particle, &expected, sizeof(particle)) == 0);
    }
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            exe = pathlib.Path(directory) / "room350-sine-fade"
            result = subprocess.run(
                ["cc", "-std=gnu11", "-O2", "-fwrapv", "-x", "c", "-", "-o", str(exe)],
                input=harness, text=True, capture_output=True,
            )
            self.assertEqual(result.returncode, 0, result.stderr)
            subprocess.run([str(exe)], check=True, timeout=20)
