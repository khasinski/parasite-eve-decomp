#include "pe1/psyq_cd.h"

extern void CdRom_AbortCmd(void);
void DsReadBreak(void);
void CdRom_EnableDsReadSystem(void);

extern int g_CdDsReadQueueState;
extern int g_CdDsReadIndex;
extern int g_CdPendingReadCount;
extern CdDsReadQueueEntry g_CdDsReadQueue[];

void CdRom_ResetFileDescriptors(void) {
    int i;
    register CdDsReadQueueEntry *p asm("$4");
    int j;
    unsigned char *q;

    CdRom_AbortCmd();

    i = 0;
    g_CdPendingReadCount = 0;
    g_CdDsReadIndex = 0;
    g_CdDsReadQueueState = 0;
    p = g_CdDsReadQueue;

    while (i < 8) {
        j = 3;
        q = (unsigned char *)p + 3;
        p->active = 0;
        p->command = 0;
        for (; j >= 0; j--, q--) {
            q[5] = 0;
        }
        p->arg0C = 0;
        p->arg10 = 0;
        p->arg14 = 0;
        asm volatile("" : "=r"(i) : "0"(i));
        i++;
        p++;
    }

    DsReadBreak();
    CdRom_EnableDsReadSystem();
}
