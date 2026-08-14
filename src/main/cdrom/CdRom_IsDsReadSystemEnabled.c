#include "pe1/psyq_cd.h"

int CdRom_IsDsReadSystemEnabled(void) {
    return g_DsReadSysEnabled.enabled;
}
