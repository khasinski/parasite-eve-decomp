/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */

#include "common.h"
#include "pe1/psyq_spu_internal.h"

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

/* Retail places a zero alignment word between the adjacent SDK routines. */
static unsigned int Spu_ReadFromSpu_alignment __attribute__((section(".text"))) = 0;

u32 Spu_UploadToSpu(void *address, u32 size) {
    if (size > 0x7EFF0) {
        size = 0x7EFF0;
    }

    _spu_Fw(address, size);
    if (_spu_transferCallback == 0) {
        D_8009B430 = 0;
    }
    return size;
}
