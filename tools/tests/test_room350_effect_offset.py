import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]


class Room350EffectOffsetTests(unittest.TestCase):
    @unittest.skipUnless(shutil.which("cc"), "host C compiler unavailable")
    def test_position_copy_offset_and_saved_frame(self):
        source = (ROOT / "src/overlays/room_m350/RoomEffect_OffsetPositionPaletteCallback.c").read_text()
        source = source.replace('asm("$3")', '')
        harness = '#include <assert.h>\n#include <string.h>\n' + source + r'''
int D_800E27EC, D_800F3428, D_800966EC[4096], D_8019A4E8[4];
unsigned short D_800F336C, D_800E1204[8];
short D_800F336A;
static Position original, expectedPosition, replacement;
static Effect object;
static int lookups, draws, handleValue, savedFrame, scaleValue, shadeValue;
_Static_assert(sizeof(Position) == 8, "position size");
int GetClut(int x, int y) {
    unsigned int index = ((unsigned int)savedFrame << 7) & 0xF80;
    assert(lookups++ == 0 && draws == 0 && x == 0);
    assert(y == D_800E1204[D_800F336C] +
           (D_800F336C == 4 && D_800F3428 ? 6 : 2));
    /* Stub mutations test captured values and reloads, not real helper effects. */
    original.x = 777; original.y = 888; original.z = 999; original.pad = 1000;
    object.position = &replacement; object.offset = 123;
    D_800E27EC += 3; D_800F336A = scaleValue;
    D_800966EC[index] = (int)(0x76540000U | (unsigned short)shadeValue);
    return handleValue;
}
void func_800CEE20(void *p, int zero, int a, int b, int scale,
                  unsigned int handle, int flag, int shade, void *colors) {
    assert(lookups == 1 && draws++ == 0 && p != &original && p != &replacement);
    assert(memcmp(p, &expectedPosition, sizeof(Position)) == 0);
    assert(zero == 0 && a == 8192 && b == 8192);
    assert(scale == scaleValue * (savedFrame >> 1));
    assert(handle == (unsigned short)handleValue && flag == 1);
    assert(shade == shadeValue >> 5 && colors == D_8019A4E8);
}
int main(void) {
    const short values[] = {-32768, -33, -1, 0, 32, 32767};
    int counter, event, kind, special, h, s, y, i;
    for (counter = -16; counter <= 80; ++counter)
    for (event = -1; event <= 3; ++event) for (s = 0; s < 6; ++s) {
        int offset = values[s];
        if (event == 2) continue;
        D_800E27EC = counter; lookups = draws = 0;
        object.position = &original; object.offset = offset;
        assert(func_80194F04(event, &object) == (event == 1 && counter >= 16));
        assert(object.position == &original);
        assert(object.offset == (short)(offset - (event == 1 && counter < 16 ? 16 : 0)));
        assert(lookups == 0 && draws == 0);
    }
    for (counter = -16; counter <= 80; ++counter)
    for (kind = 0; kind < 8; ++kind) for (special = 0; special < 2; ++special)
    for (h = 0; h < 2; ++h) for (s = 0; s < 6; ++s) for (y = 0; y < 6; ++y) {
        for (i = 0; i < 8; ++i) D_800E1204[i] = h ? 65535 : i * 100;
        memset(D_800966EC, 0, sizeof(D_800966EC));
        D_800E27EC = counter; savedFrame = counter - 1;
        D_800F336C = kind; D_800F3428 = special; D_800F336A = 999;
        scaleValue = values[s]; shadeValue = values[5-s];
        handleValue = h ? -1 : 0x12345;
        original.x = -111; original.y = values[y]; original.z = 222; original.pad = -333;
        expectedPosition = original;
        expectedPosition.y = (short)(values[y] + values[s]);
        object.position = &original; object.offset = values[s];
        lookups = draws = 0;
        assert(func_80194F04(2, &object) == 0);
        assert(lookups == 1 && draws == 1);
    }
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            exe = pathlib.Path(directory) / "room350-effect-offset"
            result = subprocess.run(
                ["cc", "-std=gnu11", "-O2", "-x", "c", "-", "-o", str(exe)],
                input=harness, text=True, capture_output=True,
            )
            self.assertEqual(result.returncode, 0, result.stderr)
            subprocess.run([str(exe)], check=True, timeout=10)
