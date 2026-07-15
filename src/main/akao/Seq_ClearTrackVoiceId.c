#include "pe1/akao.h"

typedef unsigned int u32;

extern char *D_8009D2C8;

void Seq_ClearTrackVoiceId(AkaoTrack *track, unsigned int voice_id) {
    unsigned int i;
    int bit;
    int reset;
    char *state;

    if (voice_id < 24) {
        i = 0;
        reset = 24;
        state = D_8009D2C8;
        bit = 1;
        do {
            if (track->assigned_voice_index == voice_id) {
                track->assigned_voice_index = reset;
                *(u32 *)(state + 0x14) &= ~(bit << i);
            }
            i++;
            track++;
        } while (i < 24);
    }
}
