/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */

#include "pe1/psyq_cd.h"

CdlLOC *CdIntToPos(int sector, CdlLOC *position);
int CdRom_StartRead(CdlLOC *position, int count, int destination, int mode);
int Sys_VSyncTimeout(int argument);

int ds_read(int count, int sector, int destination) {
    CdlLOC position;
    int status;

    CdIntToPos(sector, &position);
    CdRom_StartRead(&position, count, destination, 0x80);
    do {
        status = Sys_VSyncTimeout(0);
    } while (status > 0);

    return status == 0;
}
