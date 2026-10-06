/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -fcall-used-$1 */
/* Psy-Q LIBDS DSSYS_1.OBJ: DS_restart, DS_system_active. */
#include "pe1/psyq_cd.h"
#include "pe1/cdrom.h"

void DS_restart(void) {
    int enabled;
    enabled = 1;
    g_DsReadSysEnabled.enabled = enabled;
}

int DS_system_active(void) {
    int scratch;
    int enabled;

    /* Keep $at live so the load uses $v0 as both base and destination. */
    asm volatile("" : "=r"(scratch));
    enabled = g_DsReadSysEnabled.enabled;
    asm volatile("" : : "r"(scratch));
    return enabled;
}
