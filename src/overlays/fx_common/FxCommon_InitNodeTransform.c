#include "fx_common.h"

void func_80191580(FxCommonTransformNode *node)
{
    node->matrix.t[2] = 1000;
    node->matrix.m[0][0] = 0x1000;
    node->matrix.m[1][1] = 0x1000;
    node->matrix.m[2][2] = 0x1000;
    node->matrix.t[0] = 0;
    node->matrix.t[1] = 0;
    node->matrix.m[0][1] = 0;
    node->matrix.m[0][2] = 0;
    node->matrix.m[1][0] = 0;
    node->matrix.m[1][2] = 0;
    node->matrix.m[2][0] = 0;
    node->matrix.m[2][1] = 0;
    node->seed.component[0] = 0;
    node->seed.component[1] = 0;
    node->seed.component[2] = 0;
    node->record = &D_8019C340;
    node->previous = 0;
    node->next = 0;
}
