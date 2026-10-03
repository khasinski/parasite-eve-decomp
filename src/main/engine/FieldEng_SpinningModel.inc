/* Spinning model flare: the stage 0 leader follows SPINNING_MODEL_ANCHOR and
 * drops shrinking stage 1 copies; each draws the D_800F32D8 model with a
 * size-dependent colour, scaled vertically by its height.
 * SPINNING_MODEL_FUNC       entry point
 * SPINNING_MODEL_ANCHOR     s16[3] anchor position
 * SPINNING_MODEL_PALETTE    palette offset (the special palette adds 4)
 * SPINNING_MODEL_COLOR(s)   r, g, b arguments for func_800C7098 */
#include "pe1/render_object.h"
#include "pe1/field_anim.h"
#include "pe1/field_model_draw.h"
#include "pe1/gte.h"
#include "pe1/psyq_gpu.h"
#include "pe1/render_prim.h"

int SPINNING_MODEL_FUNC(int mode, FieldAnimSpinningModel *state)
{
    GteShortVector angles;
    GteMatrix matrix;
    GteVector scale;
    FieldAnimSpinningModel *copy;
    int tpage;
    int kind;
    int palette;

    switch (mode) {
    case 1:
        if (state->stage == 0) {
            state->x = SPINNING_MODEL_ANCHOR[0];
            state->z = SPINNING_MODEL_ANCHOR[2];
            state->y = SPINNING_MODEL_ANCHOR[1];
            state->angle += 32 + D_800E27EC * 4;
            state->size = rsin((D_800E27EC << 11) / 24) / 32;
            state->height = (D_800E27EC << 12) / 24;
            if (D_800E27EC & 1) {
                copy = func_800CE610(D_800F33E0->end);
                if (copy) {
                    copy->stage = 1;
                    copy->x = state->x;
                    copy->y = state->y;
                    copy->z = state->z;
                    copy->size = state->size;
                    copy->height = state->height;
                }
            }
            if (D_800E27EC >= 24)
                return 1;
        } else {
            state->angle += 3;
            state->size = state->size * 3 / 4;
            if (D_800E27EC >= 4)
                return 1;
        }
        state->ticks++;
        break;
    case 2:
        scale.z = 0x400;
        scale.x = 0x400;
        scale.y = state->height / 4;
        angles.x = 0;
        angles.y = state->angle;
        angles.z = 0;
        RotMatrixYXZ(&angles, &matrix);
        Gte_ScaleMatrix(&matrix, &scale);
        matrix.t[0] = state->x;
        matrix.t[1] = state->y;
        matrix.t[2] = state->z;
        D_800F3368.tpage = D_800E2850[D_800E11E4[9]];
        D_800F3368.palette = 1;
        func_800CEDA8(1);
        D_800F3368.parameter06 = 1;
        gte_ldrotmatrix(D_800BCFA4.value);
        gte_ldtransmatrix(D_800BCFA4.value);
        tpage = (u16)(D_800E2850[D_800E11E4[9]] | GetTPage(0, 1, 0, 0));
        kind = D_800F336C;
        palette = D_800E1204[kind];
        GsSetOrign(tpage, GetClut(0, (kind == 4 && D_800F3428) ? palette + SPINNING_MODEL_PALETTE + 4 : palette + SPINNING_MODEL_PALETTE));
        func_800C6ED8(1);
        func_800C6EF8(D_800F32D8);
        func_800C7098(D_800F32D8, SPINNING_MODEL_COLOR(state->size));
        func_800C71E4(D_800F32D8, &matrix);
        func_800C6F4C(D_800F32D8);
        break;
    }
    return 0;
}

#undef SPINNING_MODEL_FUNC
#undef SPINNING_MODEL_ANCHOR
#undef SPINNING_MODEL_PALETTE
#undef SPINNING_MODEL_COLOR
