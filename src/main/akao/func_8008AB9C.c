#include "pe1/akao.h"

typedef unsigned int u32;

extern char *D_8009D2C8;

void func_8008AB9C(AkaoTrack *track) {
    u32 mask;
    u32 bit;

    mask = *(u32 *)(D_8009D2C8 + 4);
    if (mask != 0) {
        bit = 1;
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
