import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]


class Room350EffectAimedImpactTests(unittest.TestCase):
    @unittest.skipUnless(shutil.which("cc"), "host C compiler unavailable")
    def test_spawn_angles_layout_and_helper_capture_order(self):
        source = (ROOT / "src/overlays/room_m350/RoomEffect_AimedImpactController.c").read_text()
        # Register allocation is verified by the MIPS build, not the host harness.
        source = source.replace(' asm("$4")', '').replace(' asm("$5")', '')
        harness = '#include <assert.h>\n#include <stddef.h>\n#include <stdint.h>\n#include <string.h>\n' + source + r'''
Emitter *D_800F33E0;
Actor *D_800F32D0;
Player *g_PlayerEntity;
unsigned char D_8019A86E, D_8019A855;
short D_8019A86A;
static Emitter emitter;
static Actor actors[3];
static Instance instances[3];
static Owner owners[3];
static Transform transforms[3];
static Player players[2];
static Particle particle;
static int pool, setups, allocations, roots, angles, randoms, rotations, applications, sounds;
static int setupResult, failAllocation, rng[2], angleResult;
static unsigned short yawAfterRandom;
static Vector *rotationInput;
static Matrix *rotationMatrix;
_Static_assert(sizeof(Particle) == 16, "particle size");
_Static_assert(offsetof(Particle, x) == 8, "particle position");
_Static_assert(offsetof(Particle, stopped) == 14, "particle stop flag");
_Static_assert(sizeof(Matrix) == 32, "matrix size");
_Static_assert(offsetof(Matrix, translation) == 20, "matrix translation");
_Static_assert(offsetof(Emitter, pool) == 8, "emitter pool");
_Static_assert(offsetof(Actor, instance) == 8, "actor instance");
_Static_assert(offsetof(Instance, owner) == 0, "instance owner");
_Static_assert(offsetof(Instance, yaw) == 0x3A, "instance yaw");
_Static_assert(offsetof(Instance, transform) == 0x238, "instance transform");
_Static_assert(offsetof(Transform, position) == 0xF4, "transform position");
_Static_assert(offsetof(Player, position) == 0x1FC, "player position");
_Static_assert(offsetof(Owner, soundMode) == 8, "owner sound mode");
static int signedByte(int x) { return x < 128 ? x : x - 256; }
static int signedWord(uint32_t x) {
    return x <= INT32_MAX ? (int)x : (int)((int64_t)x - 4294967296LL);
}
static short signedHalf(uint32_t x) {
    x &= 65535;
    return x < 32768 ? (short)x : (short)((int)x - 65536);
}
int func_80196F2C(int event, Particle *effect) { return 0; }
int func_800CE560(void *p, int size, int count, int (*callback)(int, Particle *)) {
    assert(setups++ == 0 && allocations == 0 && p == &pool);
    assert(size == 16 && count == 4 && callback == func_80196F2C);
    return setupResult;
}
Particle *func_800CE610(void *p) {
    assert(allocations++ == 0 && setups == 0 && p == &pool);
    /* Mutations probe captured versus reloaded values, not real helper side effects. */
    D_800F32D0 = &actors[1];
    g_PlayerEntity = &players[1];
    D_8019A855 = 165;
    D_8019A86E = 255;
    return failAllocation ? 0 : &particle;
}
int Math_IntSqrt(int value) {
    uint32_t dx = (uint32_t)transforms[1].position[0] - (uint32_t)players[1].position[0];
    uint32_t dz = (uint32_t)transforms[1].position[2] - (uint32_t)players[1].position[2];
    uint32_t sum = (uint32_t)((uint64_t)dx * dx) + (uint32_t)((uint64_t)dz * dz);
    assert(roots++ == 0 && allocations == 1 && value == signedWord(sum));
    assert(particle.x == signedHalf(transforms[1].position[0]));
    assert(particle.y == signedHalf(transforms[1].position[1]));
    assert(particle.z == signedHalf(transforms[1].position[2]));
    assert(particle.stopped == 0 && particle.pad == 0x5A);
    assert(particle.velocity.pad == 0x5A5A);
    return 12345;
}
int Gte_Atan2(int y, int horizontal) {
    uint32_t dy = (uint32_t)transforms[1].position[1] - (uint32_t)players[1].position[1];
    assert(angles++ == 0 && roots == 1 && y == signedWord(dy) && horizontal == 12345);
    return angleResult;
}
int Inv_ScrambleGrid(void) {
    assert(angles == 1 && randoms < 2);
    D_800F32D0 = &actors[2];
    instances[1].yaw = yawAfterRandom;
    return rng[randoms++];
}
Matrix *RotMatrixYXZ(Vector *v, Matrix *m) {
    unsigned pitch = ((uint32_t)angleResult + signedByte(rng[0]) / 2) & 4095;
    unsigned yaw = yawAfterRandom + signedByte(rng[1]) + 2048;
    if (pitch < 0xD00) pitch = 0xD00;
    assert(rotations++ == 0 && randoms == 2);
    assert(v->x == pitch && v->y == signedHalf(yaw) && v->z == 0);
    rotationInput = v; rotationMatrix = m;
    /* The unused pad must survive reuse of the same vector in the next phase. */
    v->pad = 1234;
    memset(m, 0x3C, sizeof(*m));
    return m;
}
Vector *ApplyMatrixSV(Matrix *m, Vector *v, Vector *out) {
    unsigned char expected[32];
    memset(expected, 0x3C, sizeof(expected));
    assert(applications++ == 0 && rotations == 1 && m == rotationMatrix && v == rotationInput);
    assert(memcmp(m, expected, sizeof(expected)) == 0);
    assert(v->x == 0 && v->y == 0 && v->z == 128 && v->pad == 1234);
    assert(out == &particle.velocity && D_8019A855 == 165);
    out->x = -3; out->y = 4; out->z = 5;
    particle.x = -32768; particle.y = 32767; particle.z = -123;
    D_800F32D0 = &actors[2];
    return out;
}
int Asset_Find08w(int id, int mode, short x, short y, short z) {
    assert(sounds++ == 0 && applications == 1 && id == 0x60A);
    assert(mode == owners[2].soundMode && x == -32768 && y == 32767 && z == -123);
    assert(D_8019A855 == 0 && D_8019A86A == -333);
    D_8019A855 = 77;
    D_8019A86A = -1;
    return -123;
}
static void reset(void) {
    setups = allocations = roots = angles = randoms = rotations = applications = sounds = 0;
    D_800F33E0 = &emitter; D_800F32D0 = &actors[0]; g_PlayerEntity = &players[0];
    D_8019A86E = 0; D_8019A855 = 1; D_8019A86A = -333;
    instances[1].yaw = 17;
    memset(&particle, 0x5A, sizeof(particle));
}
int main(void) {
    const int flags[] = {0, 1, 128, 255};
    const int pitches[] = {0, 1, 0xCFF, 0xD00, 0xFFF, -1, INT32_MIN, INT32_MAX};
    const int coordinates[] = {0, 1, -1, 32767, -32768, 100000, INT32_MIN, INT32_MAX};
    int i, j, k, stop, request, failure, event;
    emitter.pool = &pool;
    for (i = 0; i < 3; ++i) {
        actors[i].instance = &instances[i]; instances[i].owner = &owners[i];
        instances[i].transform = &transforms[i]; owners[i].soundMode = -1000 - i;
        instances[i].yaw = 555;
    }
    for (setupResult = -2; setupResult <= 2; ++setupResult) {
        reset(); assert(func_80197364(0) == setupResult && setups == 1 && allocations == 0);
    }
    for (stop = 0; stop < 4; ++stop) for (request = 0; request < 4; ++request)
    for (failure = 0; failure < 2; ++failure) {
        int active = !flags[stop] && flags[request];
        int spawned = active && !failure;
        reset(); D_8019A86E = flags[stop]; D_8019A855 = flags[request]; failAllocation = failure;
        assert(func_80197364(1) == (flags[stop] ? 2 : 0));
        assert(allocations == active && sounds == spawned && roots == spawned);
        assert(D_8019A855 == (spawned ? 77 : active ? 165 : flags[request]));
        assert(D_8019A86A == (spawned ? 8 : -333));
        if (!spawned) {
            unsigned char untouched[16]; memset(untouched, 0x5A, sizeof(untouched));
            assert(memcmp(&particle, untouched, sizeof(particle)) == 0);
        }
    }
    /* Exhaust every pair of RNG bytes across wrap/clamp and coordinate boundaries. */
    for (k = 0; k < 8; ++k) for (i = 0; i < 256; ++i) for (j = 0; j < 256; ++j) {
        int axis;
        reset(); failAllocation = 0; rng[0] = i; rng[1] = j; angleResult = pitches[k];
        yawAfterRandom = (unsigned short)(k * 8191 + j);
        for (axis = 0; axis < 3; ++axis) {
            transforms[1].position[axis] = coordinates[(k + axis) % 8];
            players[1].position[axis] = coordinates[(k + axis + 3) % 8];
        }
        assert(func_80197364(1) == 0);
        assert(allocations == 1 && randoms == 2 && roots == 1 && angles == 1);
        assert(rotations == 1 && applications == 1 && sounds == 1);
        assert(D_8019A855 == 77 && D_8019A86A == 8 && particle.stopped == 0);
        assert(particle.velocity.x == -3 && particle.velocity.y == 4 && particle.velocity.z == 5);
        assert(particle.velocity.pad == 0x5A5A && particle.pad == 0x5A);
    }
    for (event = -3; event <= 4; ++event) {
        if (event == 0 || event == 1) continue;
        reset(); assert(func_80197364(event) == 0 && setups == 0 && allocations == 0);
        assert(D_8019A855 == 1 && D_8019A86A == -333);
    }
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            exe = pathlib.Path(directory) / "room350-aimed-impact"
            result = subprocess.run(
                ["cc", "-std=gnu11", "-O2", "-fwrapv", "-x", "c", "-", "-o", str(exe)],
                input=harness, text=True, capture_output=True,
            )
            self.assertEqual(result.returncode, 0, result.stderr)
            subprocess.run([str(exe)], check=True, timeout=20)
