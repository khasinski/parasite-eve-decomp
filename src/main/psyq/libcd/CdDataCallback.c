#include "pe1/psyq_cd.h"

DsCallback CdDataCallback(DsCallback callback) {
    return DMACallback(3, callback);
}
