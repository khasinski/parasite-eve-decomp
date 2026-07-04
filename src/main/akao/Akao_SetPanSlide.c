/* MASPSX_FLAGS: --expand-div */

typedef unsigned char u8;
typedef int s32;

#include "pe1/akao.h"

void Akao_SetPanSlide(AkaoTrack *track) {
    int duration;
    int field_76;
    int next;
    int delta;

    duration = (track->panpot_slide_duration = *track->pc++);
    if (duration == 0) {
        duration = 0x100;
        track->panpot_slide_duration = duration;
    }

    field_76 = track->panpot & 0xFF00;
    next = *track->pc++;
    delta = ((((next + 0x40) & 0xFF) << 8) - field_76) / track->panpot_slide_duration;
    track->panpot = field_76;
    track->panpot_delta = delta;
}
