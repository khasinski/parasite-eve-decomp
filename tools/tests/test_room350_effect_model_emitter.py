import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]
SOURCE = ROOT / "src/overlays/room_m350/RoomEffect_ModelTransformEmitter.c"


class Room350EffectModelEmitterTests(unittest.TestCase):
    @unittest.skipUnless(
        (ROOT / "tools/old-gcc/cc1").is_file() and shutil.which("mipsel-none-elf-as"),
        "native PSX compiler/assembler unavailable",
    )
    def test_target_structure_offsets(self):
        declarations = SOURCE.read_text().split("extern Emitter", 1)[0]
        checks = r'''
#define OFFSET(type, field) ((unsigned long)&((type *)0)->field)
typedef char particle_size[sizeof(Particle) == 24 ? 1 : -1];
typedef char particle_controls[OFFSET(Particle, controls) == 8 ? 1 : -1];
typedef char particle_amplitude[OFFSET(Particle, amplitude) == 20 ? 1 : -1];
typedef char state_size[sizeof(EmissionState) == 4 ? 1 : -1];
typedef char model_count[OFFSET(Model, transformCount) == 2 ? 1 : -1];
typedef char transform_size[sizeof(Transform) == 32 ? 1 : -1];
typedef char transform_position[OFFSET(Transform, position) == 20 ? 1 : -1];
typedef char instance_model[OFFSET(Instance, model) == 0x1B4 ? 1 : -1];
typedef char instance_transforms[OFFSET(Instance, transforms) == 0x238 ? 1 : -1];
typedef char actor_instance[OFFSET(Actor, instance) == 8 ? 1 : -1];
typedef char emitter_pool[OFFSET(Emitter, pool) == 8 ? 1 : -1];
'''
        with tempfile.TemporaryDirectory() as directory:
            path = pathlib.Path(directory) / "layout.c"
            path.write_text(declarations + checks)
            result = subprocess.run(
                [str(ROOT / "tools/scripts/cc.sh"), str(path), str(path.with_suffix(".o"))],
                cwd=ROOT, text=True, capture_output=True,
            )
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)

    @unittest.skipUnless(shutil.which("cc"), "host C compiler unavailable")
    def test_transform_selection_timer_and_initialization(self):
        source = SOURCE.read_text()
        for register in (2, 3, 4, 5, 6):
            source = source.replace(f' asm("${register}")', '')
        harness = '#include <assert.h>\n#include <stddef.h>\n#include <stdint.h>\n#include <string.h>\n' + source + r'''
Emitter *D_800F33E0;
Actor *D_800F32D0;
unsigned char D_8019A8C4;
volatile unsigned short D_800E11FA;
unsigned short D_800E2850[65536];
volatile short D_800F3368, D_800F336A, D_800F336E, D_800F3372, D_800F3374, D_800F3376, D_800F3378;
volatile unsigned short D_800F336C, D_800F3370;
static Emitter emitter;
static Actor actors[2];
static Instance instances[2];
static Model models[2];
static Transform transforms[2][256];
static EmissionState state;
static Particle effect, expected;
static int pool, setups, allocations, tableRandoms, controlRandoms, failAllocation, setupResult;
static int tableRandom, controlRandom, countAfterRandom;
_Static_assert(sizeof(Particle) == 24 && sizeof(EmissionState) == 4, "payload sizes");
_Static_assert(sizeof(Transform) == 32 && offsetof(Transform, position) == 20, "transform layout");
/* Instance contains 64-bit pointers on this host. Its PSX offsets are tested separately. */
static int signedHalf(uint32_t value) {
    value &= 65535;
    return value < 32768 ? (int)value : (int)value - 65536;
}
static int selectedIndex(void) {
    int product = (tableRandom & 255) * ((int)models[1].transformCount - 1);
    int quotient = product >= 0 ? product / 256 : -((-product) / 256);
    return quotient + 1;
}
int func_801996BC(int event, Particle *particle) { return 0; }
int func_800CE560(void *p, int stride, int count, int (*callback)(int, Particle *)) {
    assert(setups++ == 0 && allocations == 0 && p == &pool);
    assert(stride == 24 && count == 8 && callback == func_801996BC);
    assert(state.count == 0 && state.delay == 0);
    return setupResult;
}
Particle *func_800CE610(void *p) {
    assert(allocations++ == 0 && setups == 0 && p == &pool && state.delay == -1);
    return failAllocation ? 0 : &effect;
}
int Inv_ScrambleGrid(void) {
    assert(tableRandoms++ == 0 && allocations == 1 && controlRandoms == 0);
    /* Probe post-helper reloads, without asserting these are the real helper's effects. */
    D_800F32D0 = &actors[1];
    state.count = countAfterRandom;
    return tableRandom;
}
int rand(void) {
    int index = selectedIndex();
    assert(controlRandoms++ == 0 && tableRandoms == 1);
    assert(index >= 1 && index <= 255);
    assert(effect.position.x == signedHalf(transforms[1][index].position[0]));
    assert(effect.position.y == signedHalf(transforms[1][index].position[1]));
    assert(effect.position.z == signedHalf(transforms[1][index].position[2]));
    assert(effect.brightness == 128 && effect.amplitude == signedHalf((countAfterRandom + 1) * 2048));
    assert(state.count == signedHalf((uint32_t)countAfterRandom + 1));
    assert(state.delay == (tableRandom & 7) + 12);
    state.count = -123; state.delay = -456; D_8019A8C4 = 165;
    return controlRandom;
}
static void reset(void) {
    setups = allocations = tableRandoms = controlRandoms = 0;
    D_800F33E0 = &emitter; D_800F32D0 = &actors[0]; D_8019A8C4 = 90;
    state.count = 0; state.delay = 0;
    memset(&effect, 0x5A, sizeof(effect));
}
static void verifySpawn(void) {
    int index = selectedIndex();
    int high = ((uint32_t)controlRandom >> 8) & 255;
    memset(&expected, 0x5A, sizeof(expected));
    expected.position.x = signedHalf(transforms[1][index].position[0]);
    expected.position.y = signedHalf(transforms[1][index].position[1]);
    expected.position.z = signedHalf(transforms[1][index].position[2]);
    expected.brightness = 128;
    expected.amplitude = signedHalf((countAfterRandom + 1) * 2048);
    expected.controls[0] = (controlRandom & 255) + 512;
    expected.controls[1] = high + ((controlRandom & 2) ? 768 : -1024);
    expected.controls[2] = 0; expected.controls[3] = 0;
    assert(memcmp(&effect, &expected, sizeof(effect)) == 0);
    assert(allocations == 1 && tableRandoms == 1 && controlRandoms == 1);
    assert(state.count == -123 && state.delay == -456 && D_8019A8C4 == 165);
}
int main(void) {
    const int counts[] = {-32768, -1, 0, 1, 23, 24, 32767};
    const int delays[] = {-32768, -1, 0, 1, 32767};
    const int controls[] = {0, 1, 2, 3, 255, 256, 32767, INT32_MIN, -1};
    const unsigned short indices[] = {0, 1, 255, 32768, 65535};
    int i, j, k, m, failure, value, event;
    emitter.pool = &pool;
    for (i = 0; i < 2; ++i) {
        actors[i].instance = &instances[i]; instances[i].model = &models[i];
        instances[i].transforms = transforms[i]; models[i].transformCount = 255;
        for (j = 0; j < 256; ++j) for (k = 0; k < 3; ++k)
            transforms[i][j].position[k] = (i ? -1 : 1) * (j * 1001 + k * 45678 + 12345);
    }
    for (setupResult = -2; setupResult <= 2; ++setupResult) {
        reset(); state.count = 123; state.delay = -123;
        assert(func_8019988C(0, &state) == setupResult && setups == 1 && allocations == 0);
    }
    /* Every delay bit pattern: a failed emission leaves -1, not a new delay. */
    for (value = -32768; value <= 32767; ++value) {
        reset(); state.delay = value; failAllocation = 1; expected = effect;
        assert(func_8019988C(1, &state) == 0);
        assert(state.delay == signedHalf((uint32_t)value - 1) && state.count == 0);
        assert(allocations == (value == 0) && tableRandoms == 0 && controlRandoms == 0);
        assert(memcmp(&effect, &expected, sizeof(effect)) == 0 && D_8019A8C4 == 90);
    }
    for (i = 0; i < 7; ++i) for (j = 0; j < 5; ++j) for (failure = 0; failure < 2; ++failure) {
        int active = counts[i] < 24 && delays[j] == 0;
        reset(); state.count = counts[i]; state.delay = delays[j]; failAllocation = failure;
        countAfterRandom = counts[i]; tableRandom = 255; controlRandom = -1; expected = effect;
        assert(func_8019988C(1, &state) == (counts[i] >= 24 ? 2 : 0));
        assert(allocations == active);
        if (active && !failure) verifySpawn();
        else {
            assert(state.count == counts[i]);
            assert(state.delay == (counts[i] >= 24 ? delays[j] : signedHalf((uint32_t)delays[j] - 1)));
            assert(D_8019A8C4 == (counts[i] >= 24 ? 1 : 90));
            assert(memcmp(&effect, &expected, sizeof(effect)) == 0);
        }
    }
    /* Exhaust every byte/count pair, including count0's signed division case. */
    for (i = 0; i < 256; ++i) for (j = 0; j < 256; ++j) for (k = 0; k < 9; ++k) {
        reset(); failAllocation = 0; models[1].transformCount = i;
        tableRandom = j | (k & 1 ? 0x7FFFFF00 : 0);
        controlRandom = controls[k]; countAfterRandom = counts[k % 7];
        assert(func_8019988C(1, &state) == 0); verifySpawn();
    }
    for (m = 0; m < 5; ++m) {
        reset(); state.count = -7; state.delay = 123;
        D_800E11FA = indices[m]; D_800E2850[indices[m]] = 65535 - m;
        assert(func_8019988C(2, &state) == 0 && allocations == 0 && setups == 0);
        assert(state.count == -7 && state.delay == 123 && D_8019A8C4 == 90);
        assert(D_800F3368 == 64 && D_800F336A == 4 && D_800F3376 == 64 && D_800F3378 == 64);
        assert(D_800F336C == 3 && D_800F336E == 1 && D_800F3372 == 5 && D_800F3374 == 0);
        assert(D_800F3370 == 65535 - m);
    }
    for (event = -3; event <= 4; ++event) {
        if (event >= 0 && event <= 2) continue;
        reset(); state.count = -7; state.delay = 123;
        assert(func_8019988C(event, &state) == 0 && allocations == 0 && setups == 0);
        assert(state.count == -7 && state.delay == 123 && D_8019A8C4 == 90);
    }
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            exe = pathlib.Path(directory) / "room350-model-emitter"
            result = subprocess.run(
                ["cc", "-std=gnu11", "-O2", "-fwrapv", "-fno-strict-aliasing",
                 "-x", "c", "-", "-o", str(exe)],
                input=harness, text=True, capture_output=True,
            )
            self.assertEqual(result.returncode, 0, result.stderr)
            subprocess.run([str(exe)], check=True, timeout=20)
