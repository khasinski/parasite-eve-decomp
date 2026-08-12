#include "fx_common.h"

void func_801917BC(FxCommonNode *node, FxCommonNode *parent)
{
    int bucket;

    node->parent = parent;
    node->bucket = parent->bucket + 1;
    bucket = node->bucket;
    node->previous = D_8019CCBC[bucket].head;
    node->next = node->previous->next;
    D_8019CCBC[bucket].head->next = node;
    D_8019CCBC[bucket].head = node;
}
