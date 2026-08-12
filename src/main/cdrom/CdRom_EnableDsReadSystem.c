
#include "pe1/psyq_cd.h"

void CdRom_EnableDsReadSystem(void);

void CdRom_EnableDsReadSystem(void) {
    g_DsReadSysEnabled.enabled = 1;
}
/* ASSEMBLER: gas */
