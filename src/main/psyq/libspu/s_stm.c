/* ASSEMBLER: GNU */
#include "pe1/psyq_spu_internal.h"

/* Historical link name; Psy-Q LIBSPU SpuSetTransferMode. */
int Spu_SetReverbMode(int mode) {
    int enabled;
    switch (mode) {
    case 0:
        enabled = 0;
        break;
    case 1:
        enabled = 1;
        break;
    default:
        enabled = 0;
        break;
    }
    D_8009B38C = mode;
    D_8009B418 = enabled;
    return enabled;
}
