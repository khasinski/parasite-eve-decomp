/* GCC_VERSION: 2.8.1 */
/* Psy-Q LIBDS DSSYS_4.OBJ: DsControl, DsControlB. */
#include "pe1/psyq_ds_queue.h"

int DsControl(unsigned char command, void *param, void *result) {
    int request;
    DslCB callback = 0;
    unsigned char status;
    asm("" : "+r"(callback));
    request = DsCommand(command, param, callback, 0);
    if (!request) return 0;
    do {
        status = DsSync(request, result);
    } while (!status);
    return status == 2;
}

int DsControlB(unsigned char command, void *param, void *result) {
    int request;
    DslCB callback = 0;
    unsigned char status;
    asm("" : "+r"(callback));
    request = DsCommand(command, param, callback, 0);
    if (!request) return 0;
    do {
        status = DsSync(request, result);
    } while (!status);
    return status == 2;
}
