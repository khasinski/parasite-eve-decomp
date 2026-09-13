/* ASSEMBLER: GNU */
#include "pe1/psyq_spu_internal.h"

/* Historical link name; Psy-Q LIBSPU SpuSetIRQCallback. */
SpuCallback Spu_SetTransferMode(SpuCallback callback) {
    SpuCallback previous = _spu_IRQCallback;
    if (callback != previous) {
        _spu_IRQCallback = callback;
        _SpuCallback(callback);
    }
    return previous;
}
