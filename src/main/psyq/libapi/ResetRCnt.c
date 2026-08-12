
#include "pe1/psyq_api_internal.h"

int ResetRCnt(unsigned int counter) {
    int id;

    id = counter & 0xFFFF;
    if (id >= 3) {
        return 0;
    }
    D_8009B7D0[id].rootCounter = 0;
    return 1;
}
