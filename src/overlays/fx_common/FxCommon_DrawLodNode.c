#include "fx_common.h"
#include "pe1/gte.h"

/* Rebuild a level-of-detail node's matrix (mode 2 composes it with the mirror
 * matrix and keeps the original translation), then draw the model kind that
 * matches its view depth when it is on screen or forced. */
void func_80191114(FxCommonLodNode *node, FxCommonBuffer *context, u8 force, u8 mode)
{
    GteVector position;
    GteMatrix mirror;
    RoomSpriteMatrix copy;
    RoomFxSeed8 seed;
    s16 margin;
    int depth;

    mirror = D_8018F014;
    position.x = node->matrix.t[0];
    position.y = node->matrix.t[1];
    position.z = node->matrix.t[2];
    margin = node->margin;
    if (mode == 2) {
        seed = node->seed;
        func_800794C4(&seed, &node->matrix);
        copy = node->matrix;
        gte_CompMatrix(&copy, &mirror, &node->matrix);
        node->matrix.t[0] = copy.t[0];
        node->matrix.t[1] = copy.t[1];
        node->matrix.t[2] = copy.t[2];
    } else {
        func_800794C4(&node->seed, &node->matrix);
    }
    *D_8019BFF0 = node->matrix;
    func_800787D4(&D_8019CC30, D_8019BFF0, D_8019BFF0);
    func_80078E94(D_8019BFF0);
    func_80078E04(D_8019BFF0);
    if (FxCommon_CheckBoundsWithMargin(&position.x, margin) | force) {
        depth = D_8019BFF0->t[2];
        if (depth < node->nearDepth) {
            if (depth > 0x180) {
                if (mode == 0)
                    FxCommon_DrawModel(context, node->resource, node->nearKind);
                if (mode == 1)
                    FxCommon_DrawModelTinted(context, node->resource, node->nearKind);
                if (mode == 2)
                    FxCommon_DrawModelTinted(context, node->resource, node->nearKind);
            }
        } else if (depth < node->farDepth) {
            if (mode == 0)
                FxCommon_DrawModel(context, node->resource, node->middleKind);
            if (mode == 1)
                FxCommon_DrawModelTinted(context, node->resource, node->middleKind);
        } else {
            if (mode == 0)
                FxCommon_DrawModel(context, node->resource, node->farKind);
            if (mode == 1)
                FxCommon_DrawModelTinted(context, node->resource, node->farKind);
            if (mode == 2)
                FxCommon_DrawModelTinted(context, node->resource, node->farKind);
        }
    }
}
