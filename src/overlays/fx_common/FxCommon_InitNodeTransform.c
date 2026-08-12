#include "fx_common.h"

void func_80191580(FxCommonTransformNode *node)
{
    node->translation[2] = 1000;
    node->matrix[0] = 0x1000;
    node->matrix[4] = 0x1000;
    node->matrix[8] = 0x1000;
    node->translation[0] = 0;
    node->translation[1] = 0;
    node->matrix[1] = 0;
    node->matrix[2] = 0;
    node->matrix[3] = 0;
    node->matrix[5] = 0;
    node->matrix[6] = 0;
    node->matrix[7] = 0;
    node->extra[0] = 0;
    node->extra[1] = 0;
    node->extra[2] = 0;
    node->record = &D_8019C340;
    node->previous = 0;
    node->next = 0;
}
