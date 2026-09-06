/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */

#include "common.h"

extern s32 D_8009B418;
extern u16 g_SpuTransferAddr;
extern s32 _spu_mem_mode_plus;
int _spu_t(int command, ...);
void _spu_FwriteByIO(void *address, u32 size);

u32 _spu_Fw(void *address, u32 size)
{
    if (D_8009B418 == 0) {
        _spu_t(2, g_SpuTransferAddr << _spu_mem_mode_plus);
        _spu_t(1);
        _spu_t(3, address, size);
    } else {
        _spu_FwriteByIO(address, size);
    }
    return size;
}
