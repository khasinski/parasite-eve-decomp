/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */

#include "pe1/psyq_ds.h"
#include "pe1/cdrom_runtime.h"

CdlLOC *CdIntToPos(int sector, CdlLOC *position);
int CdRom_StartRead(CdlLOC *position, int count, int destination, int mode);

int ds_read(int count, int sector, void *destination) {
    CdlLOC position;
    int status;

    CdIntToPos(sector, &position);
    CdRom_StartRead(&position, count, (int)destination, 0x80);
    do {
        status = Sys_VSyncTimeout(0);
    } while (status > 0);

    return status == 0;
}
