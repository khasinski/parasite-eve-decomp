/* ASSEMBLER: GNU */
/* Psy-Q LIBDS DSSYS_4.OBJ: DsControlF, DsControl, DsControlB and the object's zero tail. */
#include "pe1/psyq_ds_queue.h"

int DsControlF(u_char command, u_char *parameter) {
    return DsCommand(command, parameter, 0, 0);
}

int DsControl(unsigned char command, void *param, void *result) {
    int request;
    DslCB callback = 0;
    unsigned char status;
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
    request = DsCommand(command, param, callback, 0);
    if (!request) return 0;
    do {
        status = DsSync(request, result);
    } while (!status);
    return status == 2;
}

unsigned int LIBDS_DSSYS_4_pad[] __attribute__((section(".text"))) = {
    0x00000000,
    0x00000000,
};
