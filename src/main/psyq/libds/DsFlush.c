#include "pe1/psyq_ds.h"
#include "pe1/cdrom.h"

extern void DS_stop(void);

extern int g_CdDsReadQueueState;
extern int g_CdDsReadIndex;
extern int g_CdPendingReadCount;
extern CdDsReadQueueEntry g_CdDsReadQueue[];

void DsFlush(void) {
    int i;
    register CdDsReadQueueEntry *p asm("$4");
    int j;
    unsigned char *q;

    DS_stop();

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
        p->parameter = 0;
        p->callback = 0;
        p->count = 0;
        asm volatile("" : "=r"(i) : "0"(i));
        i++;
        p++;
    }

    DsEndReadySystem();
    DS_restart();
}
