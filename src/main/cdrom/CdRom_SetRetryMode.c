extern unsigned int D_8009B6B8;

void CdRom_SetRetryMode(unsigned int mode) {
    if (mode < 2) {
        D_8009B6B8 = mode;
    }
}
