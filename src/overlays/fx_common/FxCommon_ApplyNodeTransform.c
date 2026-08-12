#include "fx_common.h"

void func_80190D3C(FxCommonTransformNode *node, void *context)
{
    func_800794C4(&node->seed.room, &node->matrix);
    *D_8019BFF0 = node->matrix;
    func_800787D4(&D_8019CC30, D_8019BFF0, D_8019BFF0);
    func_80078E94(D_8019BFF0);
    func_80078E04(D_8019BFF0);
    func_80197BA0(context, node->resource);
}
