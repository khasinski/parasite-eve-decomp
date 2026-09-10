#include "pe1/psyq_cd.h"

extern int D_8009AFD8;
extern unsigned char *D_8009AF1C;
extern unsigned char *D_8009AF28;

void StUnSetRing(void) {
    unsigned char *interrupt_enable;

    EnterCriticalSection();
    if (D_8009AFD8 == 1) {
        DsDataCallback(0);
        DsSyncCallback(0);
    } else {
        CdDataCallback(0);
        CdReadyCallback(0);
    }
    *D_8009AF1C = 0;
    interrupt_enable = D_8009AF28;
    asm volatile("nop");
    *interrupt_enable = 0;
    ExitCriticalSection();
}
