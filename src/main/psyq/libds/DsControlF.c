#include "pe1/psyq_ds_queue.h"

int DsControlF(u_char command, u_char *parameter) {
    return DsCommand(command, parameter, 0, 0);
}
