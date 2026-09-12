/* ASSEMBLER: GNU */
#include "pe1/psyq_cd.h"

extern void CD_flush(void);
extern char func_800119CC[];
extern char *g_CdCmdNameTable[];
extern char *g_CdIntrStringTable[];

register CdCallbackDataPage *g_CdCallbackWritePage asm("$1");

void CdFlush(void) {
    CD_flush();
}

CdlCB CdReadCallback(CdlCB callback) {
    CdlCB old = g_CdReadCallback;
    g_CdCallbackWritePage = (CdCallbackDataPage *)0x800A0000;
    g_CdCallbackWritePage[-1].read = callback;
    return old;
}

char *CdComstr(unsigned int index) {
    index &= 0xFF;
    if (index >= 0x1C) {
        return func_800119CC;
    }
    return g_CdCmdNameTable[index];
}

char *CdIntstr(unsigned int index) {
    index &= 0xFF;
    if (index >= 7) {
        return func_800119CC;
    }
    return g_CdIntrStringTable[index];
}

int CdSync(int mode, u8 *result) {
    return CD_sync(mode, result);
}

int CdReady(int mode, u8 *result) {
    return CD_ready(mode, result);
}

CdlCB CdSyncCallback(CdlCB callback) {
    CdlCB old;

    old = D_8009AFB4;
    D_8009AFB4 = callback;
    return old;
}

CdlCB CdReadyCallback(CdlCB callback) {
    CdlCB old;

    old = D_8009AFB8;
    D_8009AFB8 = callback;
    return old;
}
