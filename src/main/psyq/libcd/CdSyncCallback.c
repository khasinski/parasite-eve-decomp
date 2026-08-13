
#include "pe1/psyq_cd.h"

extern int D_8009AFB4;
register CdCallbackDataPage *g_CdSyncCallbackWritePage asm("$1");

int CdSyncCallback(int callback) {
    int old;

    old = D_8009AFB4;
    g_CdSyncCallbackWritePage = (CdCallbackDataPage *)0x800A0000;
    g_CdSyncCallbackWritePage[-1].sync = callback;
    return old;
}
