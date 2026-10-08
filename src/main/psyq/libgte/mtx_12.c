/* PSY-Q LIBGTE MTX_12: SetTransMatrix. */
#include "pe1/gte.h"
#include "pe1/gte_types.h"

void SetTransMatrix(const GteMatrix *m) {
    register int tx asm("$8") = m->t[0];
    register int ty asm("$9") = m->t[1];
    register int tz asm("$10") = m->t[2];

    gte_ctc2_5(tx);
    gte_ctc2_6(ty);
    gte_ctc2_7(tz);
}
