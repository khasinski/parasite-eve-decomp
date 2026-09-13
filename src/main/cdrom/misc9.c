/* ASSEMBLER: GNU */
/* CC1_FLAGS: -O1 */
#include "pe1/psyq_cd.h"
void LIBDS_DSREADY_text_FC(int event, u_char *result);

int DsRead_IsBusy(void);

void CdRom_SeekDoneCallback(u_char event) {
    if (event == 2) {
        DsSyncCallback((DsEventCallback)LIBDS_DSREADY_text_FC);
    }
}

int DsRead_IsBusy(void) {
    return g_DsReadBusy;
}
