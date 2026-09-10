#include "common.h"
#include "pe1/gte.h"
#include "pe1/gte_types.h"

s32 RotAverageNclip3(const GteRotation *v0, const GteRotation *v1,
                     const GteRotation *v2, u32 *sxy0, u32 *sxy1,
                     u32 *sxy2, s32 *depth, s32 *average_z, u32 *flags) {
    s32 clip;
    s32 average;
    u32 projection_flags;

    gte_ldv0(v0);
    gte_ldv1(v1);
    gte_ldv2(v2);
    gte_rtpt();

    gte_getflag(projection_flags);
    gte_cop2_hazard_slot();
    *flags = projection_flags;

    gte_nclip_now();
    gte_getmac0(clip);
    gte_cop2_hazard_slot();
    if (clip > 0) {
        gte_stsxy0(sxy0);
        gte_stsxy1(sxy1);
        gte_stsxy2(sxy2);
        gte_stir0(depth);
        gte_avsz3();
        gte_getotz(average);
        gte_cop2_hazard_slot();
        *average_z = average;
    }

    return clip;
}
