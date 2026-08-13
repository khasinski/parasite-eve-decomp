
#include "pe1/psyq_cd.h"

void CD_flush(void);

extern int g_CdReadCallback;
register CdCallbackDataPage *g_CdReadCallbackWritePage asm("$1");

void CdFlush(void) {
    CD_flush();
}

int CdReadCallback(int callback) {
    int old = g_CdReadCallback;
    g_CdReadCallbackWritePage = (CdCallbackDataPage *)0x800A0000;
    g_CdReadCallbackWritePage[-1].read = callback;
    return old;
}
