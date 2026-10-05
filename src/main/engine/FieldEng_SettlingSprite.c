/* MASPSX_FLAGS: --expand-div */
#include "pe1/render_object.h"
#include "pe1/psyq_gpu.h"
#include "pe1/gte.h"
#include "pe1/random.h"

int func_800DD9E4(int mode, RenderSettlingSprite *state)
{
    RenderColor color;
    GteShortVector position;
    int scale;
    int blend;
    int palette;

    switch (mode) {
    case 1:
        state->timer++;
        if (state->stage >= 2)
            return 1;
        break;
    case 2:
        scale = 0x1000;
        switch (state->stage) {
        case 0:
            blend = 0x1000 - rcos((state->timer << 10) / 28);
            LoadAverageShort12(state, &D_800E223C, 0x1000 - blend, blend,
                               state);
            state->position.y += rsin(state->phase) / 128;
            state->phase += 160;
            position.x = state->position.x;
            position.y = state->position.y;
            position.z = state->position.z;
            func_800CF3AC(D_800E207C, &color, (D_800E27EC << 6) / 28);
            scale = 0x2000 - (state->timer << 12) / 28;
            if (state->timer >= 28) {
                state->stage = 1;
                state->timer = 0;
                state->position.x += (rand() & 255) - 128;
                state->position.y += (rand() & 255) - 128;
                state->position.z += (rand() & 255) - 128;
            }
            break;
        case 1:
            position.x = state->position.x;
            position.y = state->position.y;
            position.z = state->position.z;
            blend = rcos((state->timer << 10) / 12) / 32;
            *(u32 *)&color = blend | (blend << 8);
            state->position.x += (rand() & 7) - 3;
            state->position.y += (rand() & 7) - 3;
            state->position.z += (rand() & 7) - 3;
            if (state->timer >= 8)
                state->stage = 2;
            break;
        }
        gte_ldrotmatrix(D_800BCFA4.value);
        gte_ldtransmatrix(D_800BCFA4.value);
        func_800D1DEC(&position, &color, 128, 1);
        palette = D_800E1204[D_800F336C];
        if (D_800F336C == 4 && D_800F3428)
            palette += 4;
        func_800CEE20(&position, 0, scale, scale, 128, GetClut(32, palette), 3,
                      128, &color);
        break;
    }
    return 0;
}

#include "pe1/render_object.h"
#include "pe1/psyq_gpu.h"
#include "pe1/gte.h"
#include "pe1/field_anim.h"
#include "pe1/battle_runtime.h"
#include "pe1/random.h"
int func_800DDD70(int mode, GteShortVector *state)
{
    GteShortVector position;
    RenderColor color;
    RenderSettlingSprite *particle;
    int scale;
    int draw_mode;
    switch (mode) {
    case 0:
        func_800CE870((char *)D_8009D254, 1, &state->x);
        return func_800CE560(D_800F33E0->end, 16, 32,
                            (FieldAnimTaskCallback)func_800DD9E4);
    case 1:
        if (D_800E27EC < 67) {
            particle = func_800CE610(D_800F33E0->end);
            if (particle) {
                particle->position.x = state->x + (rand() & 511) - 256;
                particle->position.y = state->y - rand() % 600;
                particle->position.z = state->z + (rand() & 511) - 256;
                particle->stage = 0;
                particle->timer = 0;
                particle->phase = rand();
            }
        }
        if (D_800E27EC >= 86) return 1;
        break;
    case 2:
        position.x = D_8009D254->renderObject.matrices[19].translation[0];
        position.y = D_8009D254->renderObject.matrices[19].translation[1];
        position.z = D_8009D254->renderObject.matrices[19].translation[2];
        D_800E223C.x = position.x;
        D_800E223C.y = position.y;
        D_800E223C.z = position.z;
        if (D_800E27EC < 87) {
            scale = (D_800E27EC << 12) / 86 + 2048;
            D_800F3368.depth = 16;
            func_800CF3AC(D_800E20AC, &color, D_800E27EC);
            draw_mode = 1;
            if (D_800E27EC & 1) draw_mode = 3;
            func_800D004C(&position, 500, 500, 16, 0, scale, scale, &color, 0, 128, 1);
            color.b = 0;
            func_800D0728(&position, 400, 300, 20, 0, scale, scale, &color, 0, 64, draw_mode);
        }
        D_800F3368.parameter00 = 32;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 32;
        D_800F3368.extent_y = 32;
        D_800F3368.tpage = D_800E2850[D_800E11E6];
        D_800F3368.palette = 1;
        func_800CEDA8(1);
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        break;
    }
    return 0;
}
