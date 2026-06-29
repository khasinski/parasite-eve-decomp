extern int CdRom_GetLastCmd(void);

int CdRom_GetLastCmdByte(void) {
    return CdRom_GetLastCmd() & 0xFF;
}
