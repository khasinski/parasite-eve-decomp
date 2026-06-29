#include "pe1/akao.h"

void sndTrackReadDetune(AkaoTrack *track) {
    unsigned char *cursor = track->pc;

    track->pc = cursor + 1;
    track->detune = (signed char)*cursor;
}
