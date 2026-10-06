/* Psy-Q LIBDS DSSYS_2.OBJ: DsQueueLen, DsStatus, DsShellOpen, DsLastCom. */
#include "pe1/cdrom.h"
#include "pe1/psyq_ds.h"

extern int g_CdPendingReadCount;

int DS_status(void);

int DS_shell_open(void);

int DsQueueLen(void) {
    return g_CdPendingReadCount;
}

int DsStatus(void) {
    return DS_status() & 0xFF;
}

int DsShellOpen(void) {
    return DS_shell_open();
}

int DsLastCom(void) {
    return DS_lastcom() & 0xFF;
}
