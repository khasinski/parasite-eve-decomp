#include "pe1/room_sound_burst.h"
#ifndef ROOMEFFECT_SOUND_BURST_FUNC
#error "ROOMEFFECT_SOUND_BURST_FUNC must name the room entry point"
#endif
#ifndef ROOMEFFECT_SOUND_BURST_CALLBACK
#error "ROOMEFFECT_SOUND_BURST_CALLBACK must name the particle callback"
#endif

extern RoomSoundBurstTemplate8 D_8018F1D0;
extern u16 D_800F336C;
extern s16 D_800F336E, D_800F3372, D_800F3374, D_800F3370;
extern s16 D_800F336A;
extern s16 D_800F3368, D_800F3376, D_800F3378;
extern void ROOMEFFECT_SOUND_BURST_CALLBACK(void);

int ROOMEFFECT_SOUND_BURST_FUNC(int mode, RoomSoundBurstState *state) {
    RoomSoundBurstTemplate8 template = D_8018F1D0;
    s16 position[4];
    s16 target[4];
    char *pool;
    RoomSoundBurstParticle *child;
    int angle;

    switch (mode) {
    case 0:
        state->soundHandle = func_800D3F64(0x5A1, func_800D3FD8());
        pool = D_800F33E0->pool;
        return func_800CE560(pool, 24, 24, ROOMEFFECT_SOUND_BURST_CALLBACK);
    case 1:
        state->frame++;
        if (D_800E27EC < 0x47) {
            pool = D_800F32D0->pool;
            func_800CE8F0(pool, 9, &template, position);
            angle = rsin((D_800E27EC << 11) / 70) / 32;
            if (D_800E27EC % 3 == 0) {
                pool = D_800F33E0->pool;
                child = func_800CE610(pool);
                if (child) {
                    pool = D_800F32D0->pool;
                    func_800CE9D4(pool, 0, target);
                    child->height = target[1];
                    child->x = position[0];
                    child->y = position[1];
                    child->z = position[2];
                    child->phase = 0;
                    child->amplitude = 0;
                    child->swing = 0;
                    child->tilt = func_80071A54();
                    child->spread = angle;
                }
                if (D_800E2368->active) {
                    RoomSoundBurstNode **slot =
                        (RoomSoundBurstNode **)D_800F32D0->pool;
                    if (slot && *slot) {
                        u8 *flag = (*slot)->state;
                        if (*flag == 1) *flag = 2;
                    }
                }
            }
        }
        if (D_800E27EC == 0x46 && state->soundHandle != -1) {
            func_800866A4(state->soundHandle, 0);
        }
        if (D_800E27EC < 8) break;
        return 2;
    case 2:
    {
        int idx = D_800E11E8;
        int palette;
        D_800F3368 = 16;
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
        break;
    }
    }
    return 0;
}

#undef ROOMEFFECT_SOUND_BURST_FUNC
#undef ROOMEFFECT_SOUND_BURST_CALLBACK
