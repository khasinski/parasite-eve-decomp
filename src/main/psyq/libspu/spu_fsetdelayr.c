#include "pe1/psyq_spu_internal.h"

void _spu_FsetDelayR(void) {
    *_spu_delay = (*_spu_delay & 0xF0FFFFFF) | 0x22000000;
}
