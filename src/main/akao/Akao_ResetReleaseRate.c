typedef unsigned char u8;

#include "pe1/akao.h"

extern u8 D_800B290C[];

void Akao_ResetReleaseRate(AkaoTrack *track) {
    track->flags &= ~0x200;
    track->adsr_release_rate = D_800B290C[track->note_pitch << 6];
    track->update_flags |= 0x4400;
}
