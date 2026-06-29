#include "pe1/akao.h"

extern int g_AkaoInstrumentTable;
void SeqOp_SetVoiceInstrument(AkaoTrack *track, int arg1, int arg2);

void Akao_SetNotePitch(AkaoTrack *track, int arg1) {
    int ptr;

    track->note_pitch = arg1;
    ptr = (int)&g_AkaoInstrumentTable + (arg1 << 6);
    SeqOp_SetVoiceInstrument(track, ptr, *(int *)ptr);
}
