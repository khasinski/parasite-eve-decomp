/* MASPSX_FLAGS: --expand-div */

typedef unsigned char u8;
typedef signed char s8;

#include "pe1/akao.h"

void Akao_SetExpressionSlide(AkaoTrack *track) {
    register u8 *pc asm("$2");
    u8 *pc2;
    int duration;
    int current;
    int next;

    pc = track->pc;
    track->pc = pc + 1;
    duration = pc[0];
    track->expression_duration = duration;
    if (duration == 0) {
        duration = 0x100;
        track->expression_duration = duration;
    }

    pc2 = track->pc;
    track->pc = pc2 + 1;
    next = ((s8)pc2[0]) << 23;
    current = track->expression_value & 0xFFFF0000;
    track->expression_value = current;
    track->expression_delta = (next - current) / track->expression_duration;
}
