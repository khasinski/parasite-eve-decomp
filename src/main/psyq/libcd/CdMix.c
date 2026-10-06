/* Psy-Q LIBCD SYS.OBJ: CdMix, CdGetSector, CdGetSector2, CdDataCallback, CdDataSync. */
#include "pe1/psyq_cd.h"

int CD_vol(CdlATV *volume);

int CdMix(CdlATV *volume) {
    CD_vol(volume);
    return 1;
}

int CD_getsector2(void *address, int size);
int CD_getsector(void *address, int size);

int CdGetSector(void *address, int size) {
    return CD_getsector(address, size) == 0;
}

int CdGetSector2(void *address, int size) {
    return CD_getsector2(address, size) == 0;
}

DsCallback CdDataCallback(DsCallback callback) {
    return DMACallback(3, callback);
}

int CdDataSync(int mode) {
    return CD_datasync(mode);
}
