#include "pe1/psyq_callbacks.h"
typedef void (*SpuCallback)(void);


void _SpuDataCallback(SpuCallback callback) {
    DMACallback(4, callback);
}
