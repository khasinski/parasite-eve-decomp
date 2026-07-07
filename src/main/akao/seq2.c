typedef unsigned int u32;

#include "pe1/akao.h"

extern char g_AkaoVoiceStateTable[];
extern char *g_AkaoCurTrack;

void Util_CopyWords(unsigned int *src, unsigned int *dst, unsigned int size);
void Akao_StepSequencerVoice(void *arg0);

extern int g_AkaoSeqLoopCounter;
void Seq_SelectPlaybackBank(int *arg0);

void Seq_StartNestedStreams(void *arg0, int arg1, int arg2);

void Seq_RestoreSecondaryStateAndSelect(int *arg0) {
    char *state;

    state = g_AkaoCurTrack;
    if ((*(u32 *)(state + 4) != 0) && (*(u32 *)(state + 0x6C) == 0)) {
        Util_CopyWords(state, state + 0x68, 0x68);
        Util_CopyWords(g_AkaoVoiceStateTable, g_AkaoVoiceStateTable + 0x1AA0, 0x1AA0);
    }

    Akao_StepSequencerVoice(arg0[1]);
    ((AkaoTrack *)g_AkaoCurTrack)->parent_track_id = arg0[3];
}

void Seq_SelectPlaybackBankWithCountdown(int *arg0)
{
  int raw;
  register int value;
  Seq_SelectPlaybackBank(arg0);
  raw = arg0[4] & 0xFFFFFFFFFFFFFFFFu;
  value = 0;
  if (raw != 0)
  {
    value = raw - 1;
  }
  g_AkaoSeqLoopCounter = value;
}

void Seq_StartDefaultNestedStream(int *arg0) {
    int old1 = arg0[1];
    int old2 = arg0[2];

    arg0[1] = 0x400;
    arg0[2] = 0x1000000;
    arg0[3] = 0x80;
    arg0[4] = 0x7F;
    Seq_StartNestedStreams(arg0, old1, old2);
}
