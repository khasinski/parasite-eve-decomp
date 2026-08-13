
#include "pe1/psyq_cd.h"

extern int D_8009AFB8;
register CdCallbackDataPage *g_CdReadyCallbackWritePage asm("$1");

int CdReadyCallback(int callback) {
    int old;

    old = D_8009AFB8;
    g_CdReadyCallbackWritePage = (CdCallbackDataPage *)0x800A0000;
    g_CdReadyCallbackWritePage[-1].ready = callback;
    return old;
}
