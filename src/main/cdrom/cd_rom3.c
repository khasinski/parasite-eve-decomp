/* MASPSX_FLAGS: --la-return-delay --store-return-delay */

extern unsigned int g_DsPollCallback;

extern unsigned int g_DsSyncCallback;

extern unsigned int g_DsReadyCallback;

extern unsigned int g_DsReadDoneCallback;

typedef unsigned char u8;
typedef unsigned int u32;

extern u32 g_DsReadStatusBlock[];

extern unsigned char g_CdLastCmd;

extern unsigned char g_CdCmdMode;

extern unsigned char g_CdCurPosPtr[];

extern unsigned char g_CdRetryCount;

extern unsigned char g_CdCmdParam;

extern unsigned char g_CdSeekState;

extern void CD_sync(int, int);

void CdRom_Sync(int result);

extern void CD_ready(int, int);

void CdRom_SendReadyCommand(int result);

extern int g_CdDiscType;

void CdRom_SetPollCallback(unsigned int value) {
    g_DsPollCallback = value;
}

void CdRom_SetSyncCallback(unsigned int value) {
    g_DsSyncCallback = value;
}

void CdRom_SetReadyCallback(unsigned int value) {
    g_DsReadyCallback = value;
}

void CdRom_SetDispatchCallback(unsigned int value) {
    g_DsReadDoneCallback = value;
}

u32 DsSync(u32 mode) {
    register u32 offset asm("$4") = mode;
    register u32 result asm("$2");

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

unsigned char *CdRom_GetCurrentPosPtr(void) {
    return g_CdCurPosPtr;
}

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
