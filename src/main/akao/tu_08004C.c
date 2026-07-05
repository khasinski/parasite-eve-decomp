#include "pe1/akao.h"

void sndTrackReadPanpot(AkaoTrack *track) {
    unsigned char *cursor = track->pc;

    track->pc = cursor + 1;
    track->panpot_step = *cursor;
}

void sndTrackIncPanpot(AkaoTrack *track) {
    track->panpot_step = (track->panpot_step + 1) & 0xF;
}

void sndTrackDecPanpot(AkaoTrack *track) {
    track->panpot_step = (track->panpot_step - 1) & 0xF;
}
