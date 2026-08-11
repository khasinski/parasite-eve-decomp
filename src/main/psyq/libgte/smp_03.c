#include "common.h"
#include "pe1/gte.h"

long RotTransPers3(void *v0, void *v1, void *v2, void *sxy0,
                   void * volatile sxy1, void * volatile sxy2,
                   void * volatile p, void * volatile flag) {
    s32 result;

    gte_ldv0(v0);
    gte_ldv1(v1);
    gte_ldv2(v2);
    gte_rtpt_store_separate(sxy0, sxy1, sxy2, p, flag, result);
    return result >> 2;
}
