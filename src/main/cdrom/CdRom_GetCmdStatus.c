extern int CdRom_GetSeekState(void);

int CdRom_GetCmdStatus(void) {
    return CdRom_GetSeekState() & 0xFF;
}
