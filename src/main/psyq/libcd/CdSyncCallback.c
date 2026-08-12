
extern int D_8009AFB4;

int CdSyncCallback(int callback) {
    int old;

    old = D_8009AFB4;
    D_8009AFB4 = callback;
    return old;
}
/* ASSEMBLER: gas */
