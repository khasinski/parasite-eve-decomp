extern int g_CdPendingReadCount;

asm("CdRom_GetPendingReadCount = CdRom_GetPendingReadCount");

int CdRom_GetPendingReadCount(void);

extern int CdRom_GetSeekState(void);

extern int CdRom_GetDiscType(void);

extern int CdRom_GetLastCmd(void);

int CdRom_GetPendingReadCount(void) {
    return g_CdPendingReadCount;
}

int CdRom_GetCmdStatus(void) {
    return CdRom_GetSeekState() & 0xFF;
}

int CdRom_GetDiskType(void) {
    return CdRom_GetDiscType();
}

int CdRom_GetLastCmdByte(void) {
    return CdRom_GetLastCmd() & 0xFF;
}
