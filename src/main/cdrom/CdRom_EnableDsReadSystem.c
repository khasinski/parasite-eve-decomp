
#include "pe1/psyq_cd.h"

typedef struct DsSystemDataPage {
    char reserved00[4];
    CdRomSystemState state;
    char reserved54[0x4A5C];
} DsSystemDataPage;

register DsSystemDataPage *g_DsSystemWritePage asm("$1");
register int g_DsSystemEnabledValue asm("$2");

void CdRom_EnableDsReadSystem(void);

void CdRom_EnableDsReadSystem(void) {
    g_DsSystemEnabledValue = 1;
    g_DsSystemWritePage = (DsSystemDataPage *)0x800A0000;
    g_DsSystemWritePage[-1].state.enabled = g_DsSystemEnabledValue;
}
