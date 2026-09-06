import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]


class Room350EffectEmitterTests(unittest.TestCase):
    @unittest.skipUnless(shutil.which("cc"), "host C compiler unavailable")
    def test_animation_crossing_burst_and_allocation_failure(self):
        source = (ROOT / "src/overlays/room_m350/RoomEffect_AnimationBurstCallback.c").read_text()
        harness = '#include <assert.h>\n#include <string.h>\n' + source + r'''
Actor *D_800F32D0;
Emitter *D_800F33E0;
short D_8019A7A0[4];
static Actor actor, replacementActor;
static Animation animation, replacementAnimation;
static Owner owner, replacementOwner;
static Emitter emitter;
static State state;
static short *effect;
static int poolToken, soundToken, replacementSound, setupCalls, allocations, sounds;
static int failAllocation, setupResult, mutate, expectedRemaining, expectedDelay;
int func_801944F0(int event, void **object) { return 0; }
int func_800CE560(void *pool, int capacity, int size, int (*callback)(int, void **)) {
    assert(setupCalls++ == 0 && pool == &poolToken && capacity == 4 && size == 4);
    assert(callback == func_801944F0 && state.remaining == 0 && state.delay == expectedDelay);
    return setupResult;
}
short **func_800CE610(void *pool) {
    assert(allocations++ == 0 && pool == &poolToken && sounds == 0);
    assert(state.delay == expectedDelay);
    if (failAllocation) return 0;
    /* Deliberate mutations verify reloads after allocation. */
    if (mutate) {
        D_800F32D0 = &replacementActor;
        state.remaining = -3;
    }
    return &effect;
}
int Asset_Find08w(int id, int sound, short x, short y, short z) {
    assert(sounds++ == 0 && allocations == 1 && id == 1479);
    assert(sound == (mutate ? replacementSound : soundToken));
    assert(effect == D_8019A7A0 && state.delay == 1);
    assert(state.remaining == expectedRemaining);
    assert(x == D_8019A7A0[0] && y == D_8019A7A0[1] && z == D_8019A7A0[2]);
    return 0;
}
int main(void) {
    const unsigned short frames[] = {0, 7, 8, 9, 25, 26, 65535};
    const short values[] = {-32768, -1, 0, 1, 4, 32767};
    int id, f, p, r, d, fail, change, ret, event;
    emitter.pool = &poolToken; D_800F33E0 = &emitter;
    soundToken = 7; replacementSound = -9;
    actor.animation = &animation; animation.owner = &owner; owner.soundMode = soundToken;
    replacementActor.animation = &replacementAnimation;
    replacementAnimation.owner = &replacementOwner; replacementOwner.soundMode = replacementSound;
    D_8019A7A0[0] = -32768; D_8019A7A0[1] = 123; D_8019A7A0[2] = 32767;
    for (ret = -2; ret <= 2; ++ret) {
        state.remaining = 123; state.delay = -7; expectedDelay = -7;
        setupCalls = allocations = sounds = 0; setupResult = ret;
        assert(func_80194654(0, &state) == ret && setupCalls == 1);
        assert(allocations == 0 && sounds == 0);
    }
    for (event = -2; event <= 3; ++event) {
        if (event == 0 || event == 1) continue;
        assert(func_80194654(event, 0) == 0);
    }
    for (id = 12; id <= 14; ++id)
    for (f = 0; f < 7; ++f) for (p = 0; p < 7; ++p)
    for (r = 0; r < 6; ++r) for (d = 0; d < 6; ++d)
    for (fail = 0; fail < 2; ++fail) for (change = 0; change < 2; ++change) {
        int result = id == 13 && frames[f] >= 26 ? 2 : 0;
        int remaining = values[r], delay = values[d], allocate = 0, play = 0;
        if (!result) {
            if (id == 13 && frames[f] >= 8 && frames[p] < 8) { remaining = 4; delay = 0; }
            if (remaining != 0) {
                allocate = delay <= 0;
                delay = (short)(delay - 1);
                if (allocate && !fail) {
                    play = 1;
                    remaining = (short)((change ? -3 : remaining) - 1);
                }
            }
        }
        D_800F32D0 = &actor; animation.id = id;
        animation.frame = frames[f]; animation.previous = frames[p];
        state.remaining = values[r]; state.delay = values[d];
        expectedDelay = delay; expectedRemaining = remaining;
        failAllocation = fail; mutate = change; effect = 0;
        setupCalls = allocations = sounds = 0;
        assert(func_80194654(1, &state) == result);
        assert(state.remaining == remaining && state.delay == (play ? 1 : delay));
        assert(allocations == allocate && sounds == play && setupCalls == 0);
        assert(effect == (play ? D_8019A7A0 : 0));
    }
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            exe = pathlib.Path(directory) / "room350-effect-emitter"
            result = subprocess.run(
                ["cc", "-std=gnu11", "-O2", "-x", "c", "-", "-o", str(exe)],
                input=harness, text=True, capture_output=True,
            )
            self.assertEqual(result.returncode, 0, result.stderr)
            subprocess.run([str(exe)], check=True, timeout=10)
