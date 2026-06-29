#include "pe1/akao.h"

void Akao_SetReleaseRate(AkaoTrack *track) {
    unsigned char *cursor;
    unsigned int flags;
    unsigned char value;

    track->flags |= 0x200;
    cursor = track->pc;
    track->pc = cursor + 1;
    flags = track->update_flags;
    value = *cursor;
    track->update_flags = flags | 0x4400;
    track->adsr_release_rate = value;
}
