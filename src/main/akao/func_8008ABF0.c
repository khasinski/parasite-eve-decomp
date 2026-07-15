#include "pe1/akao.h"

typedef unsigned int u32;

extern u32 D_800BCD50;
extern AkaoTrack D_800BC000[];

void func_8008ABF0(void) {
    u32 mask;
    u32 bit;
    AkaoTrack *track;

    mask = D_800BCD50;
    track = D_800BC000;
    if (mask != 0) {
        bit = 0x1000;
        do {
            if (mask & bit) {
                mask ^= bit;
                track->update_flags |= AKAO_VOICE_PARAM_VOLUME;
            }
            track++;
            bit <<= 1;
        } while (mask != 0);
    }
}
