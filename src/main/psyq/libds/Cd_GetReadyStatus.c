/* GCC_VERSION: 2.8.1 */

#include "pe1/cdrom.h"

int DsSync(int arg0);

int Cd_GetReadyStatus(void) {
    int status = DsSync(0);

    if (status == 1 && CdRom_GetPendingReadCount() > 0) {
        status = 2;
    }

    return status;
}
