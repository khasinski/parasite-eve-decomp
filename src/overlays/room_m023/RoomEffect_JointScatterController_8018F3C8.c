#include "pe1/room_m023_effects.h"

/* Every `interval` frames emits a particle with a random velocity, either
 * from the tracked model joint or from a random point of a wide band. */
int func_8018F3C8(int mode, RoomM023ScatterState *state) {
    RoomM023Template template = D_8018EFF4;
    s16 position[4];
    RoomM023Particle *particle;

    switch (mode) {
    case 0:
        state->unused08 = 0;
        state->unused0A = 0;
        state->timer = 0;
        switch (D_800E2368->variant) {
        case 0:
            state->joint = 8;
            break;
        case 1:
            state->joint = 16;
            break;
        }
        return func_800CE560(D_800F33E0->pool, 16, 12, func_8018F004);
    case 1:
        func_800CE8F0(D_800F32D0->pool, state->joint, &template, position);
        state->timer++;
        if (D_800E2368->interval == 0) return 1;
        if (D_800E2368->interval < state->timer) {
            state->timer = 0;
            func_800CE870(D_800F32D0->pool, 0, position);
            particle = func_800CE610(D_800F33E0->pool);
            if (particle == 0) return 0;
            if (func_80071A54() & 1) {
                particle->x = (func_80071A54() & 0x7FF) - 0x400;
                particle->y = func_80071A54();
                particle->z = 0;
                particle->attached = 0;
            } else {
                particle->x = position[0];
                particle->y = position[1];
                particle->z = position[2];
                particle->attached = 1;
            }
            particle->vx = func_80071A54() % 70 - 35;
            particle->vy = func_80071A54() % 70 - 35;
            particle->vz = func_80071A54() % 70 - 35;
        }
        break;
    case 2:
        func_800CE8F0(D_800F32D0->pool, state->joint, &template, D_80190758);
        D_800F3368.parameter00 = 32;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 32;
        D_800F3368.extent_y = 32;
        D_800F3368.tpage = D_800E2850[D_800E11E4[3]];
        D_800F3368.palette = 3;
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 12;
        break;
    }
    return 0;
}
