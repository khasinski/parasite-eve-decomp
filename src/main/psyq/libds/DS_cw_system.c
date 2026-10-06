#include "pe1/psyq_cd.h"

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
