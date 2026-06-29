#include "pe1/akao.h"

void sndTrackReadAdsrDecayRate(AkaoTrack *track) {
    unsigned char *cursor = track->pc;
    unsigned int flags;
    unsigned char value;

    track->pc = cursor + 1;
    flags = track->update_flags;
    value = *cursor;
    track->update_flags = flags | 0x1000;
    track->adsr_decay_rate = value;
}
