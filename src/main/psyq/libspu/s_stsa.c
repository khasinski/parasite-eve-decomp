/* PSY-Q LIBSPU S_STSA: SpuSetTransferStartAddr. */
#include "common.h"
#include "pe1/psyq_spu_internal.h"


int SpuSetTransferStartAddr(int arg0) {
    int result;

    if (((unsigned int)arg0 - 0x1010) <= 0x7EFE8) {
        g_SpuTransferAddr = _spu_FsetRXXa(-1, arg0);
        /* Reload this halfword after the store, preserving the SDK access sequence. */
        asm volatile("" : "=m"(g_SpuTransferAddr) : "m"(g_SpuTransferAddr));
        result = g_SpuTransferAddr << _spu_mem_mode.shift;
    } else {
        result = 0;
    }
    return result;
}
