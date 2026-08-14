#include "pe1/psyq_cd.h"

extern void CD_sync(int, int);
extern void CD_ready(int, int);
extern int g_CdDiscType;

void CdRom_Sync(int result);
void CdRom_SendReadyCommand(int result);

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
