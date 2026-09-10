/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -fcall-used-$1 */
#include "pe1/psyq_cd.h"

void StSetMask(u32 mask, u32 start, u32 end) {
    g_CdStreamMask = mask;
    D_800B6918 = start;
    g_CdStreamEndSector = end;
}
