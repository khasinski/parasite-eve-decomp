#include "pe1/psyq_spu_internal.h"
#include "pe1/psyq_callbacks.h"
/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
typedef struct ReverbState {
    int mode;
    short depth[2];
    int delay;
    int feedback;
} ReverbState;
extern unsigned short D_8009B3B8[24];
extern ReverbState D_8009B3A0;
extern int D_8009B390, D_8009B394, D_8009B398, D_8009B46C;
extern int D_8009B45C, D_8009B460, D_8009B464, D_8009B38C;
extern int D_8009B418, D_8009B388, D_8009B3B4, D_8009B3B0, D_8009B3E8;
void SpuStart(void);

void _SpuInit(int mode) {
    int i;
    ResetCallback();
    _spu_init(mode);
    if (!mode) {
        unsigned short value = 0xC000;
        unsigned short *base = D_8009B3B8;
        i = 23;
        do {
            base[i] = value;
            --i;
        } while (i >= 0);
    }
    SpuStart();
    D_8009B390 = 0;
    D_8009B394 = 0;
    D_8009B3A0.mode = 0;
    D_8009B3A0.depth[0] = 0;
    D_8009B3A0.depth[1] = 0;
    D_8009B3A0.delay = 0;
    D_8009B3A0.feedback = 0;
    D_8009B398 = D_8009B46C;
    _spu_FsetRXX(0xD1, D_8009B398, 0);
    D_8009B45C = 0;
    D_8009B460 = 0;
    D_8009B464 = 0;
    D_8009B38C = 0;
    D_8009B418 = 0;
    D_8009B388 = 0;
    D_8009B3B4 = 0;
    D_8009B3B0 = 0;
    D_8009B3E8 = 0;
    asm volatile("" ::: "memory");
}
