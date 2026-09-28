#include "pe1/psyq_cd.h"

int CD_getsector(void *address, int size);
int CD_getsector2(void *address, int size);

int CdGetSector2(void *address, int size) {
    return CD_getsector2(address, size) == 0;
}

int CdGetSector(void *address, int size) {
    return CD_getsector(address, size) == 0;
}
