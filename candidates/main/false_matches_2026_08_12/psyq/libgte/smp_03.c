#include "common.h"
#include "pe1/gte.h"

long RotTransPers3(void *v0, void *v1, void *v2, void *sxy0,
                   void * volatile sxy1, void * volatile sxy2,
                   void * volatile p, void * volatile flag) {
    s32 result;
    gte_declare_separate_outputs();

    gte_ldv0(v0);
    gte_ldv1(v1);
    gte_ldv2(v2);
    gte_rtpt_separate();
    gte_bind_separate_outputs(sxy1, sxy2, p, flag);
    gte_stsxy0(sxy0);
    gte_stsxy1_bound();
    gte_stsxy2_bound();
    gte_stir0_bound();
    gte_getflag_bound();
    gte_getsz3_bound();
    gte_store_flag_bound();
    result = gte_depth_value;
    return result >> 2;
}
