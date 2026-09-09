/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -fcall-used-$1 */
#include "pe1/psyq_cd.h"

void StSetMask(int mask, int start, int end) {
    g_CdStreamMask = mask;
    D_800B6918 = start;
    g_CdStreamEndSector = end;
}
