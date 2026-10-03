/* MASPSX_FLAGS: --expand-div */
#include "pe1/render_object.h"
#include "pe1/field_anim.h"
#include "pe1/random.h"
#include "pe1/field_actor.h"
#include "pe1/gte.h"

int func_800D5A00(int mode, RenderArcingEmitter *state)
{
    GteShortVector position;
    RenderColor color = D_800C22C4;
    RenderCosineEffect *effect;
    int scale;
    int i;

    switch (mode) {
    case 0:
        state->phase = rand();
        state->radius = 0;
        state->position.x = D_800F32D0->actor->render_object.target_x;
        state->position.y = D_800F32D0->actor->render_object.target_y;
        state->position.z = D_800F32D0->actor->render_object.target_z;
        return func_800CE560(D_800F33E0->end, 8, 24,
                             (FieldAnimTaskCallback)func_800D5898);
    case 1:
        if (D_800E27EC >= 4 && D_800E27EC < 11) {
            for (i = 0; i < 3; i++) {
                effect = func_800CE610(D_800F33E0->end);
                if (effect) {
                    effect->amplitude = state->radius;
                    effect->duration = (rand() & 3) + 12;
                    effect->y = state->phase;
                    state->phase += 0x8AA + (rand() & 31);
                }
            }
        }
        if (D_800E27EC >= 32)
            return 1;
        break;
    case 2:
        D_800F3368.depth = 64;
        if (D_800E27EC < 13) {
            position.x = state->position.x;
            position.y = state->position.y;
            position.z = state->position.z;
            scale = rsin((D_800E27EC << 10) / 12);
            state->radius = scale / 8;
            func_800CF3AC(D_800E166C, &color, D_800E27EC);
            func_800D0728(&position, 300, 512, 12, 0, scale, scale, 0,
                          &color, 128, 1);
        }
        D_800F3368.depth = 64;
        gte_ldrotmatrix(D_800BCFA4.value);
        gte_ldtransmatrix(D_800BCFA4.value);
        FieldEng_TransformTranslation(&state->position, 0);
        break;
    }
    return 0;
}
