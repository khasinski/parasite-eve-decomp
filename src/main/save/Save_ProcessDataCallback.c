/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */

#include "pe1/psyq_cd.h"
#include "pe1/psyq_ds_queue.h"

void CdRom_ResetFileDescriptors(void);
int func_8007FCBC(int mode, int unused);

void Save_ProcessDataCallback(void) {
    int *readInProgress;
    int particleType;
    void *zeroArg1;
    DslCB callback;

    readInProgress = &g_CdReadInProgress;

    if (*readInProgress == 1) {
        CdRom_ResetFileDescriptors();
        DS_read_cbready();
        if (((CdReadProgressState *)((char *)readInProgress -
            PE1_OFFSETOF(CdReadProgressState, inProgress)))->flags & 1) {
            DsDataCallback(((CdReadProgressState *)((char *)readInProgress -
            PE1_OFFSETOF(CdReadProgressState, inProgress)))->dataCallback);
        }
        func_8007FCBC(1, 0);
        particleType = 9;
        zeroArg1 = 0;
        asm volatile("" : "+r"(particleType), "+r"(zeroArg1));
        callback = 0;
        Render_AllocParticleNode(particleType, zeroArg1, callback, -1);
    }

    g_CdReadInProgress = 0;
    asm volatile("" : : : "memory");
}
