#include "fx_common.h"

int func_801915DC(void);
int func_8019959C(void *payload, int type);
void func_801917BC(FxCommonNode *node, FxCommonNode *parent);

FxCommonNode *func_80190B78(FxCommonNode *parent, s16 type, void *payload)
{
    int index = func_801915DC();
    FxCommonNode *node = (FxCommonNode *)&D_801E4E00[(s16)index];

    node->type = type;
    node->args.payload = payload;
    node->id = index;
    node->value32 = func_8019959C(payload, type);
    func_801917BC(node, parent);
    node->state[0] = 0;
    node->state[1] = 0;
    node->state[2] = 0;
    return node;
}
