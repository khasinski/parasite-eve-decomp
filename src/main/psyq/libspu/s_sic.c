/* ASSEMBLER: GNU */
/* PSY-Q LIBSPU S_SIC: SpuSetIRQCallback. */
#include "pe1/psyq_spu_internal.h"

SpuCallback SpuSetIRQCallback(SpuCallback callback) {
    SpuCallback previous = _spu_IRQCallback;
    if (callback != previous) {
        _spu_IRQCallback = callback;
        _SpuCallback(callback);
    }
    return previous;
}
