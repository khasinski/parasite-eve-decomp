#include "fx_common.h"

void func_80191678(s16 index)
{
    int freeHead;

    D_801E4A88[D_801E4A88[index].previous].next =
        D_801E4A88[index].next;
    D_801E4A88[D_801E4A88[index].next].previous =
        D_801E4A88[index].previous;
    if (index == D_8019C9D0) {
        D_8019C9D0 = D_801E4A88[index].previous;
    }
    freeHead = D_8019CBC0;
    D_801E4A88[index].previous = -1;
    D_801E4A88[index].next = -1;
    D_8019CBC0 = index;
    D_8019C830[index] = freeHead;
}
