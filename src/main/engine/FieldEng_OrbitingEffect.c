#include "pe1/render_object.h"
#include "pe1/psyq_gpu.h"
int func_800D7E78(int mode, GteShortVector *state)
{
    GteShortVector position;
    RenderColor color;
    int scale, clut_y;
    switch (mode) {
    case 1:
        state->y--;
        if (D_800E27EC >= 6)
            return 1;
        break;
    case 2:
        position.x = state->x;
        position.y = state->y;
        position.z = state->z;
        func_800CF3AC(D_800E1988, &color, D_800E27EC);
        scale = 4096 - (D_800E27EC << 12) / 6;
        clut_y = D_800E1204[D_800F336C];
        if (D_800F336C == 4 && D_800F3428)
            clut_y += 4;
        func_800CEE20(&position, 0, scale, scale, 0x8A,
                     GetClut(0x70, clut_y), 1, 128, &color);
        break;
    }
    return 0;
}

/* MASPSX_FLAGS: --expand-div */
#include "pe1/render_object.h"
#include "pe1/field_anim.h"
#include "pe1/random.h"
#include "pe1/gte.h"
#include "pe1/psyq_gpu.h"
int func_800D7FBC(int mode, RenderOrbitingEffect *state)
{
    GteShortVector position;
    GteRotation rotation;
    RenderColor color = D_800C22E0;
    GteShortVector *particle;
    int scale, intensity, palette, interval;
    switch (mode) {
    case 1:
        state->x = D_800E21EC.x + rcos(state->angle) * state->radius / 4096;
        state->z = D_800E21EC.z + rsin(state->angle) * state->radius / 4096;
        state->angle += 96;
        state->radius = rcos((D_800E27EC << 10) / 36) * 700 / 4096;
        interval = D_800E2368->variables[6] == 11 ? 1 : 3;
        if (D_800E27EC % interval == 0) {
            particle = func_800CE610(D_800E21F4);
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
            intensity = rsin((state->timer << 10) / 12) / 32;
            scale = 4096;
            if (state->timer >= 12) {
                state->timer = 0;
                state->stage = 1;
            }
            break;
        case 1:
            scale = 4096;
            intensity = 128;
            if (state->timer >= 12) {
                state->timer = 0;
                state->stage = 2;
            }
            break;
        default:
            intensity = 128;
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
        palette = D_800E1204[interval];
        func_800CEE20(&position, &rotation, scale, scale, 36,
                     GetClut(0, (interval == 4 && D_800F3428) ? palette + 7
                                                               : palette + 3),
                     1, intensity, &color);
        break;
    }
    return 0;
}

#include "pe1/render_object.h"
#include "pe1/field_anim.h"
#include "pe1/battle_runtime.h"
#include "pe1/random.h"
#include "pe1/gte.h"

int func_800D8388(int mode, RenderOrbitingEmitter *state)
{
    GteShortVector position;
    RenderOrbitingEffect *effect;
    int count;
    int phase;

    switch (mode) {
    case 0:
        state->phase = rand();
        count = func_800CE560(D_800F33E0->end, 16, 12,
                              (FieldAnimTaskCallback)func_800D7FBC);
        count += func_800CE5AC(&state->particles, count, 8, 32,
                               func_800D7E78);
        return count;
    case 1:
        if (D_800E27EC < 44 && D_800E27EC % 3 == 0) {
            effect = func_800CE610(D_800F33E0->end);
            if (effect) {
                phase = state->phase;
                effect->stage = 0;
                effect->timer = 0;
                effect->radius = 700;
                effect->angle = phase;
                func_800CE870((char *)D_8009D254, 1, &position.x);
                effect->y = position.y + (rand() & 511) - 655;
                state->phase -= 0x555;
            }
        }
        if (D_800E27EC >= 70)
            return 1;
        func_800CE688(state->particles);
        D_800E21F4 = state->particles;
        break;
    case 2:
        gte_ldrotmatrix(D_800BCFA4.value);
        gte_ldtransmatrix(D_800BCFA4.value);
        D_800F3368.parameter00 = 16;
        D_800F3368.parameter02 = 1;
        D_800F3368.extent_x = 16;
        D_800F3368.extent_y = 16;
        D_800F3368.tpage = D_800E2850[D_800E11E4[1]];
        D_800F3368.palette = 1;
        func_800CEDA8(1);
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 8;
        func_800CE78C(state->particles);
        func_800CE870((char *)D_8009D254, 1, &D_800E21EC.x);
        D_800F3368.parameter00 = 32;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 32;
        D_800F3368.extent_y = 32;
        D_800F3368.tpage = D_800E2850[D_800E11E4[9]];
        D_800F3368.palette = 1;
        func_800CEDA8(1);
        D_800F3368.parameter06 = 1;
        D_800F3368.parameter0A = 0;
        break;
    }
    return 0;
}

#include "pe1/render_object.h"
#include "pe1/field_anim.h"
#include "pe1/battle_runtime.h"
#include "pe1/random.h"
#include "pe1/gte.h"

int func_800D868C(int mode, RenderOrbitingEmitter *state)
{
    GteShortVector position;
    RenderOrbitingEffect *effect;
    int count;
    int phase;
    switch (mode) {
    case 0:
        state->phase = rand();
        count = func_800CE560(D_800F33E0->end, 16, 3,
                             (FieldAnimTaskCallback)func_800D7FBC);
        count += func_800CE5AC(&state->particles, count, 8, 18,
                                   func_800D7E78);
        return count;
    case 1:
        if (D_800E27EC == 1) {
            for (count = 0; count < 3; ++count) {
                effect = func_800CE610(D_800F33E0->end);
                if (effect) {
                    phase = state->phase;
                    effect->stage = 0;
                    effect->timer = 0;
                    effect->radius = 700;
                    effect->angle = phase;
                    func_800CE870((char *)D_8009D254, 1, &position.x);
                    effect->y = position.y - 400;
                    state->phase -= 0x555;
                }
            }
        }
        if (D_800E27EC >= 43)
            return 1;
        func_800CE688(state->particles);
        D_800E21F4 = state->particles;
        break;
    case 2:
        gte_ldrotmatrix(D_800BCFA4.value);
        gte_ldtransmatrix(D_800BCFA4.value);
        D_800F3368.parameter00 = 16;
        D_800F3368.parameter02 = 1;
        D_800F3368.extent_x = 16;
        D_800F3368.extent_y = 16;
        D_800F3368.tpage = D_800E2850[D_800E11E4[1]];
        D_800F3368.palette = 1;
        func_800CEDA8(1);
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 8;
        func_800CE78C(state->particles);
        func_800CE870((char *)D_8009D254, 1, &D_800E21EC.x);
        D_800F3368.parameter00 = 32;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 32;
        D_800F3368.extent_y = 32;
        D_800F3368.tpage = D_800E2850[D_800E11E4[9]];
        D_800F3368.palette = 1;
        func_800CEDA8(1);
        D_800F3368.parameter06 = 1;
        D_800F3368.parameter0A = 0;
        break;
    }
    return 0;
}
