#include "pe1/akao.h"

extern char *g_AkaoCurTrack;

void Seq_SetCurrentTrackField56(AkaoValueCommand *arg0) {
    ((AkaoTrack *)g_AkaoCurTrack)->field_56 = arg0->field_4;
}
extern char *g_AkaoCurTrack;
extern unsigned int g_SpuActiveVoiceMask;
extern unsigned int g_AkaoSeqPendingFlags;

void AkaoSpuVoice_SetVolume(unsigned int index, unsigned int left, unsigned int right);
void AkaoSpuVoice_SetPitch(unsigned int index, unsigned int value);
void AkaoSpuVoice_SetAdsrAttack(unsigned int index, unsigned int left, unsigned int right);
void AkaoSpuVoice_SetAdsrSustainRate(unsigned int index, unsigned int left, unsigned int right);

void Seq_DeactivatePendingTracks(void) {
    unsigned int pending;
    unsigned int mask;
    unsigned int bit;
    unsigned int index;

    if (*(unsigned int *)(g_AkaoCurTrack + 4) != 0) {
        mask = ~g_SpuActiveVoiceMask & 0xFFFFFF;
        if (mask != 0) {
            bit = 1;
            index = 0;
            do {
                if (mask & bit) {
                    AkaoSpuVoice_SetVolume(index, 0, 0);
                    AkaoSpuVoice_SetPitch(index, 0);
                    AkaoSpuVoice_SetAdsrAttack(index, 0x7F, 1);
                    AkaoSpuVoice_SetAdsrSustainRate(index, 0x7F, 3);
                    mask &= ~bit;
                }
                bit <<= 1;
                index++;
            } while (mask != 0);
        }

        pending = *(unsigned int *)(g_AkaoCurTrack + 4);
        *(unsigned int *)(g_AkaoCurTrack + 4) = 0;
        *(unsigned int *)(g_AkaoCurTrack + 0x1C) = pending;
    }

    g_AkaoSeqPendingFlags |= 1;
}
#include "pe1/akao.h"

extern char *g_AkaoCurTrack;
extern unsigned int g_AkaoSeqPendingFlags;
extern AkaoTrackUpdateSlot g_AkaoTrackStateArray[];

void Seq_MarkTrack34MaskDirty(void);
void Seq_MarkTrack38MaskDirty(void);
void Seq_MarkTrack3CMaskDirty(void);

typedef unsigned int u32;
extern char g_AkaoVoiceChannelTable[];
extern u32 g_SpuActiveVoiceMask;
extern u32 g_SpuStoppedVoiceMask;
void AkaoSpuVoice_SetVolume(unsigned int index, unsigned int left, unsigned int right);
void AkaoSpuVoice_SetPitch(unsigned int index, unsigned int value);
void AkaoSpuVoice_SetAdsrAttack(unsigned int index, unsigned int left, unsigned int right);
void AkaoSpuVoice_SetAdsrSustainRate(unsigned int index, unsigned int left, unsigned int right);

void Seq_RestorePendingTracks(void) {
    unsigned int pending = *(unsigned int *)(g_AkaoCurTrack + 0x1C);
    unsigned int saved;
    unsigned int mask;
    AkaoTrackUpdateSlot *slot;

    if (pending != 0) {
        mask = 1;
        slot = g_AkaoTrackStateArray;
        do {
            if (pending & mask) {
                pending &= ~mask;
                slot->update_flags |= AKAO_VOICE_PARAM_RESUME;
            }
            mask <<= 1;
            slot++;
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
  g_AkaoSeqPendingFlags |= 2;
}
extern unsigned int g_SpuActiveVoiceMask;
extern unsigned int g_SpuStoppedVoiceMask;
extern unsigned int g_AkaoSeqPendingFlags;
extern int g_SpuVoiceControlTable[];

void Seq_MarkTrack34MaskDirty(void);
void Seq_MarkTrack38MaskDirty(void);
void Seq_MarkTrack3CMaskDirty(void);

#include "pe1/akao.h"
#include "pe1/akao/spu_common.h"

#include "pe1/battle_cmd.h"

extern AkaoQueueEntry D_800B8628[];
extern int g_AkaoMessageQueueCount;

void Seq_GetGlobalPitch(unsigned int *out);

void Spu_RestoreStoppedVoices(void) {
    unsigned int pending = g_SpuStoppedVoiceMask;
    unsigned int saved;
    unsigned int mask;
    int *slot;

    if (pending != 0) {
        mask = AKAO_SPU_VOICE_SFX_START_MASK;
        slot = g_SpuVoiceControlTable;
        do {
            if (pending & mask) {
                pending &= ~mask;
                *slot |= AKAO_VOICE_PARAM_RESUME;
            }
            mask <<= 1;
            slot = (int *)((char *)slot + sizeof(AkaoTrack));
        } while (pending != 0);

        saved = g_SpuStoppedVoiceMask;
        g_SpuStoppedVoiceMask = 0;
        g_SpuActiveVoiceMask = saved;
        Seq_MarkTrack34MaskDirty();
        Seq_MarkTrack38MaskDirty();
        Seq_MarkTrack3CMaskDirty();
    }

    g_AkaoSeqPendingFlags &= ~2;
}

void Akao_MessageNoop(AkaoQueueEntry *entry) {
}

void Akao_ProcessMessageQueue(void) {
    AkaoQueueEntry *entry = D_800B8628;

    if (g_AkaoMessageQueueCount != 0) {
        do {
            Akao_MessageHandlers[entry->opcode](entry);
            g_AkaoMessageQueueCount--;
            entry++;
        } while (g_AkaoMessageQueueCount != 0);
    }
}

void Akao_AllocMessageSlot(AkaoQueueEntry **out_msg) {
    *out_msg = D_800B8628;
    *out_msg = &D_800B8628[g_AkaoMessageQueueCount];
    g_AkaoMessageQueueCount++;
}

void Seq_SetParamWithReset(unsigned int arg0) {
    unsigned int value;

    Seq_GetGlobalPitch(&value);
    if (value != arg0) {
        SpuSetReverb(0);
        SPU_StepDmaRead(arg0 | 0x100);
        SpuSetReverb(1);
    }
}
