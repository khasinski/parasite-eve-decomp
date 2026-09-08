#include "pe1/psyq_cd.h"

int DsStartCallback(int callback) {
    int *slot;
    int old;

    slot = &g_DsReadCallbackState[0];
    old = *slot;
    *slot = callback;
    return old;
}

int DsSyncCallback(int callback) {
    int *slot;
    int old;

    slot = &g_DsReadCallbackState[1];
    old = *slot;
    *slot = callback;
    return old;
}

int DsReadyCallback(int callback) {
    int *slot;
    int old;

    slot = &g_DsReadCallbackState[2];
    old = *slot;
    *slot = callback;
    return old;
}

int DMACallback(int channel, int callback);

int DsDataCallback(int callback) {
    return DMACallback(3, callback);
}
