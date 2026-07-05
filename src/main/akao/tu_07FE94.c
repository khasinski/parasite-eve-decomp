#include "pe1/akao.h"

void sndTrackSkip2(int *arg0)
{
    *arg0 += 2;
}

void sndTrackNoop(void)
{
}

void Akao_SetPanTarget(AkaoTrack *track)
{
    unsigned char *cursor = track->pc;
    unsigned int flags;
    unsigned char value;

    track->pc = cursor + 1;
    value = *cursor;
    flags = track->flags;
    track->panpot_duration = 0;
    track->pan_target = value << 8;

    if (flags & 0x100) {
        track->update_flags |= 3;
    }
}
