extern int g_CdPendingReadCount;

asm("CdRom_GetPendingReadCount = CdRom_GetPendingReadCount");

int CdRom_GetPendingReadCount(void);

int CdRom_GetPendingReadCount(void) {
    return g_CdPendingReadCount;
}
