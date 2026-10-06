/* ASSEMBLER: GNU */
/* CC1_FLAGS: -O1 */
#include "pe1/psyq_ds.h"
void ER_cbready(int event, u_char *result);

void ER_cbsync(u_char event) {
    if (event == 2) {
        DsReadyCallback((DsEventCallback)ER_cbready);
    }
}

int ER_active(void) {
    return g_DsReadBusy;
}
