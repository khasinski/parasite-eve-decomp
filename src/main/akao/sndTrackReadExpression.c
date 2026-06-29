#include "pe1/akao.h"

void sndTrackReadExpression(AkaoTrack *track) {
    unsigned char *cursor = track->pc;

    track->pc = cursor + 1;
    track->expression = (signed char)*cursor;
}
