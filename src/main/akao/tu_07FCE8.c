/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --expand-div --use-comm-section -G8 */

#include "pe1/akao.h"

typedef unsigned char u8;

void sndTrackReadVolume(AkaoTrack *track) {
    unsigned char *stream = track->pc;
    unsigned int flags;
    int value;

    track->pc = stream + 1;
    flags = track->update_flags;
    value = *stream;
    track->update_flags = flags | 3;
    track->volume_base = value << 8;
}

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
