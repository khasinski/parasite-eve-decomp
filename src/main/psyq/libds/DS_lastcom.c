/* GCC_VERSION: 2.8.1 */
/* Psy-Q LIBDS DSSYS_1.OBJ: DS_lastcom, DS_lastmode, DS_lastpos, DS_lastseek,
 * DS_lastread, DS_status. */
#include "pe1/psyq_cd.h"
#include "pe1/cdrom.h"

extern unsigned char g_CdLastCmd;

int DS_lastcom(void) {
    return g_CdLastCmd;
}

extern unsigned char g_CdCmdMode;

int DS_lastmode(void) {
    return g_CdCmdMode;
}

CdlLOC *DS_lastpos(void) {
    return &g_CdCurPosPtr;
}

extern unsigned char g_CdRetryCount;

int DS_lastseek(void) {
    return g_CdRetryCount;
}

extern unsigned char g_CdCmdParam;

int DS_lastread(void) {
    return g_CdCmdParam;
}

int DS_status(void) {
    return g_CdSeekState.eventStatus;
}
