#include "pe1/akao.h"

void sndTrackReadAdsrSustainLevel(AkaoTrack *track) {
    unsigned char *cursor = track->pc;
    unsigned int flags;
    unsigned char value;

    track->pc = cursor + 1;
    flags = track->update_flags;
    value = *cursor;
    track->update_flags = flags | 0x8000;
    track->adsr_sustain_level = value;
}
