/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */

#include "pe1/psyq_cd.h"

extern int D_8009B6CC;

void CdRom_ResetFileDescriptors(void);
void DS_read_cbready(void);
int func_8007FCBC(int mode, int unused);

void Save_ProcessDataCallback(void) {
    int *readInProgress;
    register int particleType asm("$4");
    register void *zeroArg1 asm("$5");
    register int zeroArg2 asm("$6");

    readInProgress = &D_8009B6CC;
    asm volatile("" : "+r"(readInProgress));

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
        zeroArg2 = 0;
        asm volatile("" : "+r"(zeroArg2));
        Render_AllocParticleNode(particleType, zeroArg1, zeroArg2, -1);
    }

    D_8009B6CC = 0;
    asm volatile("" : : : "memory");
}
