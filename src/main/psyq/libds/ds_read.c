/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */

#include "pe1/psyq_ds.h"
#include "pe1/cdrom.h"

CdlLOC *DsIntToPos(int sector, CdlLOC *position);

int ds_read(int count, int sector, void *destination) {
    CdlLOC position;
    int status;

    DsIntToPos(sector, &position);
    DsRead(&position, count, destination, 0x80);
    do {
        status = DsReadSync(0);
    } while (status > 0);

    return status == 0;
}
