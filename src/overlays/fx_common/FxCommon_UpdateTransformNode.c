#include "fx_common.h"
#include "pe1/gte.h"

/* Rebuild a transform node's matrix when it is on screen (or forced), apply
 * the mirror matrix for mirrored nodes, then draw it with the renderer that
 * matches its pass and mirror state.  `state` holds the visibility test and
 * then the pass. */
void func_80190E04(FxCommonTransformNode *node, FxCommonBuffer *context, u8 pass, u8 force,
                   u8 mirrored)
{
    GteVector position;
    GteMatrix mirror;
    int state;

    mirror = D_8018F014;
    position.x = node->matrix.t[0];
    position.y = node->matrix.t[1];
    position.z = node->matrix.t[2];
    state = FxCommon_CheckFourBoundsWithMargin(&position.x, node->margin) | force;
    if (state) {
        func_800794C4(&node->seed.room, &node->matrix);
        if (mirrored == 1) {
            gte_CompMatrix(&mirror, &node->matrix, &node->matrix);
        }
        *D_8019BFF0 = node->matrix;
        func_800787D4(&D_8019CC30, D_8019BFF0, D_8019BFF0);
        func_80078E94(D_8019BFF0);
        func_80078E04(D_8019BFF0);
        state = pass;
        if (state == 0) {
            if (mirrored == 0)
                FxCommon_DrawModel(context, node->resource, node->kind);
            if (mirrored == 1)
                FxCommon_DrawModelTinted(context, node->resource, node->kind);
        }
        if (pass == 1)
            FxCommon_DrawModelFlat(context, node->resource, node->kind);
    }
}
