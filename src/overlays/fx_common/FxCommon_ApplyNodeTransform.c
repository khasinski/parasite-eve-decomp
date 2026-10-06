#include "fx_common.h"

void func_80190D3C(FxCommonTransformNode *node, void *context)
{
    RotMatrix(&node->seed.room, &node->matrix);
    *D_8019BFF0 = node->matrix;
    CompMatrix(&D_8019CC30, D_8019BFF0, D_8019BFF0);
    SetTransMatrix(D_8019BFF0);
    SetRotMatrix(D_8019BFF0);
    FxCommon_DrawPolyResource(context, node->resource);
}
