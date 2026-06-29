/* MASPSX_FLAGS: --expand-div */

typedef unsigned char u8;

#include "pe1/akao.h"

void sndTrackSlideVolume(AkaoTrack *track) {
    u8 *pc;
    int duration;
    int current;
    int next;

    {
        register u8 *pc0 asm("$2");

        pc0 = track->pc;
        track->pc = pc0 + 1;
        duration = pc0[0];
    }
    track->volume_duration = duration;
    if (duration == 0) {
        duration = 0x100;
        track->volume_duration = duration;
    }

    pc = track->pc;
    current = track->volume_base & 0x7F00;
    track->pc = pc + 1;
    next = pc[0] << 8;
    track->volume_base = current;
    track->volume_delta = (next - current) / track->volume_duration;
}
