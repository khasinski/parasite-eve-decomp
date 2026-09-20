#include "pe1/render_object.h"
#include "pe1/field_anim.h"
#include "pe1/battle_runtime.h"
#include "pe1/random.h"
#include "pe1/gte.h"

int func_800D868C(int mode, RenderOrbitingEmitter *state)
{
    GteShortVector position;
    RenderOrbitingEffect *effect;
    s32 **slot;
    register int x asm("$12");
    register int y asm("$13");
    register int z asm("$14");
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
        func_800CE870((char *)D_8009D254, 1, &D_800E21EC.x);
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
