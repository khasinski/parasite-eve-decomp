/* MASPSX_FLAGS: --store-return-delay */
extern unsigned int g_DsPollCallback;

void CdRom_SetPollCallback(unsigned int value) {
    g_DsPollCallback = value;
}
