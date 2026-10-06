/* Psy-Q LIBDS DSCB.OBJ: DsSyncCallback, DsReadyCallback, DsStartCallback, DsDataCallback. */
#include "pe1/psyq_ds.h"

DsEventCallback DsSyncCallback(DsEventCallback callback) {
    DsEventCallback *slot;
    DsEventCallback old;

    slot = &g_DsReadCallbackState.sync;
    old = *slot;
    *slot = callback;
    return old;
}

DsEventCallback DsReadyCallback(DsEventCallback callback) {
    DsEventCallback *slot;
    DsEventCallback old;

    slot = &g_DsReadCallbackState.ready;
    old = *slot;
    *slot = callback;
    return old;
}

DsEventCallback DsStartCallback(DsEventCallback callback) {
    DsEventCallback *slot;
    DsEventCallback old;

    slot = &g_DsReadCallbackState.start;
    old = *slot;
    *slot = callback;
    return old;
}

DsCallback DsDataCallback(DsCallback callback) {
    return DMACallback(3, callback);
}
