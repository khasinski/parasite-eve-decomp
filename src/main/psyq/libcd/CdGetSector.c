#include "pe1/psyq_cd.h"

int CD_getsector(void *address, int size);

int CdGetSector(void *address, int size) {
    return CD_getsector(address, size) == 0;
}
