#include "pe1/akao.h"

void sndTrackReadAdsrSustain(AkaoTrack *track) {
    unsigned char *cursor = track->pc;
    unsigned int flags;
    unsigned char value;

    track->pc = cursor + 1;
    flags = track->update_flags;
    value = *cursor;
    track->update_flags = flags | 0x200;
    track->adsr_sustain = value;
}
