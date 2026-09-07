import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]


class Room350EffectConfiguredPulseTests(unittest.TestCase):
    @unittest.skipUnless(shutil.which("cc"), "host C compiler unavailable")
    def test_configuration_and_pre_post_clut_samples(self):
        source = (ROOT / "src/overlays/room_m350/RoomEffect_ConfiguredPulseCallback.c").read_text()
        for register in (3, 4, 5, 6, 16):
            source = source.replace(f' asm("${register}")', '')
        # Preserve the retail aliases on the host: scale is at state+2 and
        # the two table names are low/high halfword views of one word table.
        source = source.replace('extern volatile short D_800F336A, ', 'extern volatile short ')
        source = source.replace(
            'extern short D_800966EC[], D_800966EE[];',
            'static union { int alignment; short halves[8192]; } tableStorage;\n'
            '#define D_800966EC (tableStorage.halves)\n'
            '#define D_800966EE (tableStorage.halves + 1)\n'
            '#define D_800F336A (D_800F3368.scale)',
        )
        harness = '#include <assert.h>\n#include <stddef.h>\n#include <stdint.h>\n#include <string.h>\n' + source + r'''
volatile int D_800E27EC;
volatile RenderState D_800F3368;
volatile short D_800F336E, D_800F3372, D_800F3374, D_800F3376, D_800F3378;
volatile unsigned short D_800F336C, D_800E11E8, D_800E1208;
unsigned short D_800F3370, D_800E2850[65536];
int D_8019A59C[4];
static unsigned char object[16];
static int cluts, draws, before, after, sample, shade, scale;
static unsigned short handle, paletteIndex, clutY;
_Static_assert(sizeof(RenderState) == 4, "render state prefix");
_Static_assert(offsetof(RenderState, scale) == 2, "scale alias");
_Static_assert(sizeof(SignedHalf) == 4, "signed high halfword view");
static unsigned tableIndex(int frame) { return (((uint32_t)frame - 1) << 9 & 0x3E00) / 2; }
static int signedHalf(int value) {
    unsigned v = (unsigned)value & 65535;
    return v < 32768 ? (int)v : (int)v - 65536;
}
unsigned short GetClut(int x, int y) {
    assert(cluts++ == 0 && draws == 0 && x == 0 && y == clutY);
    assert(D_800F3368.unit == 32 && D_800F336A == 2);
    assert(D_800F3376 == 32 && D_800F3378 == 32 && D_800F336C == 2);
    assert(D_800F336E == 0 && D_800F3372 == 0 && D_800F3374 == 32);
    assert(D_800F3370 == (unsigned short)(65535 - paletteIndex));
    /* These mutations test capture/reload order, not the actual helper's effects. */
    D_800E27EC = after;
    D_800F336A = scale;
    tableStorage.halves[tableIndex(before)] = 12345;
    tableStorage.halves[tableIndex(after) + 1] = shade;
    D_800E11E8 ^= 65535;
    D_800E1208 ^= 65535;
    return handle;
}
void func_800CEE20(void *position, int unused, int width, int height,
                  int tile, unsigned int clut, int flag, int brightness, void *colors) {
    unsigned phase = (((uint32_t)after - 1) >> 2) & 3;
    int expectedShade = shade >= 0 ? shade / 32 : -((-shade + 31) / 32);
    assert(draws++ == 0 && cluts == 1 && position == object && unused == 0);
    assert(width == signedHalf(sample * 2 + 4096) && height == width);
    assert(tile == scale * (int)phase + 192 && clut == handle && flag == 1);
    assert(brightness == expectedShade && colors == D_8019A59C);
}
static void reset(void) {
    cluts = draws = 0;
    D_800E27EC = before;
    D_800F3368.unit = -999; D_800F336A = -998;
    D_800F336E = -997; D_800F3372 = -996; D_800F3374 = -995;
    D_800F3376 = -994; D_800F3378 = -993; D_800F336C = 65535; D_800F3370 = 65534;
    D_800E11E8 = paletteIndex; D_800E1208 = clutY;
    D_800E2850[paletteIndex] = 65535 - paletteIndex;
    tableStorage.halves[tableIndex(before)] = sample;
    tableStorage.halves[tableIndex(after) + 1] = 23456;
}
int main(void) {
    const int frames[] = {-1, 0, 1, 4, 7, 8, 16, 31, 32, 33, INT32_MIN, INT32_MAX};
    const int samples[] = {-32768, -16384, -2048, -33, -1, 0, 1, 16384, 32767};
    const int scales[] = {-32768, -1, 0, 1, 2, 123, 32767};
    const unsigned short palettes[] = {0, 1, 255, 32768, 65535};
    int i, j, k, l, m, n, event;
    unsigned char untouched[16];
    memset(object, 0x5A, sizeof(object)); memset(untouched, 0x5A, sizeof(untouched));
    for (i = 0; i < 12; ++i) {
        before = frames[i]; reset();
        assert(func_80196C64(1, object) == (before >= 8));
        assert(cluts == 0 && draws == 0 && D_800F3368.unit == -999 && D_800F336A == -998);
    }
    for (i = 0; i < 12; ++i) for (j = 0; j < 12; ++j)
    for (k = 0; k < 9; ++k) for (l = 0; l < 9; ++l)
    for (m = 0; m < 7; ++m) for (n = 0; n < 5; ++n) {
        before = frames[i]; after = frames[j]; sample = samples[k]; shade = samples[l];
        scale = scales[m]; paletteIndex = palettes[n]; clutY = palettes[(n + 1) % 5];
        handle = palettes[(n + 2) % 5]; reset();
        assert(func_80196C64(2, object) == 0 && cluts == 1 && draws == 1);
        assert(memcmp(object, untouched, sizeof(object)) == 0);
    }
    for (event = -3; event <= 4; ++event) {
        if (event == 1 || event == 2) continue;
        reset();
        assert(func_80196C64(event, object) == 0 && cluts == 0 && draws == 0);
        assert(D_800F3368.unit == -999 && D_800F336A == -998);
        assert(D_800F3370 == 65534 && D_800F3376 == -994 && D_800F3378 == -993);
    }
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            exe = pathlib.Path(directory) / "room350-configured-pulse"
            result = subprocess.run(
                ["cc", "-std=gnu11", "-O2", "-fwrapv", "-fno-strict-aliasing",
                 "-x", "c", "-", "-o", str(exe)],
                input=harness, text=True, capture_output=True,
            )
            self.assertEqual(result.returncode, 0, result.stderr)
            subprocess.run([str(exe)], check=True, timeout=20)
