/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */

#include "pe1/psyq_cd.h"

extern int g_DsReadBusy;

int DsSyncCallback(int callback);
int DsReadyCallback(int callback);
int Render_AllocParticleNode(int type, int arg1, int arg2, int arg3);

void DsReadBreak(void) {
    int *state;
    int particleType;
    int zeroArg1;
    int zeroArg2;
    state = &g_DsReadBusy;
    if (state[0] == 1) {
        DsSyncCallback(state[-3]);
        DsReadyCallback(state[-2]);
        particleType = 9;
        zeroArg1 = 0;
        asm volatile("" : "+r"(particleType), "+r"(zeroArg1));
        zeroArg2 = 0;
                Render_AllocParticleNode(particleType, zeroArg1, zeroArg2, -1);
    }
    state[0] = 0;
}
