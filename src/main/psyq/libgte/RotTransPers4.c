
#include "common.h"
#include "pe1/gte.h"

long RotTransPers4(void *v0, void *v1, void *v2, void *v3,
                   void * volatile sxy0, void * volatile sxy1,
                   void * volatile sxy2, void * volatile sxy3,
                   void * volatile p, void * volatile flag) {
    gte_declare_rt4_results(first_flags, flags, result);
    gte_declare_three_outputs();

    gte_ldv0(v0);
    gte_ldv1(v1);
    gte_ldv2(v2);
    gte_rtpt_three_outputs();
    gte_bind_three_outputs(sxy0, sxy1, sxy2);
    gte_stsxy_three_0();
    gte_stsxy_three_1();
    gte_stsxy_three_2();
    gte_getflag(first_flags);

    gte_ldv0(v3);
    gte_rtps_three_outputs();
    gte_bind_three_outputs(sxy3, p, flag);
    gte_stsxy2_three_0();
    gte_stir0_three_1();
    gte_getflag(flags);
    gte_getsz3(result);
    flags |= first_flags;
    gte_store_third_output(flags);
    return result >> 2;
}
