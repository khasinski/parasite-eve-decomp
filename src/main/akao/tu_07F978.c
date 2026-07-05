#include "pe1/akao.h"

extern int g_AkaoInstrumentTable;
void SeqOp_SetVoiceInstrument(AkaoTrack *track, int arg1, int arg2);

extern int g_SpuActiveVoiceMask;
extern int g_AkaoVoiceMaskScratch;
extern int g_AkaoTrack34Mask;
extern int g_AkaoTrack38Mask;
extern int g_AkaoTrack3CMask;

extern char *g_AkaoCurTrack;

void Akao_SetNotePitch(AkaoTrack *track, int arg1) {
    int ptr;

    track->note_pitch = arg1;
    ptr = (int)&g_AkaoInstrumentTable + (arg1 << 6);
    SeqOp_SetVoiceInstrument(track, ptr, *(int *)ptr);
}

void SeqOp_DeactivateVoice(char *ptr, int mask) {
    int *first = &g_SpuActiveVoiceMask;

    mask = ~mask;
    *first &= mask;
    g_AkaoTrack34Mask &= mask;
    g_AkaoTrack38Mask &= mask;
    g_AkaoTrack3CMask &= mask;
    g_AkaoVoiceMaskScratch &= mask;
    *(int *)(ptr + 0x2C) = 0;
    *(int *)(ptr + 0x28) = 0;
}

void SeqOp_SetTempo(void *ptr)
{
  unsigned char *stream = *((unsigned char **) ptr);
  char *track;
  register unsigned int value;
  *((unsigned char **) ptr) = stream + 1;
  track = g_AkaoCurTrack;
  ;
  *((unsigned int *) (track + 0x20)) = (*stream) << 16;
  stream = *((unsigned char **) ptr);
  *((unsigned char **) ptr) = stream + 1;
  stream = (unsigned char *) ((unsigned int) (*stream));
  value = *((unsigned int *) (track + 0x20));
  *((short *) (track + 0x52)) = 0;
  value |= ((unsigned int) stream) << 24;
  *((unsigned int *) (track + 0x20)) = value;
}
