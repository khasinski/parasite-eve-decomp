#include "pe1/akao.h"

void SeqOp_SetVolumeOrExpression(AkaoTrack *track)
{
    register unsigned char *cursor asm("$3");
    int value;

    if ((track->flags & 8) != 0) {
        cursor = track->pc;
        track->pc = cursor + 1;
        value = *cursor;
        track->volume = value << 7;
    } else {
        register unsigned char *cursor2 asm("$3");
        register unsigned char *next asm("$2");
        register unsigned int flags asm("$2");
        register int value2 asm("$3");

        cursor2 = track->pc;
        next = cursor2 + 1;
        track->pc = next;
        value2 = *(signed char *)cursor2;
        flags = track->update_flags;
        track->expression_duration = 0;
        flags |= AKAO_VOICE_PARAM_VOLUME;
        value2 <<= 23;
        track->update_flags = flags;
        track->expression_value = value2;
    }
}
