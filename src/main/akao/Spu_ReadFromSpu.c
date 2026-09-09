/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */

#include "common.h"
#include "pe1/psyq_spu_internal.h"

extern s32 D_8009B430;

u32 Spu_ReadFromSpu(void *address, u32 size) {
    if (size > 0x7EFF0) {
        size = 0x7EFF0;
    }

    _spu_Fr(address, size);
    if (_spu_transferCallback == 0) {
        D_8009B430 = 0;
    }
    return size;
}
