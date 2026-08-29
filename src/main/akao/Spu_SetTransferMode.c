#include "common.h"

/* MASPSX_FLAGS: --stack-return-delay --store-call-delay */

extern s32 D_8009B438;

void _SpuCallback(s32 callback);

s32 Spu_SetTransferMode(s32 callback)
{
    s32 previous;

    previous = D_8009B438;
    if (callback != previous) {
        _SpuCallback(D_8009B438 = callback);
    }
    return previous;
}
