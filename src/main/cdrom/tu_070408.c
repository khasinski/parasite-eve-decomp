extern unsigned char g_CdLastCmd;

extern unsigned char g_CdCmdMode;

int CdRom_GetLastCmd(void) {
    return g_CdLastCmd;
}

int CdRom_GetCmdMode(void) {
    return g_CdCmdMode;
}
