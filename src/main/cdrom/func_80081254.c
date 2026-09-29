
#include "pe1/psyq_ds.h"

register CdReadCompleteCallbackPage *g_CdCallbackWritePage asm("$1");

CdReadCompleteCallback func_80081254(CdReadCompleteCallback callback) {
    CdReadCompleteCallback old;

    old = g_CdReadCompleteCallback;
    g_CdCallbackWritePage = (CdReadCompleteCallbackPage *)0x800A0000;
    g_CdCallbackWritePage[-1].callback = callback;
    return old;
}
