#include "pe1/akao.h"

void sndTrackReadPanpot(AkaoTrack *track) {
    unsigned char *cursor = track->pc;

    track->pc = cursor + 1;
    track->panpot_step = *cursor;
}
