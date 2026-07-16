/* MASPSX_FLAGS: --la-call-delay --stack-return-delay */

#include "pe1/psyq_cd.h"

int CdPosToInt_Local(CdlLOC *p);
CdlLOC *CdIntToPos_Local(int i, CdlLOC *p);

extern int D_800A8020;
extern CdlLOC D_800A3490;
extern int D_800A3494;

int StGetBackloc(CdlLOC *arg0) {
    CdlLOC *saved_arg = arg0;

    if (D_800A8020 != 0) {
        return -1;
    }

    CdIntToPos_Local(CdPosToInt_Local(&D_800A3490) + 1, saved_arg);
    return D_800A3494;
}
