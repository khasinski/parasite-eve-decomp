/* GCC_VERSION: 2.8.1 */
#include "pe1/psyq_cd.h"

extern unsigned char g_CdLastCmd;

int CdRom_GetLastCmd(void) {
    return g_CdLastCmd;
}

extern unsigned char g_CdCmdMode;

int CdRom_GetCmdMode(void) {
    return g_CdCmdMode;
}

CdlLOC *CdRom_GetCurrentPosPtr(void) {
    return &g_CdCurPosPtr;
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
