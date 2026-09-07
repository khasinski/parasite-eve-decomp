import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]


class Room350EffectFallTests(unittest.TestCase):
    @unittest.skipUnless(shutil.which("cc"), "host C compiler unavailable")
    def test_motion_texture_and_post_lookup_state(self):
        self.check_variant("RoomEffect_FallingTextureCallback", "func_801937B4",
                           "D_8019A464", 8192, 1)

    @unittest.skipUnless(shutil.which("cc"), "host C compiler unavailable")
    def test_accelerated_motion_texture_and_post_lookup_state(self):
        self.check_variant("RoomEffect_AcceleratedFallCallback", "func_80197594",
                           "D_8019A62C", 6144, 2)

    def check_variant(self, filename, callback, colors, size, increment):
        if filename == "RoomEffect_FallingTextureCallback":
            source = (ROOT / "src/overlays/room_m350/RoomEffect_FallingTexturePair.c").read_text()
            source = source[:source.index('int func_801938E4')]
        else:
            source = (ROOT / "src/overlays/room_m350/RoomEffect_QueuedFall.c").read_text()
            source = source[:source.index('int func_801976C8')]
        source = source.replace('register int specialKind asm("$4");',
                                'int specialKind;')
        # D_800966EE is the high half of a record starting two bytes earlier.
        source = source.replace('extern short D_800966EE[];',
            'static union { int alignment; short values[8194]; } table;\n'
            '#define D_800966EE (table.values + 1)')
        definitions = (f"#define CALLBACK {callback}\n#define COLORS {colors}\n"
                       f"#define SIZE {size}\n#define INCREMENT {increment}\n")
        harness = '#include <assert.h>\n#include <string.h>\n' + definitions + source + r'''
int D_800E27EC, D_800F3428, COLORS[4];
unsigned short D_800F336C, D_800E1204[8];
short D_800F336A;
static short position[4];
static int lookups, draws, handleValue, nextCounter, expectedShade, expectedScale;
int GetClut(int mode, int texture) {
    assert(lookups++ == 0 && draws == 0 && mode == 0);
    assert(texture == D_800E1204[D_800F336C] +
           (D_800F336C == 4 && D_800F3428 ? 6 : 2));
    D_800E27EC = nextCounter;
    D_800F336A = expectedScale;
    return handleValue;
}
void func_800CEE20(void *point, int a, int b, int c, int scale,
                  unsigned int texture, int flag, int shade, void *colors) {
    assert(draws++ == 0 && lookups == 1 && point == position);
    assert(a == 0 && b == SIZE && c == SIZE && flag == 1);
    assert(scale == expectedScale * (((nextCounter - 1) >> 1) & 7));
    assert(texture == (unsigned short)handleValue && shade == expectedShade >> 5);
    assert(colors == COLORS);
}
int main(void) {
    const short values[] = {-32768, -33, -1, 0, 32, 32767};
    short before[4], expected[4];
    int counter, event, y, v, kind, special, h, s, i;
    assert(sizeof(SignedHalf) == 4);
    for (counter = -16; counter <= 32; ++counter)
    for (event = -1; event <= 3; ++event)
    for (y = 0; y < 6; ++y) for (v = 0; v < 6; ++v) {
        int result = 0;
        if (event == 2) continue;
        position[0] = 11; position[1] = values[y];
        position[2] = 22; position[3] = values[v];
        memcpy(expected, position, sizeof(position));
        if (event == 1) {
            if (counter >= 16) result = 1;
            else {
                expected[1] = (short)(position[1] - position[3]);
                expected[3] = (short)(position[3] + INCREMENT);
            }
        }
        D_800E27EC = counter; lookups = draws = 0;
        assert(CALLBACK(event, position) == result);
        assert(memcmp(position, expected, sizeof(position)) == 0);
        assert(lookups == 0 && draws == 0);
    }
    for (counter = 0; counter <= 32; ++counter)
    for (kind = 0; kind < 8; ++kind) for (special = 0; special < 2; ++special)
    for (h = 0; h < 2; ++h) for (s = 0; s < 6; ++s) {
        unsigned int index = ((unsigned int)counter * 256 & 0x3F00) / 2;
        for (i = 0; i < 8194; ++i) table.values[i] = 12345;
        for (i = 0; i < 8; ++i) D_800E1204[i] = h ? 65535 : i * 100;
        D_800E27EC = counter; nextCounter = counter + 1;
        D_800F336C = kind; D_800F3428 = special;
        D_800F336A = 999; expectedScale = values[s]; expectedShade = values[5-s];
        D_800966EE[index] = expectedShade;
        handleValue = h ? -1 : 0x12345;
        memcpy(before, position, sizeof(position));
        lookups = draws = 0;
        assert(CALLBACK(2, position) == 0);
        assert(lookups == 1 && draws == 1);
        assert(memcmp(position, before, sizeof(position)) == 0);
    }
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            exe = pathlib.Path(directory) / "room350-effect-fall"
            result = subprocess.run(
                ["cc", "-std=gnu11", "-O2", "-fno-strict-aliasing",
                 "-x", "c", "-", "-o", str(exe)],
                input=harness, text=True, capture_output=True,
            )
            self.assertEqual(result.returncode, 0, result.stderr)
            subprocess.run([str(exe)], check=True, timeout=10)
