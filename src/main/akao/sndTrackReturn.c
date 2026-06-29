#include "pe1/akao.h"

void sndTrackReturn(AkaoTrack *track) {
    unsigned short index = track->call_stack_index;

    track->repeat_counters[index]++;
    track->pc = track->call_stack[track->call_stack_index];
}
