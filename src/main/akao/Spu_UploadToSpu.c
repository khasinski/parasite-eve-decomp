/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */

#include "common.h"

extern s32 D_8009B430;
extern s32 D_8009B434;

void _spu_Fw(void *address, u32 size);

u32 Spu_UploadToSpu(void *address, u32 size) {
    if (size > 0x7EFF0) {
        size = 0x7EFF0;
    }

    _spu_Fw(address, size);
    if (D_8009B434 == 0) {
        D_8009B430 = 0;
    }
    {
        register u32 result asm("$2") = size;
        return result;
    }
}
