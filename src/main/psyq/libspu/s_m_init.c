/* CC1_PROFILE: build0001 */
#include "pe1/psyq_spu_internal.h"

long SpuInitMalloc(long num, char *top) {
    if (num > 0) {
        _spu_memList = (SPU_MALLOC *)top;
        _spu_memList[0].addr = 0x40001010;
        _spu_memList[0].size = (0x10000 << _spu_mem_mode_plus) - 0x1010;
        _spu_AllocLastNum = 0;
        _spu_AllocBlockNum = num;
        return num;
    }
    return 0;
}
