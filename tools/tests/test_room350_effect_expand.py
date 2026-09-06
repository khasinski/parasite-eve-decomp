import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]


class Room350EffectExpandTests(unittest.TestCase):
    @unittest.skipUnless(shutil.which("cc"), "host C compiler unavailable")
    def test_expansion_and_post_clut_reloads(self):
        source = (ROOT / "src/overlays/room_m350/RoomEffect_ExpandingActorOffsetCallback.c").read_text()
        harness = '#include <assert.h>\n#include <stddef.h>\n#include <stdint.h>\n#include <string.h>\n' + source + r'''
Actor *D_800F32D0;
int D_800E27EC, D_800F3428, D_800966EC[4096], D_8019A634[4];
unsigned short D_800F336C, D_800E1204[8];
short D_800F336A;
static Actor actor;
static Instance instance;
static Transform transform;
static short object[3], expectedPosition[3];
static int lookups, draws, handleValue, expectedShade, renderScale;
_Static_assert(offsetof(Actor, instance) == 8, "actor instance offset");
_Static_assert(offsetof(Instance, transform) == 0x238, "transform pointer offset");
_Static_assert(offsetof(Transform, position) == 0xF4, "origin offset");
int GetClut(int x, int y) {
    unsigned int index;
    assert(lookups++ == 0 && draws == 0 && x == 32);
    assert(y == D_800E1204[D_800F336C] +
           (D_800F336C == 4 && D_800F3428 ? 4 : 0));
    /* Stub mutations distinguish captured coordinates from later global reads. */
    memset(transform.position, 0, sizeof(transform.position));
    memset(D_800966EC, 0, sizeof(D_800966EC));
    object[0] = 123; object[1] = 456; object[2] = 789;
    D_800F336A = renderScale; D_800E27EC += 3;
    index = ((unsigned int)(D_800E27EC - 1) << 7) & 0xF80;
    D_800966EC[index] = (int)(0x12340000u | (unsigned short)expectedShade);
    return handleValue;
}
void func_800CEE20(void *p, int zero, int a, int b, int scale,
                  unsigned int handle, int flag, int shade, void *colors) {
    assert(lookups == 1 && draws++ == 0 && p != object && zero == 0);
    /* Retail does not initialize the fourth local halfword. */
    assert(memcmp(p, expectedPosition, sizeof(expectedPosition)) == 0);
    assert(a == 5120 && b == 5120 && scale == renderScale * 2 + 216);
    assert(handle == (unsigned short)handleValue && flag == 1);
    assert(shade == (expectedShade >> 5) && colors == D_8019A634);
}
int main(void) {
    const short values[] = {-32768, -4097, -33, -1, 0, 32, 4097, 32767};
    int counter, event, kind, special, h, s, v, i;
    actor.instance = &instance; instance.transform = &transform; D_800F32D0 = &actor;
    for (counter = -16; counter <= 80; ++counter)
    for (event = -1; event <= 3; ++event) {
        if (event == 2) continue;
        D_800E27EC = counter; lookups = draws = 0;
        assert(func_80197A04(event, 0) == (event == 1 && counter >= 8));
        assert(lookups == 0 && draws == 0);
    }
    for (counter = -16; counter <= 80; ++counter)
    for (kind = 0; kind < 8; ++kind) for (special = 0; special < 2; ++special)
    for (h = 0; h < 2; ++h) for (s = 0; s < 8; ++s) for (v = 0; v < 8; ++v) {
        unsigned int index = ((unsigned int)(counter - 1) << 7) & 0xF80;
        for (i = 0; i < 8; ++i) D_800E1204[i] = h ? 65535 : i * 100;
        memset(D_800966EC, 0, sizeof(D_800966EC));
        D_800E27EC = counter; D_800F336C = kind; D_800F3428 = special;
        D_800966EC[index] = (int)(0x56780000u | (unsigned short)values[s]);
        expectedShade = values[7-s];
        for (i = 0; i < 3; ++i) {
            int64_t product;
            uint32_t low;
            object[i] = values[(v + i) % 8];
            transform.position[i] = (i - 1) * 100000 + counter;
            /* Independently model MULT's low 32 bits, including overflow. */
            low = (uint32_t)((int64_t)object[i] * (4096 - values[s]) * 2);
            product = low <= INT32_MAX ? (int64_t)low : (int64_t)low - 4294967296LL;
            expectedPosition[i] = (short)(product / 4096 + transform.position[i]);
        }
        D_800F336A = 999; renderScale = values[7-v];
        handleValue = h ? -1 : 0x12345;
        lookups = draws = 0;
        assert(func_80197A04(2, object) == 0);
        assert(lookups == 1 && draws == 1);
    }
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            exe = pathlib.Path(directory) / "room350-effect-expand"
            result = subprocess.run(
                ["cc", "-std=gnu11", "-O2", "-fwrapv", "-fno-strict-aliasing",
                 "-x", "c", "-", "-o", str(exe)],
                input=harness, text=True, capture_output=True,
            )
            self.assertEqual(result.returncode, 0, result.stderr)
            subprocess.run([str(exe)], check=True, timeout=10)
