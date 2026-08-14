/* CC1_VERSION: 2.8.1 */
#include "pe1/psyq_cd.h"

extern CdlLOC g_CdLastPos;

CdlLOC *CdLastPos(void) {
    return &g_CdLastPos;
}