#include "pe1/render_object.h"
#include "pe1/field_anim.h"
#include "pe1/battle_runtime.h"
#include "pe1/random.h"
#include "pe1/gte.h"
#include "pe1/psyq_gpu.h"

int func_800D8D14(int mode, GteShortVector *state)
{
    GteShortVector position;
    GteRotation rotation;
    RenderColor color;
    int scale, palette;
    switch (mode) {
    case 1:
        state->y -= 3;
        if (D_800E27EC >= 12)
            return 1;
        break;
    case 2:
        position.x = state->x;
        position.y = state->y;
        position.z = state->z;
        rotation.x = 0;
        rotation.y = 0;
        rotation.z = D_800E27EC << 6;
        rotation.flags = 0;
        func_800CF3AC(D_800E1AA0, &color, D_800E27EC);
        scale = 4096 - (D_800E27EC << 12) / 12;
        palette = D_800E1204[D_800F336C];
        if (D_800F336C == 4 && D_800F3428)
            palette += 4;
        func_800CEE20(&position, &rotation, scale, scale, 138,
                     GetClut(112, palette), 1, 128, &color);
        break;
    }
    return 0;
}

int func_800D8E74(int mode, RenderHelicalEffect *state)
{
    GteShortVector position;
    GteRotation rotation;
    RenderColor color = D_800C22E0;
    GteShortVector *particle;
    int scale, palette, interval;
    /* Matching debt: retail compares the palette selector against $3. */
    register int special_palette asm("$3");
    switch (mode) {
    case 1:
        state->x = D_800E2200.x + rcos(state->angle) * state->radius / 4096;
        state->z = D_800E2200.z + rsin(state->angle) * state->radius / 4096;
        if (state->direction)
            state->y = D_800E2200.y - D_800E27EC * 500 / 36;
        else
            state->y = (s16)(D_800E27EC * 500 / 36 - 1000) + D_800E2200.y;
        state->angle += 96;
        state->radius = rcos((D_800E27EC << 10) / 36) * 700 / 4096;
        if ((D_800E27EC & 1) == 0) {
            particle = func_800CE610(D_800E2208);
            if (particle) {
                particle->x = state->x + (rand() & 7) - 3;
                particle->y = state->y + (rand() & 7) - 3;
                particle->z = state->z + (rand() & 7) - 3;
            }
        }
        state->timer++;
        if (D_800E27EC >= 36)
            return 1;
        break;
    case 2:
        switch (state->stage) {
        case 0:
            rsin((state->timer << 10) / 12);
            scale = 4096;
            if (state->timer >= 12) {
                state->timer = 0;
                state->stage = 1;
            }
            break;
        case 1:
            scale = 4096;
            if (state->timer >= 12) {
                state->timer = 0;
                state->stage = 2;
            }
            break;
        default:
            scale = rcos((state->timer << 10) / 12);
            state->radius += state->timer * 4;
            break;
        }
        position.x = state->x;
        position.y = state->y;
        position.z = state->z;
        rotation.x = 0;
        rotation.y = 0;
        rotation.z = D_800E27EC << 7;
        rotation.flags = 0;
        scale *= 2;
        interval = D_800F336C;
        special_palette = 4;
        palette = D_800E1204[interval];
        if (interval == special_palette && D_800F3428)
            palette += 6;
        else
            palette += 2;
        func_800CEE20(&position, &rotation, scale, scale, 228,
                     GetClut(0, palette), 1, 128, &color);
        break;
    }
    return 0;
}

int func_800D927C(int mode, RenderOrbitingEmitter *state)
{
    RenderHelicalEffect *effect;
    s32 **slot;
    register int x asm("$12");
    register int y asm("$13");
    register int z asm("$14");
    int count;
    int phase;
    switch (mode) {
    case 0:
        state->phase = rand();
        count = func_800CE560(D_800F33E0->end, 16, 4,
                             (FieldAnimTaskCallback)func_800D8E74);
        count += func_800CE5AC(&state->particles, count, 8, 32,
                                   func_800D8D14);
        return count;
    case 1:
        if (D_800E27EC < 2) {
            for (count = 0; count < 2; ++count) {
                effect = func_800CE610(D_800F33E0->end);
                if (effect) {
                    phase = state->phase;
                    effect->stage = 0;
                    effect->timer = 0;
                    effect->radius = 700;
                    effect->angle = phase + (count << 11);
                    effect->direction = D_800E27EC;
                }
            }
            state->phase += 1024;
        }
        if (D_800E27EC >= 70)
            return 1;
        func_800CE688(state->particles);
        D_800E2208 = state->particles;
        break;
    case 2: {
        register GteMatrixWords *matrix asm("$8");
        /* Fixed GTE transfer registers; matching debt is documented. */
        slot = &D_800BCFA4.value;
        asm volatile("" : "=r"(slot) : "0"(slot));
        matrix = (GteMatrixWords *)*slot;
        x = matrix->r11_r12;
        y = matrix->r13_r21;
        gte_ctc2_0(x);
        gte_ctc2_1(y);
        x = matrix->r22_r23;
        y = matrix->r31_r32;
        z = matrix->r33_pad;
        gte_ctc2_2(x);
        gte_ctc2_3(y);
        gte_ctc2_4(z);
        x = matrix->tx;
        y = matrix->ty;
        gte_ctc2_5(x);
        z = matrix->tz;
        gte_ctc2_6(y);
        gte_ctc2_7(z);
        /* Preserve the parameter store before the texture-table lookup. */
        *(volatile u16 *)&D_800F3368.parameter00 = 16;
        D_800F3368.parameter02 = 1;
        D_800F3368.extent_x = 16;
        D_800F3368.extent_y = 16;
        D_800F3368.tpage = D_800E2850[D_800E11E6];
        D_800F3368.palette = 1;
        func_800CEDA8(1);
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 8;
        func_800CE78C(state->particles);
        func_800CE870((char *)D_8009D254, 1, &D_800E2200.x);
        *(volatile u16 *)&D_800F3368.parameter00 = 32;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 32;
        D_800F3368.extent_y = 32;
        D_800F3368.tpage = D_800E2850[D_800E11F6];
        D_800F3368.palette = 1;
        func_800CEDA8(1);
        D_800F3368.parameter06 = 1;
        D_800F3368.parameter0A = 0;
        break;
    }
    }
    return 0;
}
