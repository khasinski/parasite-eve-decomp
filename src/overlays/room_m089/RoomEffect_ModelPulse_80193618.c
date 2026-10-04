#include "common.h"
#include "pe1/field_model_draw.h"
#include "pe1/room_m089_model_pulse.h"

/* Pulsing model burst: spawns a ring of swirl sprites on the first frame and
 * rising sprites every other frame, tags the player while it is close, and
 * draws three turning layers of one model, a fourth model and a glow strip
 * that fade out over the first 0x31 frames. */
int func_80193618(int mode, RoomM089ModelPulse *state, GteShortVector *anchor) {
    GteShortVector rotation;
    RenderColor color;
    GteMatrix matrix;
    GteVector scale;
    GteMatrix wideMatrix;
    GteVector wideScale;
    GteMatrix lowMatrix;
    GteVector lowScale;
    GteMatrix coreMatrix;
    RoomSwirlRiseState *child;
    RoomM089PulseChannel *channel;
    int fade;
    int outer;
    int tall;
    int glow;
    int wide;
    int low;
    int page;
    int i;

    switch (mode) {
    case 0:
        state->phase = 0;
        state->timer = 0;
        state->position.x = anchor->x;
        state->position.y = anchor->y;
        state->position.z = anchor->z;
        if (D_800E2368->active) {
            RoomM089PulsePool *pool = D_800F32D0->pool;
            if (pool && pool->node) {
                u8 *flag = pool->node->state;
                if (*flag == 1) *flag = 2;
            }
        }
        D_8019416C = func_8006E498(D_800B0E64.channel, 0xC5464704);
        func_800C6D5C(D_8019416C, 0, 0);
        D_80194168 = func_8006E498(D_800B0E64.channel, 0xC5864704);
        func_800C6D5C(D_80194168, 0, 0);
        return func_800CE560(D_800F33E0->pool, 0x10, 0x18, func_80193200);
    case 1:
        if (D_800E27EC == 1) {
            for (i = 0; i < 16; i++) {
                child = func_800CE610(D_800F33E0->pool);
                if (child) {
                    child->phase = 0;
                    child->angle = (func_80071A54() & 0x7F) + (i << 8);
                    child->radius = (func_80071A54() & 0xFF) + 0x1F4;
                }
            }
        }
        if (D_800E27EC < 0x38 && (D_800E27EC & 1)) {
            child = func_800CE610(D_800F33E0->pool);
            if (child) {
                child->phase = 1;
                child->x = (func_80071A54() & 0x7F) + 0x200;
                child->y = func_80071A54();
                child->z = 0;
            }
        }
        if (D_800E27EC >= 0x40) return 1;
        if (func_800C6B90(state, 0xF0) == 0) break;
        if (D_800E27EC >= 0x28) break;
        if (D_800E2368->active) {
            channel = D_800F32D0;
            if ((channel->pool->node->flags & 0x3F000000) == 0x01000000) {
                D_8009D254->actor->flags |= 0x4000;
                channel->pool->node->flags =
                    (channel->pool->node->flags & 0xC0FFFFFF) | 0x29000000;
                channel->pool->node->flags |= 0x80000000;
            }
        }
        break;
    case 2:
        fade = func_80077DC4(D_800E27EC << 5) / 32;
        outer = func_80077DC4(D_800E27EC << 5) + 0x400;
        tall = func_80077CF4(D_800E27EC << 5) + 0x400;
        glow = func_80077DC4(D_800E27EC << 5) / 32;
        wide = func_80077CF4(D_800E27EC << 5) + 0x800;
        low = 0x800 - func_80077CF4(D_800E27EC << 5) / 4;
        D_800F3368.depth = 0x10;
        outer /= 4;
        wide /= 3;
        if (D_800E27EC < 0x21) {
            u16 *index;
            rotation.x = 0;
            rotation.y = -(D_800E27EC << 5);
            rotation.z = 0;
            index = &D_800E11FA;
            D_800F3368.tpage = D_800E2850[*index];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 1;
            page = (u16)(D_800E2850[*index] | GetTPage(0, 1, 0, 0));
            {
                int kind = D_800F3368.palette;
                int palette = D_800E1204[kind];
                GsSetOrign(page, GetClut(0, (kind == 4 && D_800F3428) ? palette + 6
                                                                     : palette + 2));
            }
            func_800C6ED8(1);
            RotMatrixYXZ(&rotation, &matrix);
            matrix.t[0] = state->position.x;
            matrix.t[1] = state->position.y;
            matrix.t[2] = state->position.z;
            scale.x = outer;
            scale.y = tall;
            scale.z = outer;
            Gte_ScaleMatrix(&matrix, &scale);
            func_800C6EF8(D_8019416C);
            func_800C6FA0(D_8019416C, fade);
            func_800C71E4(D_8019416C, &matrix);
            func_800C6F4C(D_8019416C);
            rotation.x = 0;
            rotation.z = 0;
            rotation.y = D_800E27EC << 5;
            RotMatrixYXZ(&rotation, &wideMatrix);
            wideMatrix.t[0] = state->position.x;
            wideMatrix.t[1] = state->position.y;
            wideMatrix.t[2] = state->position.z;
            wideScale.x = wide;
            wideScale.y = low;
            wideScale.z = wide;
            Gte_ScaleMatrix(&wideMatrix, &wideScale);
            func_800C6EF8(D_8019416C);
            func_800C6FA0(D_8019416C, glow);
            func_800C71E4(D_8019416C, &wideMatrix);
            func_800C6F4C(D_8019416C);
            rotation.x = 0;
            rotation.z = 0;
            rotation.y = D_800E27EC << 4;
            RotMatrixYXZ(&rotation, &lowMatrix);
            lowMatrix.t[0] = state->position.x;
            lowMatrix.t[1] = state->position.y;
            lowMatrix.t[2] = state->position.z;
            lowScale.x = outer;
            lowScale.y = low * 3 / 2;
            lowScale.z = outer;
            Gte_ScaleMatrix(&lowMatrix, &lowScale);
            func_800C6EF8(D_8019416C);
            func_800C6FA0(D_8019416C, glow);
            func_800C71E4(D_8019416C, &lowMatrix);
            func_800C6F4C(D_8019416C);
        }
        if (D_800E27EC < 0x19) {
            u16 *index;
            rotation.x = 0;
            rotation.y = D_800E27EC << 6;
            rotation.z = 0;
            fade = func_80077DC4((D_800E27EC << 10) / 24) / 32;
            outer = func_80077CF4((D_800E27EC << 10) / 24) * 3 / 2;
            index = &D_800E11FA;
            D_800F3368.tpage = D_800E2850[*index];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 1;
            page = (u16)(D_800E2850[*index] | GetTPage(0, 3, 0, 0));
            {
                int kind = D_800F3368.palette;
                int palette = D_800E1204[kind];
                GsSetOrign(page, GetClut(0, (kind == 4 && D_800F3428) ? palette + 7
                                                                     : palette + 3));
            }
            func_800C6ED8(1);
            RotMatrixYXZ(&rotation, &coreMatrix);
            coreMatrix.t[0] = state->position.x;
            coreMatrix.t[1] = state->position.y;
            coreMatrix.t[2] = state->position.z;
            lowScale.x = outer;
            lowScale.y = outer;
            lowScale.z = outer;
            Gte_ScaleMatrix(&coreMatrix, &lowScale);
            func_800C6EF8(D_80194168);
            func_800C6FA0(D_80194168, fade);
            func_800C71E4(D_80194168, &coreMatrix);
            func_800C6F4C(D_80194168);
        }
        if (D_800E27EC < 0x31) {
            func_800CF3AC(D_801940F8, &color, D_800E27EC);
            func_800D004C(&state->position, 0x1F4, 0x1F4, 0xC, 0, 0x1000, 0x1000, &color, 0, 0x80, 1);
        }
        D_800F3368.parameter00 = 0x20;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 0x20;
        D_800F3368.extent_y = 0x20;
        D_800F3368.tpage = D_800E2850[D_800E11EA];
        D_800F3368.palette = 3;
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        break;
    }
    return 0;
}
