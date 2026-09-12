#include "pe1/gte.h"
#include "pe1/gte_types.h"

GteVector *ApplyMatrix(const GteMatrix *matrix, const GteShortVector *v,
                       GteVector *out) {
    const int *m = (const int *)matrix->m;
    register int r0 asm("$8") = m[0];
    register int r1 asm("$9") = m[1];
    register int r2 asm("$10") = m[2];
    register int r3 asm("$11") = m[3];
    register int r4 asm("$12") = m[4];
    GteVector *ret;
    gte_ctc2_0(r0);
    gte_ctc2_1(r1);
    gte_ctc2_2(r2);
    gte_ctc2_3(r3);
    gte_ctc2_4(r4);
    gte_lwc2_0_0(v);
    gte_lwc2_1_4(v);
    gte_cop2_hazard_slot();
    gte_mvmva_rotation_v0_sf12();
    gte_swc2_25_0(out);
    gte_swc2_26_4(out);
    gte_swc2_27_8(out);
    asm volatile("" : "=r"(ret) : "0"(out));
    return ret;
}
