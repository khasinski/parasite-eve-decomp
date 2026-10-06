/* Psy-Q LIBDS DSSYS_1.OBJ: DS_sync, DS_ready, DS_shell_open, DS_cw_system. */
#include "pe1/psyq_cd.h"

void DS_sync(u8 *result) {
    CD_sync(1, result);
}

void DS_ready(u8 *result) {
    CD_ready(1, result);
}

extern int g_CdDiscType;

int DS_shell_open(void) {
    return g_CdDiscType;
}

int LIBDS_DSSYS_1_text_368(int arg0);

int DS_cw_system(int arg0) {
    volatile int *ptr;
    ptr = &g_CdRomCmdTimeout;
    if (*ptr > 0) {
        return 0;
    }
    asm volatile("" : "=r"(ptr) : "0"(ptr));
    ptr[-10] = 0x20;
    return LIBDS_DSSYS_1_text_368(arg0 & 0xFF);
}
