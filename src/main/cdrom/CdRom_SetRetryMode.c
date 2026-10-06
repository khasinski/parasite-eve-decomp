extern unsigned int D_8009B6B8;

void DsReadMode(unsigned int mode) {
    if (mode < 2) {
        D_8009B6B8 = mode;
    }
}
