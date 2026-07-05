/* MASPSX_FLAGS: --store-return-delay */
extern int g_DsReadSysEnabled;

void CdRom_EnableDsReadSystem(void);

void CdRom_EnableDsReadSystem(void) {
    g_DsReadSysEnabled = 1;
}
