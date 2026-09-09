#include "pe1/psyq_spu_internal.h"

extern short g_SpuReverbDepth[];

void Akao_SetMasterVolume(short left, short right) {
    short *vol;

    _spu_RXX->reverb_volume_left = left;
    _spu_RXX->reverb_volume_right = right;
    asm volatile("" : "=r"(vol) : "0"(g_SpuReverbDepth));
    vol[0] = left;
    vol[1] = right;
}
