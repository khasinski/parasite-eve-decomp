
/* CC1_FLAGS: -fno-schedule-insns */
/* CC1_FLAGS: -fno-schedule-insns2 */

#include "pe1/psyq_cd.h"

extern void Cd_SetIntrMask(void);
extern void InterruptCallback(int event, void (*callback)(void));
extern void ResetCallback(void);

extern int g_CdReadyCallback;
extern int g_CdSyncCallback;
extern int g_CdResultByte;

register CdCallbackDataPage *g_CdInitCallbackPage asm("$1");

void CD_initintr(void) {
    g_CdReadyCallback = 0;
    g_CdSyncCallback = 0;
    g_CdResultByte = 0;
    g_CdInitCallbackPage = (CdCallbackDataPage *)0x800A0000;
    g_CdInitCallbackPage[-1].status = 0;
    ResetCallback();
    InterruptCallback(2, Cd_SetIntrMask);
}
