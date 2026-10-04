#include "common.h"
#include "pe1/gte.h"
#include "pe1/room_drift_pulse.h"
#include "pe1/room_m023_effects.h"

/* Drift/pulse sprite for room_m023. The glow strip's anchor is taken into
 * a local pointer first: its address load is what keeps the frame counter
 * load after the palette lookup like retail. */

int func_8018F004(int mode, RoomDriftPulseState *state) {
    GteShortVector origin;
    GteRotation rotation;
    RenderColor color;
    RenderMatrixSlot *matrixSlot;

    switch (mode) {
    case 1:
        switch (state->phase) {
        case 0:
            state->x += state->vx;
            state->y += state->vy;
            state->z += state->vz;
            if (D_800E27EC >= 4) {
                return 1;
            }
        case 1:
            state->x += state->vx;
            state->y += state->vy;
            state->z += state->vz;
            if (D_800E27EC < 8) {
                break;
            }
            return 1;
        default:
            return 0;
        }
        break;
    case 2:
        matrixSlot = &D_800BCFA4;
        gte_ldrotmatrix(matrixSlot->value);
        gte_ldtransmatrix(matrixSlot->value);
        switch (state->phase) {
        case 0:
            {
                GteShortVector *anchor = &D_80190758;
                int kind;
                int palette;
                int fade;
                u16 clut;

                *(int *)&color = 0x808080;
                origin.x = state->x;
                origin.y = state->y;
                origin.z = state->z;
                kind = D_800F336C;
                palette = D_800E1204[kind];
                fade = (D_800E27EC - 1) << 4;
                if (kind == 4 && D_800F3428 != 0) palette += 4;
                clut = func_80077AA4(0, palette);
                func_800D2370(anchor, (GteRotation *)state, 0x3C0, 0x64, 0, fade,
                              0xFF, 0xF, clut, 0, &color, 0xBE, 1);
            }
            rotation.x = state->x;
            rotation.y = state->y;
            rotation.z = state->z;
            rotation.flags = 1;
            {
                int kind = D_800F336C;
                int palette = D_800E1204[kind];
                u16 clut;

                if (kind == 4 && D_800F3428 != 0) palette += 4;
                clut = func_80077AA4(0x20, palette);
                func_800CEE20(&D_80190758, &rotation, 0x1000, 0x1000,
                              (D_800F336A << 1) * (D_800E27EC - 1) + 0x60, clut,
                              3, 0x80, 0);
            }
            break;
        case 1:
            rotation.x = 0;
            rotation.y = 0;
            rotation.flags = 0;
            rotation.z = D_800E27EC << 6;
            {
                int scale = func_80077DC4((D_800E27EC - 1) << 7) / 32;
                int kind = D_800F336C;
                int palette = D_800E1204[kind];
                u16 clut;

                if (kind == 4 && D_800F3428 != 0) palette += 4;
                clut = func_80077AA4(0x10, palette);
                func_800CEE20((GteShortVector *)state, &rotation, 0x1000, 0x1000,
                              D_800F336A * D_800E27EC + 0x40, clut, 1, scale, 0);
            }
            break;
        default:
            return 0;
        }
        break;
    default:
        return 0;
    }
    return 0;
}

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
        func_800CE8F0(D_800F32D0->pool, state->joint, &template, &D_80190758);
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
