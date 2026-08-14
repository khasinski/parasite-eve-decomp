#include "pe1/psyq_cd.h"

int CdRom_GetSeekState(void) {
    return g_CdSeekState.eventStatus;
}
