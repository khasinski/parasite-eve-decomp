#include "common.h"
#include "pe1/akao/voice_masks.h"
#include "pe1/psyq_spu_internal.h"


void Akao_StepVoiceNote(AkaoTrack *track, unsigned mask, unsigned direct, unsigned *output) {
    unsigned bit = 1;
    int index = 0;
    unsigned requested = mask & g_AkaoCurTrack->key_on_request_mask;
    do {
        if (mask & bit) {
            Akao_SetVoiceKeyOff(track, bit, index);
            if (track->update_flags) {
                if (requested & bit) {
                    if (direct & bit) {
                        *output |= 1 << index;
                        track->assigned_voice_index = index;
                        track->update_flags |= 0x1ff93;
                    } else {
                        unsigned voice = 0;
                        AkaoVoiceEnvelopeSlot *slot = g_AkaoVoiceEnvelopeTable;
                        do {
                            if (!slot->level) {
                                track->update_flags |= 0x1ff93;
                                *output |= 1 << voice;
                                track->assigned_voice_index = voice;
                                *(short *)&slot->level = 0x7fff;
                                g_AkaoVoiceUpdateFlags |= 0x100;
                                voice = 24;
                            } else {
                                voice++;
                                slot++;
                                if (voice == 24) {
                                    track->assigned_voice_index = voice;
                                    g_AkaoCurTrack->status_flags |= 1;
                                }
                            }
                        } while (voice < 24);
                    }
                }
                if (g_AkaoVoicePortamentoResetMask & bit) {
                    track->volume_right = 0;
                    track->volume_left = 0;
                }
                if ((unsigned)track->assigned_voice_index < 24)
                    Akao_WriteVoiceParam(track->assigned_voice_index, (AkaoVoiceParams *)&track->assigned_voice_index, track->flags);
            }
            mask &= ~bit;
        }
        bit <<= 1;
        track++;
        index++;
    } while (mask);
}


/* The tracked voice ID is AkaoTrack::assigned_voice_index (+0xF0); retain
 * the original word-stride loop while exposing the shared track type in the API. */
void Akao_RemoveVoice(AkaoTrack *voices, int voiceIndex) {
    int *assignedVoiceIndex = (int *)voices;
    int i = 0;
    int unusedVoice = 0x18;

    assignedVoiceIndex += 0x3C;
    do {
        if (voiceIndex == *assignedVoiceIndex) {
            *assignedVoiceIndex = unusedVoice;
        }
        i++;
        assignedVoiceIndex += 0x47;
    } while ((unsigned int)i < 0x18);
}

void Akao_UpdateVoiceEnvelopes(s32 protectedMask) {
    AkaoVoiceEnvelopeSlot *envelope;
    s32 combinedMask;
    u32 voiceIndex;
    u32 mask;
    AkaoTrack *voiceBase;

    voiceIndex = 0;
    mask = 1;
    voiceBase = &g_AkaoVoiceStateTable[0];
    envelope = &g_AkaoVoiceEnvelopeTable[0];
    combinedMask = (((*((s32 *) (((u8 *) g_AkaoCurTrack) + 4))) & (*((s32 *) (((u8 *) g_AkaoCurTrack) + 0xC)))) | ((*((s32 *) (((u8 *) g_AkaoCurTrack) + 0x6C))) & (*((s32 *) (((u8 *) g_AkaoCurTrack) + 0x74))))) | protectedMask;
    do {
        if (combinedMask & (mask << voiceIndex)) {
            register s32 level = 0x7FFF;
            envelope->level = level;
        } else {
            SpuGetVoiceEnvelope(voiceIndex, (unsigned short *)envelope);
            if (envelope->level == 0) {
                if (envelope) {
                    Akao_RemoveVoice(voiceBase, voiceIndex);
                    Akao_RemoveVoice(voiceBase + AKAO_VOICE_COUNT, voiceIndex);
                } else {
                    Akao_RemoveVoice(voiceBase, voiceIndex);
                    Akao_RemoveVoice(voiceBase + AKAO_VOICE_COUNT, voiceIndex);
                }
            }
        }
        voiceIndex += 1;
        envelope++;
    } while (voiceIndex < 0x18U);
}

