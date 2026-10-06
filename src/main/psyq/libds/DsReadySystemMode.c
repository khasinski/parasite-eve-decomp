/* Psy-Q LIBDS DSREADY.OBJ: DsReadySystemMode. */
#include "pe1/psyq_ds.h"

extern DsCallback D_8009B708;

DsCallback DsReadySystemMode(DsCallback callback) {
    DsCallback *slot;
    DsCallback old;

    slot = &D_8009B708;
    old = *slot;
    *slot = callback;
    return old;
}
