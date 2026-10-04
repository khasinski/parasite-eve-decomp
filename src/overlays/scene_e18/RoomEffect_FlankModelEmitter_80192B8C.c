#include "pe1/scene_e18_effects.h"

/* Flank model emitter: loads the flank model, then every sixth frame
 * releases one of four model particles on alternating sides of the room
 * model (the first one plays the two launch sounds at the model's root
 * joint) and finishes once the particles have counted D_801941C8 down. */
int func_80192B8C(int mode, SceneE18EmitterState *state) {
    SceneE18FlankParticle *particle;
    SceneE18Actor *actor;
    int side;

    switch (mode) {
    case 0:
        D_801941C8 = 4;
        D_801941C4 = func_8006E498(D_800B0E64, 0xC58C7704);
        func_800C6D5C(D_801941C4, 0, 0);
        state->count = 0;
        state->delay = 0;
        return func_800CE560(D_800F33E0->pool, 0x14, 4, func_801928CC);
    case 1:
        if (state->count < 4) {
            state->delay++;
            if (state->delay < 6) break;
            particle = func_800CE610(D_800F33E0->pool);
            if (particle == 0) break;
            actor = D_800F32D0;
            particle->position = actor->instance->transform.t;
            particle->angle = 0;
            if (state->count & 1)
                side = 0x80;
            else
                side = -0x80;
            particle->width = 0x200;
            particle->height = 0x200;
            particle->turn = side;
            particle->widthGrowth = 0;
            particle->heightGrowth = 0;
            particle->brightness = 0xC0;
            particle->leader = state->count == 0;
            state->delay = 0;
            state->count++;
            if (state->count == 1) {
                func_8006DCE4(0x5C4, actor->instance->owner->asset,
                              (s16)actor->instance->transforms->t[0],
                              (s16)actor->instance->transforms->t[1],
                              (s16)actor->instance->transforms->t[2]);
                func_8006DCE4(0x5C5, D_800F32D0->instance->owner->asset,
                              (s16)D_800F32D0->instance->transforms->t[0],
                              (s16)D_800F32D0->instance->transforms->t[1],
                              (s16)D_800F32D0->instance->transforms->t[2]);
            }
            break;
        }
        if (D_801941C8 != 0) break;
        return 1;
    case 2:
        {
            int kind;
            int palette;
            int page = (D_800E2850[D_800E11FA] | func_80077A64(0, 1, 0, 0)) & 0xFFFF;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            func_800C6EC0(page, func_80077AA4(0, (kind == 4 && D_800F3428 != 0) ? palette + 10
                                                                         : palette + 6));
        }
        func_800C6ED8(1);
        break;
    }
    return 0;
}
