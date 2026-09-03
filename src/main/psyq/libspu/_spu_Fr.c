/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */

#include "common.h"

extern u16 g_SpuTransferAddr;
extern s32 _spu_mem_mode_plus;

int _spu_t(int command, ...);

s32 _spu_Fr(void *address, s32 size) {
    _spu_t(2, g_SpuTransferAddr << _spu_mem_mode_plus);
    _spu_t(0);
    _spu_t(3, address, size);
    return size;
}
