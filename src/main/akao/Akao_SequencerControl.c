#include "pe1/akao.h"
#include "pe1/akao/tick.h"
#include "pe1/akao/seq_param.h"

extern int g_AkaoPlaybackMode;
extern char g_AkaoVoiceStateTable[];
extern AkaoSequencerBank *g_AkaoCurTrack;

void Seq_MarkDirtyTracks(char *arg0);
void Spu_MarkActiveVoicesDirty(void);

extern int g_AkaoVoicePortamentoResetMask;
extern AkaoTrackUpdateSlot g_AkaoTrackStateArray[];

void Seq_SetPlaybackMode1AndRefreshVoices(void) {
    char *base = g_AkaoVoiceStateTable;

    g_AkaoPlaybackMode = 1;
    Seq_MarkDirtyTracks(base);
    g_AkaoCurTrack++;
    Seq_MarkDirtyTracks(base + 0x1AA0);
    g_AkaoCurTrack--;
    Spu_MarkActiveVoicesDirty();
}

void Seq_SetPlaybackMode4AndRefreshVoices(void) {
    char *base = g_AkaoVoiceStateTable;

    g_AkaoPlaybackMode = 4;
    Seq_MarkDirtyTracks(base);
    g_AkaoCurTrack++;
    Seq_MarkDirtyTracks(base + 0x1AA0);
    g_AkaoCurTrack--;
    Spu_MarkActiveVoicesDirty();
}

void Seq_SetPlaybackMode2AndRefreshVoices(void) {
    char *base = g_AkaoVoiceStateTable;

    g_AkaoPlaybackMode = 2;
    Seq_MarkDirtyTracks(base);
    g_AkaoCurTrack++;
    Seq_MarkDirtyTracks(base + 0x1AA0);
    g_AkaoCurTrack--;
    Spu_MarkActiveVoicesDirty();
}

void Seq_SetGlobalD2B8AndDirtyAllTracks(AkaoValueCommand *cmd) {
    unsigned int i = 0;
    int value = cmd->field_4;
    AkaoTrackUpdateSlot *slot;

    slot = g_AkaoTrackStateArray;
    g_AkaoVoicePortamentoResetMask = value;
    for (; i < 0x18; i++, slot++) {
        slot->update_flags |= AKAO_VOICE_PARAM_VOLUME;
    }
}

void Seq_SetCurrentTrackField56(AkaoValueCommand *arg0) {
    ((AkaoTrack *)g_AkaoCurTrack)->note_length = arg0->field_4;
}
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

    if (g_AkaoCurTrack->active_voice_mask != 0) {
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

        pending = g_AkaoCurTrack->active_voice_mask;
        g_AkaoCurTrack->active_voice_mask = 0;
        g_AkaoCurTrack->pending_restore_mask = pending;
    }

    g_AkaoSeqPendingFlags |= 1;
}

extern unsigned int g_AkaoSeqPendingFlags;
extern AkaoTrackUpdateSlot g_AkaoTrackStateArray[];

void Seq_MarkTrack34MaskDirty(void);
void Seq_MarkTrack38MaskDirty(void);
void Seq_MarkTrack3CMaskDirty(void);

extern char g_AkaoVoiceChannelTable[];
extern u32 g_SpuActiveVoiceMask;
extern u32 g_SpuStoppedVoiceMask;
void AkaoSpuVoice_SetVolume(unsigned int index, unsigned int left, unsigned int right);
void AkaoSpuVoice_SetPitch(unsigned int index, unsigned int value);
void AkaoSpuVoice_SetAdsrAttack(unsigned int index, unsigned int left, unsigned int right);
void AkaoSpuVoice_SetAdsrSustainRate(unsigned int index, unsigned int left, unsigned int right);

