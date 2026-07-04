/* CC1_FLAGS: -fno-schedule-insns -fno-schedule-insns2 */
#include "pe1/akao.h"

void SeqOp_SetVolumeOrExpression(AkaoTrack *track)
{
    unsigned char *cursor;
    int value;

    if ((track->flags & 8) != 0) {
        cursor = track->pc;
        track->pc = cursor + 1;
        value = *cursor;
        track->volume = value << 7;
    } else {
        unsigned char *cursor2;
        register unsigned char *next;
        register unsigned int flags;
        int value2;

        cursor2 = track->pc;
        next = cursor2 + 1;
        track->pc = next;
        value2 = *(signed char *)cursor2;
        flags = track->update_flags;
        track->expression_duration = 0;
        flags |= 3;
        value2 <<= 23;
        track->update_flags = flags;
        track->expression_value = value2;
    }
}
