/* GCC_VERSION: 2.8.1 */

#include "pe1/cdrom.h"

int DS_system_status(int arg0);

int DsSystemStatus(void) {
    int status = DS_system_status(0);

    if (status == 1 && DsQueueLen() > 0) {
        status = 2;
    }

    return status;
}
