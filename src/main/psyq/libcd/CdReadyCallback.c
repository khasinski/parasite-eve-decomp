/* MASPSX_FLAGS: --store-return-delay */

extern int D_8009AFB8;

int CdReadyCallback(int callback) {
    int old;

    old = D_8009AFB8;
    D_8009AFB8 = callback;
    return old;
}
