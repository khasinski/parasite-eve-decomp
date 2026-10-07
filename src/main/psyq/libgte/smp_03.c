/* PSY-Q LIBGTE SMP_03: RotTransPers3. */
#include "common.h"
#include "pe1/gte.h"

long RotTransPers3(void *v0, void *v1, void *v2, void *sxy0,
                   void * volatile sxy1, void * volatile sxy2,
                   void * volatile p, void * volatile flag) {
    s32 result;
    register void *gte_out1 asm("$8");
    register void *gte_out2 asm("$9");
    register void *gte_depth_out asm("$10");
    register void *gte_flags_out asm("$11");
    int gte_flags;
    int gte_depth_value;

    gte_ldv0(v0);
    gte_ldv1(v1);
    gte_ldv2(v2);
    gte_rtpt();
    gte_bind_separate_outputs(sxy1, sxy2, p, flag);
    gte_stsxy3(sxy0, gte_out1, gte_out2);
    gte_stir0(gte_depth_out);
    gte_getflag(gte_flags);
    gte_getsz3(gte_depth_value);
    *(s32 *)gte_flags_out = gte_flags;
    asm volatile("" : : : "memory");
    result = gte_depth_value;
    return result >> 2;
}
