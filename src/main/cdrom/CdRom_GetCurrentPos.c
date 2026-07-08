/* MASPSX_FLAGS: --stack-return-delay */

#include "pe1/psyq_cd.h"

CdlLOC *CdRom_GetCurrentPosPtr(void);

CdlLOC *CdRom_GetCurrentPos(CdlLOC *dst);

CdlLOC *CdRom_GetCurrentPos(CdlLOC *dst) {
    if (dst != 0) {
        *dst = *CdRom_GetCurrentPosPtr();
        return dst;
    }

    return CdRom_GetCurrentPosPtr();
}