extern unsigned D_800BCD58, g_AkaoVoiceMaskScratch;
extern unsigned short g_AkaoTrack5ATransposeValue;
extern AkaoTrack g_AkaoVoiceChannelTable[];
void Akao_WriteVoiceParam(int, int *, unsigned);
void Akao_SetMasterVolume(short, short);
void Spu_WriteReverbEnable(unsigned);
void Spu_WriteNoiseEnable(unsigned);
void Spu_WriteFmEnable(unsigned);
void Spu_WriteKeyOn(unsigned);
void Akao_ProcessVoiceQueue(void)
{
  unsigned *active = &g_SpuActiveVoiceMask;
  unsigned excluded;
  unsigned secondary;
  unsigned secondary_new;
  unsigned int allocated_primary;
  unsigned primary;
  unsigned primary_new;
  unsigned result = 0;

  register unsigned bit asm("$18");
  unsigned flags;
  AkaoSequencerBank *bank;
  AkaoTrack *track;
  excluded = (*active) | g_SpuStoppedVoiceMask;
  if ((g_AkaoCurTrack[0].active_voice_mask & g_AkaoCurTrack[0].key_on_request_mask) | (g_AkaoCurTrack[1].active_voice_mask & g_AkaoCurTrack[1].key_on_request_mask))
  {
    Akao_UpdateVoiceEnvelopes(excluded);
  }
  secondary = (g_AkaoCurTrack[1].active_voice_mask & g_AkaoCurTrack[1].allocated_voice_mask) & (~(g_AkaoCurTrack[1].field_0C & excluded));
  secondary_new = (secondary & g_AkaoCurTrack[1].field_0C) & (~excluded);
  if (secondary & g_AkaoCurTrack[1].pending_voice_mask)
  {
    g_AkaoCurTrack++;
    Akao_StepVoiceNote(g_AkaoVoiceStateTable2, secondary & g_AkaoCurTrack->pending_voice_mask, secondary_new, &result);
    bank = g_AkaoCurTrack;
    secondary &= ~bank->pending_voice_mask;
    g_AkaoCurTrack--;
    bank->key_on_request_mask &= ~bank->pending_voice_mask;
  }
  allocated_primary = g_AkaoCurTrack->active_voice_mask & g_AkaoCurTrack->allocated_voice_mask;
  primary_new = secondary_new | excluded;
  primary = allocated_primary & (~(g_AkaoCurTrack->field_0C & primary_new));
  excluded = (primary & g_AkaoCurTrack->field_0C) & (~primary_new);
  if (primary & g_AkaoCurTrack->pending_voice_mask)
  {
    Akao_StepVoiceNote(g_AkaoVoiceStateTable, primary & g_AkaoCurTrack->pending_voice_mask, excluded, &result);
    bank = g_AkaoCurTrack;
    primary &= ~bank->pending_voice_mask;
    /* Retail reloads the pending mask for this independent flag update. */
    bank->key_on_request_mask &= ~*(volatile unsigned *)&bank->pending_voice_mask;
  }
  asm("" : : "r"(secondary));
  if (secondary)
  {
    g_AkaoCurTrack++;
    Akao_StepVoiceNote(g_AkaoVoiceStateTable2, secondary, secondary_new & (~excluded), &result);
    g_AkaoCurTrack->key_on_request_mask = 0;
    g_AkaoCurTrack--;
  }
  if (primary)
  {
    Akao_StepVoiceNote(g_AkaoVoiceStateTable, primary, excluded, &result);
    g_AkaoCurTrack->key_on_request_mask = 0;
  }
  primary = (*active) & D_800BCD58;
  if (primary)
  {
    bit = 0x1000;
    track = g_AkaoVoiceChannelTable;
    result |= g_AkaoVoiceMaskScratch;
    while (primary)
    {
      if (primary & bit)
      {
        Akao_SetVoiceKeyOn(track, bit);
        if (track->update_flags)
        {
          Akao_WriteVoiceParam(track->assigned_voice_index, &track->assigned_voice_index, track->flags);
        }
        secondary_new = bit;
        primary &= ~secondary_new;
      }
      bit <<= 1;
      track++;
    }

    g_AkaoVoiceMaskScratch = 0;
  }
  primary = g_AkaoVoiceUpdateFlags;
  if (primary & 0x80)
  {
    short volume = ((short *) g_AkaoCurTrack->field_40)[1];
    Akao_SetMasterVolume(volume, volume);
    g_AkaoVoiceUpdateFlags &= ~0x80;
  }
  if (primary & 0x10)
  {
    unsigned short clock;
    if (g_SpuActiveVoiceMask)
    {
      clock = g_AkaoTrack5ATransposeValue;
    }
    else
    {
      clock = g_AkaoCurTrack->field_5A;
    }
    SpuSetNoiseClock(clock);
    g_AkaoVoiceUpdateFlags &= ~0x10;
  }
  if (0x100 & primary)
  {
    Akao_SetVoiceAdsr();
    Akao_SetVoiceVolume();
    Akao_SetVoiceStartAddr();
    Spu_WriteReverbEnable(D_800C0DD0);
    Spu_WriteNoiseEnable(D_800C0DD4);
    Spu_WriteFmEnable(D_800C0DD8);
    g_AkaoVoiceUpdateFlags &= ~0x100;
  }
  if (result)
  {
    Spu_WriteKeyOn(result);
  }
}


