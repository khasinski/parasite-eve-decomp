#include "pe1/gte.h"
#include "pe1/gte_types.h"

int RotTransPers(const GteShortVector *v, s32 *sxy, s32 *p, s32 *flag) {
    int result;
    int flagValue;
    gte_lwc2_0_0(v);
    gte_lwc2_1_4(v);
    gte_cop2_hazard_slot();
    gte_rtps_command();
    gte_stsxy2(sxy);
    gte_stir0(p);
    gte_getflag(flagValue);
    gte_getsz3(result);
    *flag = flagValue;
    return result >> 2;
}
