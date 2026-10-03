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
