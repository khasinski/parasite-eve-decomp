#include "fx_common.h"

int func_801915DC(void);
void func_801917BC(FxCommonNode *node, FxCommonNode *parent);

FxCommonNode *func_80190AEC(FxCommonNode *parent, void *payload)
{
    int index = func_801915DC();
    FxCommonNode *node = (FxCommonNode *)&D_801E4E00[(s16)index];

    node->type = 0;
    node->args.payload = payload;
    node->id = index;
    func_801917BC(node, parent);
    node->state[0] = 0;
    node->state[1] = 0;
    node->state[2] = 0;
    return node;
}
