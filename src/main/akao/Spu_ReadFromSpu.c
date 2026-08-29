#include "common.h"

/* MASPSX_FLAGS: --stack-return-delay */

extern s32 D_8009B430;
extern s32 D_8009B434;

void _spu_Fr(s32 address, u32 size);

u32 Spu_ReadFromSpu(s32 address, u32 size)
{
    if (size > 0x7EFF0) {
        size = 0x7EFF0;
    }
    _spu_Fr(address, size);
    if (D_8009B434 == 0) {
        D_8009B430 = 0;
    }
    return size;
}
