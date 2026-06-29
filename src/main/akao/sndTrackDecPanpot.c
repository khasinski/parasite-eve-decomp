#include "pe1/akao.h"

void sndTrackDecPanpot(AkaoTrack *track) {
    track->panpot_step = (track->panpot_step - 1) & 0xF;
}
