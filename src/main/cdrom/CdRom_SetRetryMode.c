extern int D_8009B6B8;

void CdRom_SetRetryMode(unsigned int arg0) {
    if (arg0 < 2) {
        D_8009B6B8 = arg0;
    }
}
