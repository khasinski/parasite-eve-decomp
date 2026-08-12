#include "fx_common.h"

int func_801915DC(void)
{
    s16 index = D_8019CBC0;
    s16 current = D_8019C9D0;
    u16 freeNext;
    u16 next;
    int nextIndex;

    freeNext = D_8019C830[index];
    D_801E4A88[index].previous = current;
    next = D_801E4A88[current].next;
    D_801E4A88[index].next = next;
    D_801E4A88[current].next = index;
    nextIndex = D_801E4A88[index].next;
    D_8019C9D0 = index;
    D_8019CBC0 = freeNext;
    D_801E4A88[nextIndex].previous = index;
    return index;
}
