/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */

#include "common.h"

extern int D_8009B6CC;

void CdRom_ResetFileDescriptors(void);
void DS_read_cbready(void);
int DsDataCallback(int callback);
int func_8007FCBC(int mode, int unused);
int Render_AllocParticleNode(int type, int arg1, int arg2, int arg3);

void Save_ProcessDataCallback(void) {
    int *readInProgress;
    register int particleType asm("$4");
    register int zeroArg1 asm("$5");
    register int zeroArg2 asm("$6");

    readInProgress = &D_8009B6CC;
    asm volatile("" : "+r"(readInProgress));

    if (*readInProgress == 1) {
        CdRom_ResetFileDescriptors();
        DS_read_cbready();
        if (readInProgress[-5] & 1) {
            DsDataCallback(readInProgress[-3]);
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