void Seq_RestorePendingTracks(void) {
    unsigned int pending = g_AkaoCurTrack->pending_restore_mask;
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

        saved = g_AkaoCurTrack->pending_restore_mask;
        g_AkaoCurTrack->pending_restore_mask = 0;
        g_AkaoCurTrack->active_voice_mask = saved;
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

#include "pe1/akao/spu_common.h"

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
            Akao_MessageHandlers[entry->opcode.id](entry);
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

int Akao_EnqueueStagedCommand(void) {
    AkaoQueueEntry *msg;
    unsigned short *data;
    unsigned int opcode;
    int *staged;
    int result;
    int param;
    AkaoSequenceCounter sequence;

    result = 0;
    D_8009D268 = 1;
    opcode = g_AkaoCmd.opcode;
    switch (opcode) {
    case 0x10:
    case 0x12:
    case 0x19:
        staged = &g_AkaoCmd.opcode;
        data = g_AkaoCmd.arg0.sample_header;
        if (Spu_ValidateSampleHeader(data) != 0) {
            result = -1;
            break;
        }
        data += 2;
        result = *data;
        data += 2;
        param = *data;
        data += 4;
        if (D_8009D2C8->timing.bank_id != result) {
            Seq_SetParamWithReset(param);
            Akao_AllocMessageSlot(&msg);
            msg->arg0.sample_data = data;
            msg->arg2 = result;
            if (*staged == 0x12)
                msg->arg3 = g_AkaoCmd.arg1;
            msg->opcode.word = *staged;
        } else {
            result = 0;
        }
        break;
    case 0x24:
        Akao_AllocMessageSlot(&msg);
        msg->arg0.value = g_AkaoCmd.arg0.value;
        msg->arg1 = g_AkaoCmd.arg1;
        sequence = D_8009CDF0;
        msg->arg2 = g_AkaoCmd.arg2;
        D_8009CDF0.value = ((sequence.value + 1) & 0x1FF) + 0x400;
        msg->arg3 = g_AkaoCmd.arg3;
        result = sequence.value;
        msg->sequence = result;
        msg->opcode.word = opcode;
        break;
    case 0xD8:
        Akao_AllocMessageSlot(&msg);
        msg->arg0.value = g_AkaoCmd.arg0.value;
        msg->opcode.word = 0xD0;
        Akao_AllocMessageSlot(&msg);
        msg->arg0.value = g_AkaoCmd.arg0.value;
        msg->opcode.word = 0xD4;
        break;
    case 0xD9:
        Akao_AllocMessageSlot(&msg);
        msg->arg0.value = g_AkaoCmd.arg0.value;
        msg->arg1 = g_AkaoCmd.arg1;
        msg->opcode.word = 0xD1;
        Akao_AllocMessageSlot(&msg);
        msg->arg0.value = g_AkaoCmd.arg0.value;
        msg->arg1 = g_AkaoCmd.arg1;
        msg->opcode.word = 0xD5;
        break;
    case 0xDA:
        Akao_AllocMessageSlot(&msg);
        msg->arg0.value = g_AkaoCmd.arg0.value;
        msg->arg1 = g_AkaoCmd.arg1;
        msg->arg2 = g_AkaoCmd.arg2;
        msg->opcode.word = 0xD2;
        Akao_AllocMessageSlot(&msg);
        msg->arg0.value = g_AkaoCmd.arg0.value;
        msg->arg1 = g_AkaoCmd.arg1;
        msg->arg2 = g_AkaoCmd.arg2;
        msg->opcode.word = 0xD6;
        break;
    case 0x99:
        Akao_AllocMessageSlot(&msg);
        msg->opcode.word = 0x9B;
        Akao_AllocMessageSlot(&msg);
        msg->opcode.word = 0x9D;
        break;
    case 0x98:
        Akao_AllocMessageSlot(&msg);
        msg->opcode.word = 0x9A;
        Akao_AllocMessageSlot(&msg);
        msg->opcode.word = 0x9C;
        break;
    default:
        Akao_AllocMessageSlot(&msg);
        msg->arg0.value = g_AkaoCmd.arg0.value;
        msg->arg1 = g_AkaoCmd.arg1;
        msg->arg2 = g_AkaoCmd.arg2;
        msg->arg3 = g_AkaoCmd.arg3;
        msg->opcode.word = g_AkaoCmd.opcode;
        break;
    }
    D_8009D268 = 0;
    return result;
}
