#include "pe1/psyq_callbacks.h"
#include "pe1/psyq_spu_internal.h"

void _SpuDataCallback(SpuCallback callback) {
    DMACallback(4, callback);
}
