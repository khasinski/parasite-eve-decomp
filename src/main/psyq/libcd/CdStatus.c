#include "pe1/psyq_cd.h"

int CdStatus(void) {
    return *(u8 *)&D_8009AFC4;
}
