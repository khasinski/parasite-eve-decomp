#include "pe1/render_object.h"
#include "pe1/field_spiral.h"
#include "pe1/random.h"
#include "pe1/gte.h"
#include "pe1/psyq_gpu.h"

/* Spiral emitter on the ground under the actor: releases 24 circling
 * sprites (func_800D629C) on the first frame, publishes the anchor and the
 * swelling radius they follow, and draws a spinning model in two sizes. */
int func_800D6514(int mode, RenderArcingEmitter *state)
{
    GteMatrix matrix;
    GteVector scale;
    GteShortVector angles;
    RenderSpiralSprite *sprite;
    u16 *slots;
    u16 *tpages;
    int intensity;
    int size;
    int angle;
    int i;

    switch (mode) {
    case 0:
        state->phase = 0;
        state->position.x = D_800F32D0->actor->render_object.target_x;
        state->position.y = D_800F32D0->actor->render_object.target_y;
        state->position.z = D_800F32D0->actor->render_object.target_z;
        state->position.y = D_800942EC.value;
        func_800C6D5C(D_800F32D4, 0, 0xC0);
        return func_800CE560(D_800F33E0->end, 16, 24,
                             func_800D629C);
    case 1:
        if (D_800E27EC == 0) {
            for (i = 0, angle = 0; i < 24; i++, angle += 0xAA) {
                sprite = func_800CE610(D_800F33E0->end);
                if (sprite) {
                    sprite->height = state->position.y;
                    sprite->phase = rand();
                    sprite->angle = angle;
                }
            }
        }
        if (D_800E27EC >= 24)
            return 1;
        D_800E21D4 = rsin((D_800E27EC << 10) / 24) / 3;
        D_800E21D8.x = state->position.x;
        D_800E21D8.y = state->position.y;
        D_800E21D8.z = state->position.z;
        break;
    case 2:
        size = rsin((D_800E27EC << 10) / 24) / 2;
        intensity = rcos((D_800E27EC << 10) / 24) / 32;
        scale.x = scale.y = scale.z = size;
        angles.x = 0;
        angles.y = D_800E27EC << 5;
        angles.z = 0;
        RotMatrixYXZ(&angles, &matrix);
        Gte_ScaleMatrix(&matrix, &scale);
        matrix.t[0] = state->position.x;
        matrix.t[1] = state->position.y;
        matrix.t[2] = state->position.z;
        slots = D_800E11E4;
        D_800F3368.tpage = D_800E2850[slots[0]];
        D_800F3368.palette = 0;
        func_800CEDA8(0);
        tpages = D_800E2850;
        D_800F3368.parameter06 = 0;
        gte_ldrotmatrix(D_800BCFA4.value);
        gte_ldtransmatrix(D_800BCFA4.value);
        {
            int tpage = (u16)(tpages[slots[0]] | GetTPage(0, 1, 0, 0));
            int kind = D_800F336C;
            int palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428)
                palette += 4;
            GsSetOrign(tpage, GetClut(0xC0, palette));
        }
        func_800C6ED8(1);
        func_800C6EF8(D_800F32D4);
        func_800C6FA0(D_800F32D4, intensity);
        func_800C71E4(D_800F32D4, &matrix);
        scale.x /= 2;
        scale.y /= 2;
        scale.z /= 2;
        RotMatrixYXZ(&angles, &matrix);
        Gte_ScaleMatrix(&matrix, &scale);
        matrix.t[0] = state->position.x;
        matrix.t[1] = state->position.y;
        matrix.t[2] = state->position.z;
        func_800C71E4(D_800F32D4, &matrix);
        func_800C6F4C(D_800F32D4);
        D_800F3368.parameter00 = 32;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 32;
        D_800F3368.extent_y = 32;
        D_800F3368.tpage = D_800E2850[D_800E11E4[0]];
        D_800F3368.palette = 0;
        func_800CEDA8(0);
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 64;
        break;
    }
    return 0;
}
