#include "pe1/render_object.h"
#include "pe1/field_anim.h"

int func_800DF6AC(int mode, RenderFadeEmitter *state)
{
    RenderFadeParticle *particle;
    u16 intensity, page;
    switch (mode) {
    case 0:
        state->count = 0;
        state->intensity = 160;
        return func_800CE560(D_800F33E0->end, 20, 32, func_800DEFFC);
    case 1:
        if ((D_800E27EC & 3) == 0 && state->count < 8) {
            particle = func_800CE610(D_800F33E0->end);
            if (particle) {
                particle->kind = D_800E2164[state->count];
                particle->phase = 0;
                particle->timer = 0;
            }
            state->count++;
        }
        if (D_800E27EC >= 32)
            state->intensity -= 2;
        if (state->intensity <= 0) {
            state->intensity = 0;
            return 1;
        }
        break;
    case 2:
        intensity = state->intensity;
        D_800F3368.parameter00 = 32;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 32;
        D_800F3368.extent_y = 32;
        page = D_800E2850[D_800E11E6];
        D_800E2244 = intensity;
        D_800F3368.palette = 1;
        D_800F3368.tpage = page;
        func_800CEDA8(1);
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 24;
        break;
    }
    return 0;
}
