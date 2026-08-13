
/* CC1_FLAGS: -fno-schedule-insns2 */

#include "pe1/psyq_cd.h"

extern void CD_flush(void);
extern void VSyncCallbacks(int, int);

extern int g_CdSyncCallback;
extern int g_CdReadyCallback;

typedef struct CdRomSystemDataPage {
    CdRomSystemState state;
    char reserved50[0x4A5C];
} CdRomSystemDataPage;

register CdRomSystemDataPage *g_CdRomSystemDataPage asm("$1");
register int g_CdRomClearCallbackArg asm("$4");

void CdRom_ClearDsCallbacks(void);

void CdRom_ClearDsCallbacks(void) {
    g_CdRomSystemDataPage = (CdRomSystemDataPage *)0x800A0000;
    g_CdRomSystemDataPage[-1].state.enabled = 0;
    CD_flush();
    g_CdRomClearCallbackArg = 0;
    g_CdSyncCallback = 0;
    g_CdReadyCallback = 0;
    VSyncCallbacks(g_CdRomClearCallbackArg, 0);
}
