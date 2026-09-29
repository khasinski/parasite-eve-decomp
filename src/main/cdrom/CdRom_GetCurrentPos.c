/* GCC_VERSION: 2.8.1 */

#include "pe1/psyq_cd.h"
#include "pe1/cdrom.h"

CdlLOC *CdRom_GetCurrentPos(CdlLOC *dst) {
    if (dst != 0) {
        *dst = *CdRom_GetCurrentPosPtr();
        return dst;
    }

    return CdRom_GetCurrentPosPtr();
}
