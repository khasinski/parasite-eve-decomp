/* CC1_FLAGS: -ffixed-20 -ffixed-21 -ffixed-22 -ffixed-23 */
/* MASPSX_FLAGS: --expand-div */
#include "pe1/render_object.h"
#include "pe1/gte.h"

int func_800D5898(int mode, RenderCosineEffect *state)
{
    RenderColor color = D_800C22C0;

    switch (mode) {
    case 1:
        state->x = state->amplitude
            * rcos((D_800E27EC << 10) / state->duration) / 4096;
        if (D_800E27EC >= state->duration)
            return 1;
        break;
    case 2:
        func_800D27FC(state->x, state->y, &color,
            rcos((D_800E27EC << 10) / state->duration) / 64 + 64, 1);
        break;
    }
    return 0;
}

/* MASPSX_FLAGS: --expand-div */
#include "pe1/render_object.h"
#include "pe1/field_anim.h"
#include "pe1/random.h"
#include "pe1/field_actor.h"
#include "pe1/gte.h"

/* Matching debt: 3 GTE transfer pins, 4 empty constraints, and the four
 * TU-local register reservations above. The matrix pointer remains unpinned
 * so GCC can also use t0 for the multiply-high result in division by 12. */
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
        {
            s32 **slot;
            const GteMatrixWords *matrix;
            register s32 a asm("$12");
            register s32 b asm("$13");
            register s32 c asm("$14");
            asm volatile("" : : : "memory");
            slot = &D_800BCFA4.value;
            asm volatile("" : "=r"(slot) : "0"(slot));
            matrix = (const GteMatrixWords *)*slot;
            asm("" : "=r"(matrix) : "0"(matrix) : "$2", "$3", "$4", "$5", "$6", "$7");
            a = matrix->r11_r12;
            b = matrix->r13_r21;
            gte_ctc2_0(a);
            gte_ctc2_1(b);
            a = matrix->r22_r23;
            b = matrix->r31_r32;
            c = matrix->r33_pad;
            gte_ctc2_2(a);
            gte_ctc2_3(b);
            gte_ctc2_4(c);
            a = matrix->tx;
            b = matrix->ty;
            gte_ctc2_5(a);
            c = matrix->tz;
            gte_ctc2_6(b);
            gte_ctc2_7(c);
        }
        FieldEng_TransformTranslation(&state->position, 0);
        /* Exclude other reload temporaries without emitting instructions. */
        asm volatile("" : : : "$9", "$10", "$11", "$15", "$24", "$25", "$16", "$17", "$18", "$19");
        break;
    }
    return 0;
}
