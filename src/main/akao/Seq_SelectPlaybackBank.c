typedef unsigned short u16;

#include "pe1/akao.h"

extern u16 g_AkaoSelectedBankId;
extern char *g_AkaoCurTrack;

void Akao_UpdateVoiceMask(int arg0);
void Akao_StepSequencerVoice(void *arg0);

void Seq_SelectPlaybackBank(int *arg0) {
    u16 value;

    value = g_AkaoSelectedBankId;
    if ((value != 0) && (value == arg0[3])) {
        Akao_UpdateVoiceMask(arg0[1]);
    } else {
        Akao_StepSequencerVoice(arg0[1]);
        ((AkaoTrack *)g_AkaoCurTrack)->parent_track_id = arg0[3];
    }
}
