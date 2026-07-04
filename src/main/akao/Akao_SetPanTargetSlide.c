/* MASPSX_FLAGS: --expand-div */

typedef unsigned char u8;
typedef int s32;

#include "pe1/akao.h"

void Akao_SetPanTargetSlide(AkaoTrack *track) {
    int duration;
    int field_d8;
    int next;
    int delta;

    duration = (track->panpot_duration = *track->pc++);
    if (duration == 0) {
        duration = 0x100;
        track->panpot_duration = duration;
    }

    field_d8 = (unsigned short)track->pan_target & 0xFF00;
    next = *track->pc++;
    delta = ((next << 8) - (short)field_d8) / track->panpot_duration;
    track->pan_target = field_d8;
    track->pan_delta = delta;
}
