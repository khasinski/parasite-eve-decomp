extern int D_800B8AB4;

int DsSyncCallback(int callback) {
    int *slot;
    int old;

    slot = &D_800B8AB4;
    old = *slot;
    *slot = callback;
    return old;
}
