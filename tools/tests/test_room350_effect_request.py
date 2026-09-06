import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]


class Room350EffectRequestTests(unittest.TestCase):
    @unittest.skipUnless(shutil.which("cc"), "host C compiler unavailable")
    def test_request_lifecycle_and_render_configuration(self):
        source = (ROOT / "src/overlays/room_m350/RoomEffect_QueuedFallController.c").read_text()
        harness = '#include <assert.h>\n#include <string.h>\n' + source + r'''
Emitter *D_800F33E0;
unsigned char D_8019A86E, D_8019A857;
short D_8019A864, D_8019A866, D_8019A868;
unsigned short D_800E11FA, D_800E2850[65536];
short D_800F3368, D_800F336A, D_800F336E, D_800F3372, D_800F3374;
unsigned short D_800F336C, D_800F3370;
volatile short D_800F3376, D_800F3378;
static Emitter emitter;
static int poolToken, setups, allocations, failAllocation, setupResult;
static short effect[4], xValue, yValue, zValue;
int func_80197594(int event, short *point) { return 0; }
int func_800CE560(void *pool, int a, int b, int (*callback)(int, short *)) {
    assert(setups++ == 0 && allocations == 0 && pool == &poolToken);
    assert(a == 8 && b == 4 && callback == func_80197594);
    return setupResult;
}
short *func_800CE610(void *pool) {
    assert(allocations++ == 0 && setups == 0 && pool == &poolToken);
    assert(D_8019A857 != 0 && D_8019A86E == 0);
    /* Stub mutations verify that coordinates are fetched after allocation. */
    D_8019A864 = xValue; D_8019A866 = yValue; D_8019A868 = zValue;
    return failAllocation ? 0 : effect;
}
int main(void) {
    const unsigned char flags[] = {0, 1, 128, 255};
    const short values[] = {-32768, -33, -1, 0, 32, 32767};
    const unsigned short indices[] = {0, 1, 255, 32768, 65535};
    short before[4];
    int event, stop, request, fail, x, y, z, i, ret;
    emitter.pool = &poolToken; D_800F33E0 = &emitter;
    for (ret = -2; ret <= 2; ++ret) {
        setups = allocations = 0; setupResult = ret;
        D_8019A857 = 128; D_8019A86E = 255;
        assert(func_801976C8(0) == ret && setups == 1 && allocations == 0);
        assert(D_8019A857 == 128 && D_8019A86E == 255);
    }
    for (stop = 0; stop < 4; ++stop) for (request = 0; request < 4; ++request)
    for (fail = 0; fail < 2; ++fail)
    for (x = 0; x < 6; ++x) for (y = 0; y < 6; ++y) for (z = 0; z < 6; ++z) {
        int allocate = flags[stop] == 0 && flags[request] != 0;
        int emitted = allocate && !fail;
        D_8019A86E = flags[stop]; D_8019A857 = flags[request];
        D_8019A864 = 123; D_8019A866 = 456; D_8019A868 = 789;
        xValue = values[x]; yValue = values[y]; zValue = values[z];
        memset(effect, 0x5A, sizeof(effect)); memcpy(before, effect, sizeof(effect));
        setups = allocations = 0; failAllocation = fail;
        assert(func_801976C8(1) == (flags[stop] ? 2 : 0));
        assert(allocations == allocate && setups == 0 && D_8019A86E == flags[stop]);
        assert(D_8019A857 == (emitted ? 0 : flags[request]));
        if (emitted) {
            assert(effect[0] == xValue && effect[1] == yValue && effect[2] == zValue && effect[3] == 0);
        } else assert(memcmp(effect, before, sizeof(effect)) == 0);
    }
    for (i = 0; i < 65536; ++i) D_800E2850[i] = (unsigned short)(i * 13 + 17);
    for (i = 0; i < 5; ++i) {
        D_800E11FA = indices[i]; setups = allocations = 0;
        D_8019A857 = 128; D_8019A86E = 255;
        D_800F3368 = D_800F336A = D_800F336E = D_800F3372 = D_800F3374 = -7;
        D_800F336C = D_800F3370 = 65535; D_800F3376 = D_800F3378 = -7;
        assert(func_801976C8(2) == 0 && setups == 0 && allocations == 0);
        assert(D_800F3368 == 32 && D_800F336A == 2 && D_800F336C == 3 && D_800F336E == 1);
        assert(D_800F3376 == 32 && D_800F3378 == 32 && D_800F3372 == 0 && D_800F3374 == 0);
        assert(D_800F3370 == D_800E2850[indices[i]]);
        assert(D_8019A857 == 128 && D_8019A86E == 255);
    }
    for (event = -2; event <= 4; ++event) {
        if (event >= 0 && event <= 2) continue;
        setups = allocations = 0;
        assert(func_801976C8(event) == 0 && setups == 0 && allocations == 0);
    }
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            exe = pathlib.Path(directory) / "room350-effect-request"
            result = subprocess.run(
                ["cc", "-std=gnu11", "-O2", "-x", "c", "-", "-o", str(exe)],
                input=harness, text=True, capture_output=True,
            )
            self.assertEqual(result.returncode, 0, result.stderr)
            subprocess.run([str(exe)], check=True, timeout=10)
