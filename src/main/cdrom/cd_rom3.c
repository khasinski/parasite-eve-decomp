
#include "pe1/psyq_cd.h"

typedef struct DsCallbackSetPage {
    char reserved00[0x36A0];
    unsigned int poll;
    unsigned int sync;
    unsigned int ready;
    unsigned int dispatch;
} DsCallbackSetPage;

typedef struct CdCurrentPosPage {
    char reserved00[0x4A7E];
} CdCurrentPosPage;

register DsCallbackSetPage *g_DsCallbackSetPage asm("$1");
register CdCurrentPosPage *g_CdCurrentPosPage asm("$2");

extern u32 g_DsReadStatusBlock[];

extern unsigned char g_CdLastCmd;

extern unsigned char g_CdCmdMode;

extern unsigned char g_CdRetryCount;

extern unsigned char g_CdCmdParam;


extern void CD_sync(int, int);

void CdRom_Sync(int result);

extern void CD_ready(int, int);

void CdRom_SendReadyCommand(int result);

extern int g_CdDiscType;

void CdRom_SetPollCallback(unsigned int value) {
    g_DsCallbackSetPage = (DsCallbackSetPage *)0x800A0000;
    g_DsCallbackSetPage->poll = value;
}

void CdRom_SetSyncCallback(unsigned int value) {
    g_DsCallbackSetPage = (DsCallbackSetPage *)0x800A0000;
    g_DsCallbackSetPage->sync = value;
}

void CdRom_SetReadyCallback(unsigned int value) {
    g_DsCallbackSetPage = (DsCallbackSetPage *)0x800A0000;
    g_DsCallbackSetPage->ready = value;
}

void CdRom_SetDispatchCallback(unsigned int value) {
    g_DsCallbackSetPage = (DsCallbackSetPage *)0x800A0000;
    g_DsCallbackSetPage->dispatch = value;
}

u32 DsSync(u32 mode) {
    u32 offset = mode;
    u32 result;
    /* Match note: keep the address expansion in $v0 without a maspsx temp-dest flag. */
    asm volatile(
        "sll\t%1,%1,2\n\t"
        "lui\t%0,%%hi(g_DsReadStatusBlock)\n\t"
        "addu\t%0,%0,%1\n\t"
        "lw\t%0,%%lo(g_DsReadStatusBlock)(%0)"
        : "=r"(result), "=r"(offset)
        : "1"(offset));
    return result;
}

int CdRom_GetLastCmd(void) {
    return g_CdLastCmd;
}

int CdRom_GetCmdMode(void) {
    return g_CdCmdMode;
}

CdlLOC *CdRom_GetCurrentPosPtr(void) {
    g_CdCurrentPosPage = (CdCurrentPosPage *)0x800A0000;
    return (CdlLOC *)&g_CdCurrentPosPage[-1];
}

int CdRom_GetRetryCount(void) {
    return g_CdRetryCount;
}

int CdRom_GetCmdParam(void) {
    return g_CdCmdParam;
}

int CdRom_GetSeekState(void) {
    return g_CdSeekState.eventStatus;
}

void CdRom_Sync(int result) {
    CD_sync(1, result);
}

void CdRom_SendReadyCommand(int result) {
    CD_ready(1, result);
}

int CdRom_GetDiscType(void) {
    return g_CdDiscType;
}
