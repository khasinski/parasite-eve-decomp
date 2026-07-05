extern unsigned char g_CdRetryCount;

extern unsigned char g_CdCmdParam;

extern unsigned char g_CdSeekState;

extern void CD_sync(int, int);

void CdRom_Sync(int result);

extern void CD_ready(int, int);

void CdRom_SendReadyCommand(int result);

extern int g_CdDiscType;

int CdRom_GetRetryCount(void) {
    return g_CdRetryCount;
}

int CdRom_GetCmdParam(void) {
    return g_CdCmdParam;
}

int CdRom_GetSeekState(void) {
    return g_CdSeekState;
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
