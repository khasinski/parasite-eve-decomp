#include "pe1/psyq_cd.h"

int CdStatus(void) {
    return *(u8 *)&D_8009AFC4;
}

int CdMode(void) {
    return g_CdMode;
}

int CdLastCom(void) {
    return g_CdLastCom;
}
