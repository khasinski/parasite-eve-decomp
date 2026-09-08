#include "pe1/psyq_cd.h"

typedef struct CdCurrentPosPage {
    char reserved00[0x4A7E];
} CdCurrentPosPage;

register CdCurrentPosPage *g_CdCurrentPosPage asm("$2");

extern unsigned char g_CdLastCmd;

int CdRom_GetLastCmd(void) {
    return g_CdLastCmd;
}

extern unsigned char g_CdCmdMode;

int CdRom_GetCmdMode(void) {
    return g_CdCmdMode;
}

CdlLOC *CdRom_GetCurrentPosPtr(void) {
    g_CdCurrentPosPage = (CdCurrentPosPage *)0x800A0000;
    return (CdlLOC *)&g_CdCurrentPosPage[-1];
}

extern unsigned char g_CdRetryCount;

int CdRom_GetRetryCount(void) {
    return g_CdRetryCount;
}

extern unsigned char g_CdCmdParam;

int CdRom_GetCmdParam(void) {
    return g_CdCmdParam;
}

int CdRom_GetSeekState(void) {
    return g_CdSeekState.eventStatus;
}
