#include "pe1/render_object.h"
#include "pe1/gte.h"
#include "pe1/psyq_gpu.h"
#include "pe1/field_anim.h"
#include "pe1/battle_runtime.h"
#include "pe1/random.h"

int func_800D8978(int mode, RenderVerticalEffect *state)
{
    GteShortVector position;
    GteRotation rotation;
    RenderColor color;
    int scale, palette, selector;
    /* Matching debt: retail compares the palette selector against $3. */
    register int special_palette asm("$3");
    switch (mode) {
    case 1:
        state->y = D_800E21F8.y + (-D_800E27EC * 900) / 24;
        state->x = D_800E21F8.x;
        state->z = D_800E21F8.z;
        state->angle += 128;
        state->timer++;
        if (D_800E27EC >= 24)
            return 1;
        break;
    case 2:
        if (state->stage == 0) {
            scale = 8192 - rsin(state->timer << 7);
            if (state->timer >= 8) {
                state->timer = 0;
                state->stage = 1;
            }
        } else {
            scale = 4096;
        }
        position.x = state->x;
        position.y = state->y;
        position.z = state->z;
        rotation.x = 1024;
        rotation.y = 0;
        rotation.z = D_800E27EC << 7;
        rotation.flags = 1;
        func_800CF3AC(D_800E1A14, &color, D_800E27EC);
        scale *= 2;
        selector = D_800F336C;
        special_palette = 4;
        palette = D_800E1204[selector];
        if (selector == special_palette && D_800F3428)
            palette += 7;
        else
            palette += 3;
        func_800CEE20(&position, &rotation, scale, scale, 68,
                     GetClut(0, palette), 1, 128, &color);
        break;
    }
    return 0;
}

int func_800D8B6C(int mode, void *state)
{
    GteShortVector position;
    RenderVerticalEffect *effect;
    switch (mode) {
    case 0:
        return func_800CE560(D_800F33E0->end, 12, 14,
                            (FieldAnimTaskCallback)func_800D8978);
    case 1:
        if (D_800E27EC < 32 && (D_800E27EC & 1)) {
            effect = func_800CE610(D_800F33E0->end);
            if (effect) {
                effect->angle = rand();
                effect->stage = 0;
                effect->timer = 0;
                func_800CE870((char *)D_8009D254, 1, &position.x);
                effect->y = position.y - 512;
            }
        }
        if (D_800E27EC >= 50)
            return 1;
        /* Fall through: refresh the center and render parameters. */
    case 2:
        func_800CE870((char *)D_8009D254, 1, &D_800E21F8.x);
        D_800F3368.parameter00 = 32;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 32;
        D_800F3368.extent_y = 32;
        D_800F3368.tpage = D_800E2850[D_800E11F6];
        D_800F3368.palette = 1;
        func_800CEDA8(1);
        D_800F3368.parameter06 = 1;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 8;
        break;
    }
    return 0;
}
