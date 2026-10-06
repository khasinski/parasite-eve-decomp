/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */

#include "pe1/psyq_cd.h"
#include "pe1/psyq_ds_queue.h"

void DsFlush(void);
int DS_cw_system(int mode, int unused);

void DsReadBreak(void) {
    int *readInProgress;
    int particleType;
    void *zeroArg1;
    DslCB callback;

    readInProgress = &g_CdReadInProgress;

    if (*readInProgress == 1) {
        DsFlush();
        ER_clear();
        if (((CdReadProgressState *)((char *)readInProgress -
            PE1_OFFSETOF(CdReadProgressState, inProgress)))->flags & 1) {
            DsDataCallback(((CdReadProgressState *)((char *)readInProgress -
            PE1_OFFSETOF(CdReadProgressState, inProgress)))->dataCallback);
        }
        DS_cw_system(1, 0);
        particleType = 9;
        zeroArg1 = 0;
        asm volatile("" : "+r"(particleType), "+r"(zeroArg1));
        callback = 0;
        DsCommand(particleType, zeroArg1, callback, -1);
    }

    g_CdReadInProgress = 0;
    asm volatile("" : : : "memory");
}
