import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]


class Room350ProximityTriggerTests(unittest.TestCase):
    @unittest.skipUnless(shutil.which("cc"), "host C compiler unavailable")
    def test_bounds_flags_and_post_sqrt_pointers(self):
        source = (ROOT / "src/overlays/room_m350/RoomActor_TriggerWithinRadius.c").read_text()
        harness = '#include <assert.h>\n#include <string.h>\n' + source + r'''
WorldActor *g_PlayerEntity;
RoomActor **D_800F32D0;
short D_8019A86C, D_800942EC;
static WorldActor world[2];
static ActorFlags actors[2];
static RoomActor rooms[2];
static RoomActor *roomSlots[2][3];
static unsigned int roomFlags[2];
static int calls, expectedSquared, distanceResult, mutate;
int Math_IntSqrt(int squared) {
    assert(calls++ == 0 && squared == expectedSquared);
    if (mutate) {
        g_PlayerEntity = &world[1];
        D_800F32D0 = roomSlots[1];
        D_8019A86C = 77;
    }
    return distanceResult;
}
int main(void) {
    const int radii[] = {-1, 0, 1, 5, 10};
    const int offsets[] = {-11, -5, 0, 3, 4, 5, 11};
    short point[7] = {0, 0, 0, 0, 20, 0, -30};
    int gate, height, r, x, z, hasActor, m, delta, i;
    for (gate = 0; gate < 2; ++gate) for (height = -1; height <= 1; ++height)
    for (r = 0; r < 5; ++r) for (x = 0; x < 7; ++x) for (z = 0; z < 7; ++z)
    for (hasActor = 0; hasActor < 2; ++hasActor) for (m = 0; m < 2; ++m)
    for (delta = -1; delta <= 1; ++delta) {
        int dx = offsets[x], dz = offsets[z], expectedCalls, expectedResult;
        int active, expectedGate;
        for (i = 0; i < 2; ++i) {
            memset(&world[i], 0, sizeof(world[i]));
            memset(&actors[i], 0, sizeof(actors[i]));
            world[i].actor = &actors[i];
            actors[i].flags = 0x12340005U;
            roomFlags[i] = 0x00123456U;
            rooms[i].actor = hasActor ? &roomFlags[i] : 0;
            roomSlots[i][2] = &rooms[i];
        }
        g_PlayerEntity = &world[0]; D_800F32D0 = roomSlots[0];
        world[0].x = point[4] + dx; world[0].z = point[6] + dz;
        D_800942EC = 700; point[5] = 188 + height;
        D_8019A86C = gate ? 30 : 0;
        calls = 0; mutate = m; distanceResult = radii[r] + delta;
        expectedSquared = dx * dx + dz * dz;
        expectedCalls = !gate && height >= 0 &&
            (dx < 0 ? -dx : dx) <= radii[r] &&
            (dz < 0 ? -dz : dz) <= radii[r];
        expectedResult = expectedCalls && distanceResult <= radii[r];
        active = expectedCalls && mutate;
        expectedGate = expectedResult ? 30 : active ? 77 : gate ? 30 : 0;
        assert(func_8019721C(point, radii[r]) == expectedResult);
        assert(calls == expectedCalls && D_8019A86C == expectedGate);
        for (i = 0; i < 2; ++i) {
            int changed = expectedResult && i == active;
            assert(actors[i].flags == (0x12340005U | (changed ? 0x4000 : 0)));
            assert(roomFlags[i] == (0x00123456U |
                   (changed && hasActor ? 0x80000000U : 0)));
        }
    }
    D_8019A86C = -1; g_PlayerEntity = 0; D_800F32D0 = 0; calls = 0;
    assert(func_8019721C(0, 10) == 0 && calls == 0);
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            exe = pathlib.Path(directory) / "room350-proximity-trigger"
            result = subprocess.run(
                ["cc", "-std=gnu11", "-O2", "-x", "c", "-", "-o", str(exe)],
                input=harness, text=True, capture_output=True,
            )
            self.assertEqual(result.returncode, 0, result.stderr)
            subprocess.run([str(exe)], check=True, timeout=10)
