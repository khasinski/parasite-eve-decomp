/* MASPSX_FLAGS: --store-return-delay */

extern int D_8009AFB4;

int CdSyncCallback(int callback) {
    int old;

    old = D_8009AFB4;
    D_8009AFB4 = callback;
    return old;
}
