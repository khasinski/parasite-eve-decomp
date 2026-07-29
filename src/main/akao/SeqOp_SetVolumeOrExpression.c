#include "pe1/akao.h"

void SeqOp_SetVolumeOrExpression(AkaoTrack *track) {
    AkaoU32 new_var;
    register unsigned char *cursor;
    int value;

    if ((track->flags & AKAO_TRACK_FLAG_BRANCH_ACTIVE) != 0) {
        cursor = track->pc;
        track->pc = cursor + 1;
        if (1) {
            value = *cursor;
            track->volume = value << 7;
        }
    } else {
        register unsigned char *cursor2;
        register unsigned char *next;
        register unsigned int flags;
        register int value2;

        cursor2 = track->pc;
        next = cursor2 + 1;
        track->pc = next;
        new_var = track->update_flags;
        value2 = *((signed char *)cursor2);
        flags = new_var;
        track->expression_duration = 0;
        flags |= AKAO_VOICE_PARAM_VOLUME;
        value2 <<= 23;
        track->update_flags = flags;
        track->expression_value = value2;
    }
}
