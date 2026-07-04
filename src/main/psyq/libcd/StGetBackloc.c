/* CC1_FLAGS: -g3 */
/* MASPSX_FLAGS: --stack-return-delay --la-call-delay */

#include "pe1/psyq_cd.h"

int CdPosToInt_Local(CdlLOC *p);
CdlLOC *CdIntToPos_Local(int i, CdlLOC *p);

extern int g_DsStreamNoLocFlag;
extern CdlLOC D_800A3490;
extern int D_800A3494;

int StGetBackloc(CdlLOC *arg0) {
    if (g_DsStreamNoLocFlag != 0) {
        return -1;
    }

    CdIntToPos_Local(CdPosToInt_Local(&D_800A3490) + 1, arg0);
    return D_800A3494;
}
