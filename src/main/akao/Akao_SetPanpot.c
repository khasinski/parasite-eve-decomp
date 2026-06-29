#include "pe1/akao.h"

void Akao_SetPanpot(AkaoTrack *track)
{
    unsigned char *cursor = track->pc;
    int value;

    track->pc = cursor + 1;
    value = *cursor;
    track->panpot_slide_duration = 0;
    value += 0x40;
    track->update_flags |= 3;
    value &= 0xFF;
    track->panpot = value << 8;
}
