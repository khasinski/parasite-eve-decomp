#include "pe1/akao.h"

void sndTrackReadAdsrReleaseRate(AkaoTrack *track) {
    unsigned char *cursor = track->pc;
    unsigned int flags;
    unsigned char value;

    track->pc = cursor + 1;
    flags = track->update_flags;
    value = *cursor;
    track->update_flags = flags | 0x4400;
    track->adsr_release_rate = value;
}