void Spu_VoiceMaskCompose(AkaoTrack *track, int *mask_out, int mask, int mask_keep) {
    int bit;
    int idx;

    bit = 1;
    do {
        if ((mask & bit) != 0) {
            idx = track->assigned_voice_index;
            if ((unsigned int) idx < 0x18) {
                *mask_out |= 1 << idx;
            }
        }
        mask &= ~bit;
        track++;
        bit <<= 1;
    } while (mask != 0);

    *mask_out &= mask_keep;
}

void Akao_SetVoicePitch(void) {
    unsigned secondary, primary, keep;
    AkaoSequencerBank *bank;
    int result = 0;
    keep = ~(g_SpuActiveVoiceMask | g_SpuStoppedVoiceMask);
    secondary = g_AkaoCurTrack[1].active_voice_mask & g_AkaoCurTrack[1].key_off_request_mask;
    if (secondary & g_AkaoCurTrack[1].pending_voice_mask) {
        g_AkaoCurTrack++;
        Spu_VoiceMaskCompose(g_AkaoVoiceStateTable2, &result, secondary & g_AkaoCurTrack->pending_voice_mask, keep);
        bank = g_AkaoCurTrack;
        secondary &= ~bank->pending_voice_mask;
        g_AkaoCurTrack--;
        bank->key_off_request_mask &= ~bank->pending_voice_mask;
    }
    primary = g_AkaoCurTrack->active_voice_mask & g_AkaoCurTrack->key_off_request_mask;
    if (primary & g_AkaoCurTrack->pending_voice_mask) {
        Spu_VoiceMaskCompose(g_AkaoVoiceStateTable, &result, primary & g_AkaoCurTrack->pending_voice_mask, keep);
        bank = g_AkaoCurTrack;
        primary &= ~bank->pending_voice_mask;
        /* Preserve the retail reload before clearing the bank requests. */
        __asm__("" ::: "memory");
        bank->key_off_request_mask &= ~bank->pending_voice_mask;
    }
    if (secondary) {
        g_AkaoCurTrack++;
        Spu_VoiceMaskCompose(g_AkaoVoiceStateTable2, &result, secondary, keep);
        g_AkaoCurTrack->key_off_request_mask = 0;
        g_AkaoCurTrack--;
    }
    if (primary) {
        Spu_VoiceMaskCompose(g_AkaoVoiceStateTable, &result, primary, keep);
        g_AkaoCurTrack->key_off_request_mask = 0;
    }
    result |= g_SpuPendingKeyOffMask;
    g_SpuPendingKeyOffMask = 0;
    if (result) Spu_WriteKeyOff(result);
}

void Seq_MarkTrack34MaskDirty(void) {
    g_AkaoVoiceUpdateFlags |= 0x100;
}

void Akao_SetVoiceVolume(void) {
    unsigned secondary, primary, keep;
    int result = 0;
    keep = ~(g_SpuActiveVoiceMask | g_SpuStoppedVoiceMask);
    secondary = g_AkaoCurTrack[1].active_voice_mask & g_AkaoCurTrack[1].key_off_dirty_mask;
    if (secondary & g_AkaoCurTrack[1].pending_voice_mask) {
        g_AkaoCurTrack++;
        Spu_VoiceMaskCompose(g_AkaoVoiceStateTable2, &result, secondary & g_AkaoCurTrack->pending_voice_mask, keep);
        secondary &= ~g_AkaoCurTrack->pending_voice_mask;
        g_AkaoCurTrack--;
    }
    primary = g_AkaoCurTrack->active_voice_mask & g_AkaoCurTrack->key_off_dirty_mask;
    if (primary & g_AkaoCurTrack->pending_voice_mask) {
        Spu_VoiceMaskCompose(g_AkaoVoiceStateTable, &result, primary & g_AkaoCurTrack->pending_voice_mask, keep);
        primary &= ~g_AkaoCurTrack->pending_voice_mask;
    }
    if (secondary) {
        g_AkaoCurTrack++;
        Spu_VoiceMaskCompose(g_AkaoVoiceStateTable2, &result, secondary, keep);
        g_AkaoCurTrack--;
    }
    if (primary) Spu_VoiceMaskCompose(g_AkaoVoiceStateTable, &result, primary, keep);
    result |= g_AkaoTrack34Mask;
    D_800C0DD4 = result;
    g_AkaoVoiceUpdateFlags |= 0x100;
}

