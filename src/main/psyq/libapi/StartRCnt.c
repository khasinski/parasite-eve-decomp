/* MASPSX_FLAGS: --store-return-delay */

#include "pe1/psyq_api_internal.h"

int StartRCnt(unsigned int counter) {
    int id;

    id = counter & 0xFFFF;
    _interrupt_status_register[1] |= _interrupt_status_masks[id];
    return id < 3;
}
