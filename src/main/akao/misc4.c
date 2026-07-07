#include "pe1/akao.h"

extern char *g_AkaoCurTrack;
extern unsigned int g_AkaoSeqPendingFlags;
extern int g_AkaoTrackStateArray[];

void Seq_MarkTrack34MaskDirty(void);
void Seq_MarkTrack38MaskDirty(void);
void Seq_MarkTrack3CMaskDirty(void);

typedef unsigned int u32;
extern char g_AkaoVoiceChannelTable[];
extern u32 g_SpuActiveVoiceMask;
extern u32 g_SpuStoppedVoiceMask;
extern u32 g_AkaoSeqPendingFlags_1 __asm__("g_AkaoSeqPendingFlags");
void AkaoSpuVoice_SetVolume(unsigned int index, unsigned int left, unsigned int right);
void AkaoSpuVoice_SetPitch(unsigned int index, unsigned int value);
void AkaoSpuVoice_SetAdsrAttack(unsigned int index, unsigned int left, unsigned int right);
void AkaoSpuVoice_SetAdsrSustainRate(unsigned int index, unsigned int left, unsigned int right);

void Seq_RestorePendingTracks(void) {
    unsigned int pending = *(unsigned int *)(g_AkaoCurTrack + 0x1C);
    unsigned int saved;
    unsigned int mask;
    int *slot;

    if (pending != 0) {
        mask = 1;
        slot = g_AkaoTrackStateArray;
        do {
            if (pending & mask) {
                pending &= ~mask;
                *slot |= AKAO_VOICE_PARAM_RESUME;
            }
            mask <<= 1;
            slot = (int *)((char *)slot + sizeof(AkaoTrack));
        } while (pending != 0);

        saved = *(unsigned int *)(g_AkaoCurTrack + 0x1C);
        *(unsigned int *)(g_AkaoCurTrack + 0x1C) = 0;
        *(unsigned int *)(g_AkaoCurTrack + 4) = saved;
        Seq_MarkTrack34MaskDirty();
        Seq_MarkTrack38MaskDirty();
        Seq_MarkTrack3CMaskDirty();
    }

    g_AkaoSeqPendingFlags &= ~1;
}

void Spu_VoiceStopAll(void)
{
  u32 pending;
  u32 bit;
  u32 voice_index;
  register u32 *active_ptr;
  u32 active;
  u32 new_var;
  u32 inverse;
  char *voice;
  u32 i;
  pending = g_SpuActiveVoiceMask;
  if (pending != 0)
  {
    voice = g_AkaoVoiceChannelTable;
    bit = AKAO_SPU_VOICE_SFX_START_MASK;
    i = 0;
    do
    {
      if ((pending & bit) != 0)
      {
        if (((*((u32 *) (voice + 0x2C))) & 0x02000000) != 0)
        {
          pending &= ~bit;
        }
      }
      i++;
      voice += sizeof(AkaoTrack);
      bit <<= 1;
    }
    while (i < 12);
    bit = AKAO_SPU_VOICE_SFX_START_MASK;
    voice_index = AKAO_SPU_VOICE_SFX_START_INDEX;
    active_ptr = &g_SpuActiveVoiceMask;
    g_SpuStoppedVoiceMask = pending;
    new_var = *active_ptr;
    active = new_var;
    inverse = ~pending;
    active &= inverse;
    *active_ptr = active;
    if (pending != 0)
    {
      do
      {
        if ((pending & bit) != 0)
        {
          AkaoSpuVoice_SetVolume(voice_index, 0, 0);
          AkaoSpuVoice_SetPitch(voice_index, 0);
          AkaoSpuVoice_SetAdsrAttack(voice_index, 0x7F, 1);
          AkaoSpuVoice_SetAdsrSustainRate(voice_index, 0x7F, 3);
          pending &= ~bit;
        }
        bit <<= 1;
        voice_index++;
      }
      while (pending != 0);
    }
  }
  g_AkaoSeqPendingFlags_1 |= 2;
}
