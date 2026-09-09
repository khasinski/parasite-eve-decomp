#include "pe1/psyq_cd.h"

void CdRom_Sync(u8 *result) {
    CD_sync(1, result);
}

void CdRom_SendReadyCommand(u8 *result) {
    CD_ready(1, result);
}
