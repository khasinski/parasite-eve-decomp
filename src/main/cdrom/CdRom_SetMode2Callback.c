#include "pe1/psyq_cd.h"

void CdRom_SetMode2Callback(unsigned char arg0) {
    if (arg0 == 2) {
        CdRom_InitAsyncRead(CdRom_ReadProgressCallback, -1);
    }
}
