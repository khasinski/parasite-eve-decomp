#include "pe1/gte.h"

void SetColorMatrix(int *m) {
    register int r0 asm("$8") = m[0];
    register int r1 asm("$9") = m[1];
    register int r2 asm("$10") = m[2];
    register int r3 asm("$11") = m[3];
    register int r4 asm("$12") = m[4];

    gte_ctc2_16(r0);
    gte_ctc2_17(r1);
    gte_ctc2_18(r2);
    gte_ctc2_19(r3);
    gte_ctc2_20(r4);
}
