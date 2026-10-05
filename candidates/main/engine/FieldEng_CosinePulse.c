/* WIP candidate, not promoted: func_800D5A00 has linked score 10.
 * The 740-byte emitter differs only at 800D5BE4/800D5BE8: the division
 * high result uses t1 instead of retail t0. The unchanged sibling is 360 bytes.
 * All matrix CPU loads are C; GTE transfers have individual macros.
 * Matching debt: three word-register pins and three empty barriers.
 * Verified on darwine with stock native GCC 2.7.2 and MASPSX 2.56.
 * 64 constraint-removal subsets and 112 successfully compiled reservation /
 * constraint-form trials did not improve the score. A bounded permuter run
 * completed 78,709 iterations (14,175 rejected compilations), also without
 * improvement, and stopped. No worker from that run remains active.
 * Research: scratch/cosine_pulse, scratch/cosine_focus; remote equivalents
 * under /home/hasik/fx-search-archives. Do not promote without exact bytes.
 */
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
            register u32 a asm("$12");
            register u32 b asm("$13");
            register u32 c asm("$14");
            asm volatile("" : : : "memory");
            slot = &D_800BCFA4.value;
            asm volatile("" : "=r"(slot) : "0"(slot));
            matrix = (const GteMatrixWords *)*slot;
            asm volatile("" : "=r"(matrix) : "0"(matrix) : "$2", "$3", "$4", "$5", "$6", "$7");
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
        break;
    }
    return 0;
}
