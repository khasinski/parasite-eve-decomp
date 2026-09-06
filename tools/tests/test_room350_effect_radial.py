import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]


class Room350EffectRadialTests(unittest.TestCase):
    @unittest.skipUnless(shutil.which("cc"), "host C compiler unavailable")
    def test_burst_allocation_random_vectors_and_configuration(self):
        source = (ROOT / "src/overlays/room_m350/RoomEffect_RadialBurstController.c").read_text()
        # Host tests check behavior; the MIPS build separately verifies register allocation.
        source = source.replace(' asm("$16")', '').replace(' asm("$18")', '')
        harness = '#include <assert.h>\n#include <stddef.h>\n#include <string.h>\n' + source + r'''
Emitter *D_800F33E0;
unsigned char D_8019A86E, D_8019A859;
unsigned short D_800E11E8, D_800E2850[65536];
short D_800F3368, D_800F336A, D_800F336E, D_800F3372, D_800F3374;
unsigned short D_800F336C, D_800F3370;
volatile short D_800F3376, D_800F3378;
static Emitter emitters[2];
static int pools[2], allocations, draws, rotations, randomCalls, setups;
static int failAt, requestAfterAllocation, setupResult, randomValues[8];
static Vector effects[2];
_Static_assert(offsetof(Emitter, pool) == 8, "pool offset");
_Static_assert(sizeof(Matrix) == 32, "matrix size");
_Static_assert(offsetof(Matrix, translation) == 20, "translation offset");
_Static_assert(sizeof(Vector) == 8, "vector size");
int func_80197A04(int event, short *object) { return 0; }
int func_800CE560(void *pool, int a, int b, int (*callback)(int, short *)) {
    assert(setups++ == 0 && allocations == 0 && pool == &pools[0]);
    assert(a == 8 && b == 16 && callback == func_80197A04);
    return setupResult;
}
short *func_800CE610(void *pool) {
    assert(setups == 0 && allocations < 2);
    assert(pool == &pools[allocations & 1]);
    /* Mutations probe reload order, not the real allocator's side effects. */
    D_800F33E0 = &emitters[1];
    D_8019A859 = requestAfterAllocation;
    D_8019A86E = 255;
    allocations++;
    if (allocations == failAt) return 0;
    return (short *)&effects[allocations - 1];
}
int Inv_ScrambleGrid(void) {
    assert(randomCalls < 8);
    return randomValues[randomCalls++];
}
Matrix *RotMatrixYXZ(Vector *vector, Matrix *matrix) {
    assert(rotations == draws && randomCalls == draws * 4 + 2);
    assert(vector->x == (short)(randomValues[draws * 4] * 16));
    assert(vector->y == (short)(randomValues[draws * 4 + 1] * 16));
    assert(vector->z == 0);
    /* The unused pad is not initialized; it must survive the second phase. */
    vector->pad = 1234;
    memset(matrix, 0x5A, sizeof(*matrix));
    rotations++;
    return matrix;
}
Vector *ApplyMatrixSV(Matrix *matrix, Vector *vector, Vector *effect) {
    unsigned char expectedMatrix[32];
    int combined = randomValues[draws * 4 + 2] * 256 | randomValues[draws * 4 + 3];
    memset(expectedMatrix, 0x5A, sizeof(expectedMatrix));
    assert(memcmp(matrix, expectedMatrix, sizeof(expectedMatrix)) == 0);
    assert(rotations == draws + 1 && randomCalls == (draws + 1) * 4);
    assert(vector->x == 0 && vector->y == 0 && vector->z == combined % 64 + 256);
    assert(vector->pad == 1234 && effect == &effects[draws]);
    effect->x = 100 + draws; effect->y = -200; effect->z = vector->z;
    draws++;
    return effect;
}
static void reset(void) {
    setups = allocations = draws = rotations = randomCalls = 0;
    D_800F33E0 = &emitters[0];
}
int main(void) {
    const int values[] = {0, 1, 63, 64, 127, 128, 254, 255};
    const int flags[] = {0, 1, 128, 255};
    const unsigned short indices[] = {0, 1, 255, 32768, 65535};
    int seed, stop, request, failure, mutation, i, event;
    emitters[0].pool = &pools[0]; emitters[1].pool = &pools[1];
    for (setupResult = -2; setupResult <= 2; ++setupResult) {
        reset();
        assert(func_80197B98(0) == setupResult && setups == 1);
    }
    for (seed = 0; seed < 64; ++seed)
    for (stop = 0; stop < 4; ++stop) for (request = 0; request < 4; ++request)
    for (failure = 0; failure < 3; ++failure) for (mutation = 0; mutation < 4; ++mutation) {
        int active = !flags[stop] && flags[request];
        int expectedDraws = !active ? 0 : failure == 1 ? 0 : failure == 2 ? 1 : 2;
        reset(); failAt = failure; requestAfterAllocation = flags[mutation];
        D_8019A86E = flags[stop]; D_8019A859 = flags[request];
        for (i = 0; i < 8; ++i) randomValues[i] = values[(seed + i * (seed / 8 + 1)) % 8];
        assert(func_80197B98(1) == (flags[stop] ? 2 : 0));
        assert(allocations == (active ? failure == 1 ? 1 : 2 : 0));
        assert(draws == expectedDraws && rotations == draws && randomCalls == draws * 4);
        assert(D_8019A859 == (active ? (unsigned char)(requestAfterAllocation - 1) : flags[request]));
    }
    for (i = 0; i < 5; ++i) {
        reset(); D_800E11E8 = indices[i]; D_800E2850[indices[i]] = 65535 - i;
        assert(func_80197B98(2) == 0 && allocations == 0 && setups == 0);
        assert(D_800F3372 == 0 && D_800F3368 == 16 && D_800F336A == 1);
        assert(D_800F3376 == 16 && D_800F3378 == 16 && D_800F336C == 2);
        assert(D_800F336E == 0 && D_800F3374 == 0 && D_800F3370 == 65535 - i);
    }
    for (event = -3; event <= 4; ++event) {
        if (event >= 0 && event <= 2) continue;
        reset(); D_8019A859 = 255;
        assert(func_80197B98(event) == 0 && D_8019A859 == 255);
        assert(allocations == 0 && draws == 0 && setups == 0);
    }
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            exe = pathlib.Path(directory) / "room350-effect-radial"
            result = subprocess.run(
                ["cc", "-std=gnu11", "-O2", "-x", "c", "-", "-o", str(exe)],
                input=harness, text=True, capture_output=True,
            )
            self.assertEqual(result.returncode, 0, result.stderr)
            subprocess.run([str(exe)], check=True, timeout=10)
