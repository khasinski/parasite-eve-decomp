/* MASPSX_FLAGS: --la-return-delay */
extern unsigned char g_CdLastPos[];

unsigned char *CdLastPos(void) {
    return g_CdLastPos;
}
