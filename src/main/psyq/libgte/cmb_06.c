#include "common.h"
#include "pe1/gte.h"

long RotAverageNclip3(
    void *v0,
    void *v1,
    void *v2,
    void *sxy0,
    void *sxy1,
    void *sxy2,
    void *sz,
    int *otz,
    int *flag) {
    int flagValue;
    long nclipValue;
    int otzValue;

    gte_ldv0(v0);
    gte_ldv1(v1);
    gte_ldv2(v2);
    gte_rtpt();
    gte_getflag(flagValue);
    *flag = flagValue;
    gte_nclip_now();
    gte_getmac0(nclipValue);

    if (nclipValue > 0) {
        gte_stsxy0(sxy0);
        gte_stsxy1(sxy1);
        gte_stsxy2(sxy2);
        gte_stir0(sz);
        gte_avsz3();
        gte_getotz(otzValue);
        *otz = otzValue;
    }

    return nclipValue;
}
