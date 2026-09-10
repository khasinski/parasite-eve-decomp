#include "pe1/psyq_cd.h"

extern unsigned char g_CdLastCom;

int CdLastCom(void) {
    return g_CdLastCom;
}
