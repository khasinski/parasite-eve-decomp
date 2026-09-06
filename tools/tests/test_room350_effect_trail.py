import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]


class Room350EffectTrailTests(unittest.TestCase):
    @unittest.skipUnless(shutil.which("cc"), "host C compiler unavailable")
    def test_queue_consumption_reloads_and_configuration(self):
        source = (ROOT / "src/overlays/room_m350/RoomEffect_TrailQueueController.c").read_text()
        harness = '#include <assert.h>\n#include <stddef.h>\n#include <string.h>\n' + source + r'''
Emitter *D_800F33E0;
unsigned char D_8019A86E;
short D_8019A85C;
Vector g_RoomEffectTrailPositions[4];
unsigned short D_800E11E8, D_800E2850[65536];
short D_800F3368, D_800F336A, D_800F336E, D_800F3372, D_800F3374;
unsigned short D_800F336C, D_800F3370;
volatile short D_800F3376, D_800F3378;
static Emitter emitters[2];
static int pools[2], setups, allocations, successes, failAt, setupResult, replacementCount;
static short effects[4][8], coordinates[4][3];
_Static_assert(offsetof(Emitter, pool) == 8, "pool offset");
_Static_assert(sizeof(Vector) == 8, "queue stride");
_Static_assert(offsetof(Vector, y) == 2 && offsetof(Vector, z) == 4, "coordinate offsets");
int func_8019A014(int event, short *object) { return 0; }
int func_800CE560(void *pool, int a, int b, int (*callback)(int, short *)) {
    assert(setups++ == 0 && allocations == 0 && pool == &pools[0]);
    assert(a == 8 && b == 16 && callback == func_8019A014);
    return setupResult;
}
short *func_800CE610(void *pool) {
    int slot = allocations++;
    assert(setups == 0 && slot < 4 && pool == &pools[slot ? 1 : 0]);
    /* Stub changes probe reloads; they do not describe actual allocator behavior. */
    D_800F33E0 = &emitters[1];
    D_8019A86E = 255;
    if (replacementCount >= 0) D_8019A85C = replacementCount;
    if (allocations == failAt) return 0;
    g_RoomEffectTrailPositions[slot].x = coordinates[slot][0];
    g_RoomEffectTrailPositions[slot].y = coordinates[slot][1];
    g_RoomEffectTrailPositions[slot].z = coordinates[slot][2];
    successes++;
    return effects[slot];
}
static void reset(void) {
    setups = allocations = successes = 0;
    D_800F33E0 = &emitters[0];
    memset(effects, 0x6B, sizeof(effects));
    memset(g_RoomEffectTrailPositions, 0x5A, sizeof(g_RoomEffectTrailPositions));
}
int main(void) {
    const short values[] = {-32768, -4097, -1, 0, 1, 4097, 32767};
    const int flags[] = {0, 1, 128, 255};
    const unsigned short indices[] = {0, 1, 255, 32768, 65535};
    int count, stop, failure, replacement, seed, i, j, event;
    emitters[0].pool = &pools[0]; emitters[1].pool = &pools[1];
    for (setupResult = -2; setupResult <= 2; ++setupResult) {
        reset();
        assert(func_8019784C(0) == setupResult && setups == 1);
    }
    for (count = -2; count <= 4; ++count) for (stop = 0; stop < 4; ++stop)
    for (failure = 0; failure <= 4; ++failure)
    for (replacement = -1; replacement <= 4; ++replacement) for (seed = 0; seed < 7; ++seed) {
        int expectedAllocations = 0, expectedSuccesses = 0;
        int currentCount = count;
        if (!flags[stop]) {
            for (i = 0; i < currentCount; ++i) {
                expectedAllocations++;
                if (replacement >= 0) currentCount = replacement;
                if (expectedAllocations == failure) break;
                expectedSuccesses++;
            }
        }
        reset(); failAt = failure; replacementCount = replacement;
        D_8019A85C = count; D_8019A86E = flags[stop];
        for (i = 0; i < 4; ++i) for (j = 0; j < 3; ++j)
            coordinates[i][j] = values[(seed + i + j) % 7];
        assert(func_8019784C(1) == (flags[stop] ? 2 : 0));
        assert(allocations == expectedAllocations && successes == expectedSuccesses);
        assert(D_8019A85C == (flags[stop] ? count : 0));
        for (i = 0; i < 4; ++i) for (j = 0; j < 8; ++j)
            assert(effects[i][j] == (i < successes && j < 3 ? coordinates[i][j] : 0x6B6B));
    }
    for (i = 0; i < 5; ++i) {
        reset(); D_800E11E8 = indices[i]; D_800E2850[indices[i]] = 65535 - i;
        assert(func_8019784C(2) == 0 && allocations == 0 && setups == 0);
        assert(D_800F3368 == 16 && D_800F336A == 1);
        assert(D_800F3376 == 16 && D_800F3378 == 16 && D_800F336C == 2);
        assert(D_800F336E == 0 && D_800F3372 == 0 && D_800F3374 == 0);
        assert(D_800F3370 == 65535 - i);
    }
    for (event = -3; event <= 4; ++event) {
        if (event >= 0 && event <= 2) continue;
        reset(); D_8019A85C = -2;
        assert(func_8019784C(event) == 0 && D_8019A85C == -2);
        assert(allocations == 0 && setups == 0);
    }
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            exe = pathlib.Path(directory) / "room350-effect-trail"
            result = subprocess.run(
                ["cc", "-std=gnu11", "-O2", "-x", "c", "-", "-o", str(exe)],
                input=harness, text=True, capture_output=True,
            )
            self.assertEqual(result.returncode, 0, result.stderr)
            subprocess.run([str(exe)], check=True, timeout=10)
