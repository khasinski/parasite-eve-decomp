#include "pe1/gte.h"
#include "pe1/gte_types.h"

void SetLightMatrix(const GteMatrix *matrix) {
    const GteMatrixWords *m = (const GteMatrixWords *)matrix;
    register int r0 asm("$8") = m->r11_r12;
    register int r1 asm("$9") = m->r13_r21;
    register int r2 asm("$10") = m->r22_r23;
    register int r3 asm("$11") = m->r31_r32;
    register int r4 asm("$12") = m->r33_pad;

    gte_ctc2_8(r0);
    gte_ctc2_9(r1);
    gte_ctc2_10(r2);
    gte_ctc2_11(r3);
    gte_ctc2_12(r4);
}
