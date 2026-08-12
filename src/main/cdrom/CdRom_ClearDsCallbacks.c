
#include "pe1/psyq_cd.h"

extern void CD_flush(void);
extern void VSyncCallbacks(int, int);

extern int g_CdSyncCallback;
extern int g_CdReadyCallback;

void CdRom_ClearDsCallbacks(void);

void CdRom_ClearDsCallbacks(void) {
    g_DsReadSysEnabled.enabled = 0;
    CD_flush();
    g_CdSyncCallback = 0;
    g_CdReadyCallback = 0;
    VSyncCallbacks(0, 0);
}
