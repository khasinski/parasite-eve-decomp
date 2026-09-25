/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -fcall-used-$1 */
#include "pe1/psyq_cd.h"

void CdRom_EnableDsReadSystem(void);

void CdRom_EnableDsReadSystem(void) {
    register int enabled asm("$2");
    enabled = 1;
    g_DsReadSysEnabled.enabled = enabled;
}

int CdRom_IsDsReadSystemEnabled(void) {
    register int scratch asm("$1");
    int enabled;

    /* Keep $at live so the load uses $v0 as both base and destination. */
    asm volatile("" : "=r"(scratch));
    enabled = g_DsReadSysEnabled.enabled;
    asm volatile("" : : "r"(scratch));
    return enabled;
}
