#include "common.h"
#include "pe1/room_m005.h"

int func_8018F330(int mode, RoomM005OrbiterState *state) {
    RoomM005Seed8 seed = D_8018EFF4;
    s16 target[4];
    char *pool;
    RoomM005OrbiterChild *child;
    void **soundSlot;
    void *currentSound;
    int handle;
    int spread;
    int volume;

    switch (mode) {
    case 0:
        state->armed = 0;
        state->frame = 0;
        pool = D_800F33E0->pool;
        return func_800CE560(pool, 16, 24, RoomM005_FxOrbiter_8018F018);
    case 1:
        if (D_800E27EC == 7) {
            soundSlot = &D_800B0E64;
            currentSound = *soundSlot;
            if (currentSound != 0) {
                volume = 0x7F;
                handle = func_800D3FD8();
                func_8006DF50(*soundSlot, 0x5AB, handle, 0x80, volume);
                currentSound = *soundSlot;
                /* Preserve the separate sound-owner read after the first call. */
                asm("" : : "r"(currentSound) : "memory");
                if (currentSound != 0) {
                    func_8006DF50(*soundSlot, 0x5AC, 0x80, 0x80, volume);
                }
            }
        }
        if (D_800E27EC < 25) {
            pool = D_800F32D0->pool;
            func_800CE870(pool, 0, target);
            pool = D_800F33E0->pool;
            child = func_800CE610(pool);
            if (child) {
                child->seed = func_80071A54();
                spread = func_80071A54() & 0x1FF;
                child->y = target[1] + spread - 0x100;
                child->flag = 0;
                child->radius = (func_80071A54() & 0xFF) + 0x100;
                child->state = 0;
                child->timer = 0;
            }
        }
        if (state->armed == 1) {
            state->frame++;
            if ((s16)state->frame >= 32) return 1;
        } else if (D_800E2368->triggered != 0) {
            state->armed = 1;
        }
        break;
    case 2:
        pool = D_800F32D0->pool;
        func_800CE870(pool, 0, (s16 *)D_80190B84);
        pool = D_800F32D0->pool;
        func_800CE8F0(pool, 20, &seed, D_80190B8C);
        {
            int idx = D_800E11E8;
            int palette;
            D_800F3368.parameter00 = 16;
            D_800F336A = 1;
            D_800F3376 = 16;
            D_800F3378 = 16;
            palette = D_800E2850[idx];
            PE1_COMPILER_MEMORY_BARRIER();
            D_800F336C = 2;
            D_800F336E = 0;
            D_800F3372 = 0;
            D_800F3374 = 8;
            D_800F3370 = palette;
        }
        break;
    }
    return 0;
}