void Seq_MarkTrack38MaskDirty(void) {
    g_AkaoVoiceUpdateFlags |= 0x100;
}

void Akao_SetVoiceAdsr(void) {
    unsigned secondary, primary, keep;
    int result = 0;
    keep = ~(g_SpuActiveVoiceMask | g_SpuStoppedVoiceMask);
    secondary = g_AkaoCurTrack[1].active_voice_mask & g_AkaoCurTrack[1].volume_dirty_mask;
    if (secondary & g_AkaoCurTrack[1].pending_voice_mask) {
        g_AkaoCurTrack++;
        Spu_VoiceMaskCompose(g_AkaoVoiceStateTable2, &result, secondary & g_AkaoCurTrack->pending_voice_mask, keep);
        secondary &= ~g_AkaoCurTrack->pending_voice_mask;
        g_AkaoCurTrack--;
    }
    primary = g_AkaoCurTrack->active_voice_mask & g_AkaoCurTrack->volume_dirty_mask;
    if (primary & g_AkaoCurTrack->pending_voice_mask) {
        Spu_VoiceMaskCompose(g_AkaoVoiceStateTable, &result, primary & g_AkaoCurTrack->pending_voice_mask, keep);
        primary &= ~g_AkaoCurTrack->pending_voice_mask;
    }
    if (secondary) {
        g_AkaoCurTrack++;
        Spu_VoiceMaskCompose(g_AkaoVoiceStateTable2, &result, secondary, keep);
        g_AkaoCurTrack--;
    }
    if (primary) Spu_VoiceMaskCompose(g_AkaoVoiceStateTable, &result, primary, keep);
    result |= g_AkaoTrack38Mask;
    D_800C0DD0 = result;
    g_AkaoVoiceUpdateFlags |= 0x100;
}

void Seq_MarkTrack3CMaskDirty(void) {
    g_AkaoVoiceUpdateFlags |= 0x100;
}

void Akao_SetVoiceStartAddr(void) {
    unsigned secondary, primary, keep;
    int result = 0;
    keep = ~(g_SpuActiveVoiceMask | g_SpuStoppedVoiceMask);
    secondary = g_AkaoCurTrack[1].active_voice_mask & g_AkaoCurTrack[1].adsr_dirty_mask;
    if (secondary & g_AkaoCurTrack[1].pending_voice_mask) {
        g_AkaoCurTrack++;
        Spu_VoiceMaskCompose(g_AkaoVoiceStateTable2, &result, secondary & g_AkaoCurTrack->pending_voice_mask, keep);
        secondary &= ~g_AkaoCurTrack->pending_voice_mask;
        g_AkaoCurTrack--;
    }
    primary = g_AkaoCurTrack->active_voice_mask & g_AkaoCurTrack->adsr_dirty_mask;
    if (primary & g_AkaoCurTrack->pending_voice_mask) {
        Spu_VoiceMaskCompose(g_AkaoVoiceStateTable, &result, primary & g_AkaoCurTrack->pending_voice_mask, keep);
        primary &= ~g_AkaoCurTrack->pending_voice_mask;
    }
    if (secondary) {
        g_AkaoCurTrack++;
        Spu_VoiceMaskCompose(g_AkaoVoiceStateTable2, &result, secondary, keep);
        g_AkaoCurTrack--;
    }
    if (primary) Spu_VoiceMaskCompose(g_AkaoVoiceStateTable, &result, primary, keep);
    result |= g_AkaoTrack3CMask;
    D_800C0DD8 = result;
    g_AkaoVoiceUpdateFlags |= 0x100;
}
