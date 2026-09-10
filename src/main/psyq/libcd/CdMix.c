#include "pe1/psyq_cd.h"

int CD_vol(CdlATV *volume);

int CdMix(CdlATV *volume) {
    CD_vol(volume);
    return 1;
}
