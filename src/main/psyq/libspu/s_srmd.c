/* PSY-Q LIBSPU S_SRMD: SpuSetReverbModeDepth. */
#include "pe1/psyq_spu_internal.h"

void SpuSetReverbModeDepth(short left, short right) {
    short *vol;

    _spu_RXX->reverb_volume_left = left;
    _spu_RXX->reverb_volume_right = right;
    asm volatile("" : "=r"(vol) : "0"(D_8009B3A0.depth));
    vol[0] = left;
    vol[1] = right;
}

unsigned int gap_akao_Akao_SetMasterVolume_tail_7A750[] __attribute__((section(".text"))) = {
    0x00000000,
    0x00000000,
};
