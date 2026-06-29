/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

#include "pe1/akao.h"

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
