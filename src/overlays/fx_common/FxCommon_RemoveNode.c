#include "fx_common.h"

void func_80191834(FxCommonNode *node)
{
    node->previous->next = node->next;
    node->next->previous = node->previous;
}
