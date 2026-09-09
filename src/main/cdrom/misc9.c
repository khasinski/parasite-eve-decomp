/* CC1_FLAGS: -O1 */
#include "pe1/psyq_cd.h"
void CdRom_AsyncCallback(u_char event, u_char *result);

register volatile int g_CdCallback asm("$4");

int DsRead_IsBusy(void);

void CdRom_SeekDoneCallback(void) {
    g_CdCallback = (unsigned char)g_CdCallback;
    if (g_CdCallback == 2) {
        g_CdCallback = 0x80080000;
        g_CdCallback += 0x1E70;
        DsSyncCallback((DsEventCallback)g_CdCallback);
    }
}

int DsRead_IsBusy(void) {
    return g_DsReadBusy;
}
