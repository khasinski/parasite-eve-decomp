/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -O1 */
/* Semantic reconstruction; epilogue differs. See README.md. */
#include "pe1/psyq_cd.h"

void CdRom_AsyncCallback(u_char event, u_char *result);

void CdRom_SeekDoneCallback(u_char event, u_char *result) {
    if (event == 2) {
        DsSyncCallback(CdRom_AsyncCallback);
    }
}

int DsRead_IsBusy(void) {
    return g_DsReadBusy;
}
