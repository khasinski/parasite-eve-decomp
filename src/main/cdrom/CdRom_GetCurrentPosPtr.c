/* MASPSX_FLAGS: --la-return-delay */
extern unsigned char g_CdCurPosPtr[];

unsigned char *CdRom_GetCurrentPosPtr(void) {
    return g_CdCurPosPtr;
}
