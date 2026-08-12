#include "fx_common.h"

int func_801915DC(void);
int func_8019959C(void *payload, int subtype);
void func_801917BC(FxCommonNode *node, FxCommonNode *parent);

FxCommonNode *func_80190C1C(FxCommonNode *parent, s16 subtype, s16 variant,
                            s16 type, void *data38, void *data3C,
                            void *payload)
{
    int index = func_801915DC();
    FxCommonNode *node = (FxCommonNode *)&D_801E4E00[(s16)index];

    node->args.configured.subtype = subtype;
    node->args.configured.variant = variant;
    node->type = type;
    node->id = index;
    node->args.configured.payload = payload;
    node->value36 = func_8019959C(payload, subtype);
    node->data38 = data38;
    node->data3C = data3C;
    func_801917BC(node, parent);
    node->state[1] = 0;
    node->state[2] = 0;
    node->state[3] = 0;
    return node;
}
