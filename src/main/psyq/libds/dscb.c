extern int D_800B8AB4;
extern int D_800B8AB8;

int DsSyncCallback(int callback) {
    int *slot;
    int old;

    slot = &D_800B8AB4;
    old = *slot;
    *slot = callback;
    return old;
}

int DsReadyCallback(int callback) {
    int *slot;
    int old;

    slot = &D_800B8AB8;
    old = *slot;
    *slot = callback;
    return old;
}

int DMACallback(int channel, int callback);

int DsDataCallback(int callback) {
    return DMACallback(3, callback);
}
