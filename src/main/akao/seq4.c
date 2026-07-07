typedef unsigned int u32;

extern char *g_AkaoCurTrack;
extern u32 g_AkaoTrack38Mask;

void Seq_MarkTrack38MaskDirty(void);

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32_1;
extern char *g_AkaoCurTrack_1 __asm__("g_AkaoCurTrack");
extern u32_1 g_AkaoVoiceUpdateFlags;
extern u16 g_AkaoTrack5ATransposeValue;

#include "pe1/akao.h"

typedef unsigned char u8_2;

extern u8_2 D_800B290C[];

typedef signed short s16;
typedef unsigned char u8_3;
typedef unsigned short u16_3;

void SeqOp_SetTrack38Mask(AkaoTrack *track, u32 mask) {
    if (track->parent_track_id == 0) {
        *(u32 *)(g_AkaoCurTrack + 0x38) |= mask;
    } else {
        g_AkaoTrack38Mask |= mask;
    }

    Seq_MarkTrack38MaskDirty();
}

void SeqOp_ClearMask(AkaoTrack *track, u32 mask) {
    char *player;

    if (track->parent_track_id == 0) {
        player = g_AkaoCurTrack;
        *(u32 *)(player + 0x38) &= ~mask;
        if ((track->flags & AKAO_TRACK_FLAG_VOICE_ALLOCATED) != 0) {
            *(u32 *)(player + 0x30) &= ~(1 << track->voice_index);
        }
    } else {
        g_AkaoTrack38Mask &= ~mask;
    }

    Seq_MarkTrack38MaskDirty();
}

void SeqOp_EnableField84(AkaoTrack *track) {
    track->tremolo_phase = 1;
}

void SeqOp_Noop_904AC(void) {
}

void SeqOp_Noop_904B4(void) {
}

void SeqOp_Noop_904BC(void) {
}

unsigned int SeqOp_SetTrack5AValue(void *ptr)
{
  u8 *pc;
  int value;
  register char *track;
  pc = *((u8 **) ptr);
  *((u8 **) ptr) = pc + 1;
  value = pc[0];
  if ((*((u16 *) (((char *) ptr) + 0x54))) == 0)
  {
    if ((value & 0xC0) != 0)
    {
      ;
      *((u16 *) (g_AkaoCurTrack_1 + 0x5A)) = ((*((u16 *) (g_AkaoCurTrack_1 + 0x5A))) + (value & 0x3F)) & 0x3F;
    }
    else
    {
      track = g_AkaoCurTrack_1;
      *((u16 *) (track + 0x5A)) = value;
    }
  }
  else
    if ((value & 0xC0) != 0)
  {
    int m = value & 0x3F;
    g_AkaoTrack5ATransposeValue = (g_AkaoTrack5ATransposeValue + m) & 0x3F;
  }
  else
  {
    g_AkaoTrack5ATransposeValue = value;
  }
  g_AkaoVoiceUpdateFlags |= AKAO_VOICE_PARAM_PITCH;
}

void sndTrackReadAdsrAttackRate(AkaoTrack *track) {
    unsigned char *cursor = track->pc;
    unsigned int flags;
    unsigned char value;

    track->pc = cursor + 1;
    flags = track->update_flags;
    value = *cursor;
    track->update_flags = flags | AKAO_VOICE_PARAM_ADSR_ATTACK;
    track->adsr_attack_rate = value;
}

void sndTrackReadAdsrDecayRate(AkaoTrack *track) {
    unsigned char *cursor = track->pc;
    unsigned int flags;
    unsigned char value;

    track->pc = cursor + 1;
    flags = track->update_flags;
    value = *cursor;
    track->update_flags = flags | AKAO_VOICE_PARAM_ADSR_DECAY_RATE;
    track->adsr_decay_rate = value;
}

void sndTrackReadAdsrSustainLevel(AkaoTrack *track) {
    unsigned char *cursor = track->pc;
    unsigned int flags;
    unsigned char value;

    track->pc = cursor + 1;
    flags = track->update_flags;
    value = *cursor;
    track->update_flags = flags | AKAO_VOICE_PARAM_ADSR_SUSTAIN_LEVEL;
    track->adsr_sustain_level = value;
}

void sndTrackReadAdsrSustainRate(AkaoTrack *track) {
    unsigned char *cursor = track->pc;
    unsigned int flags;
    unsigned char value;

    track->pc = cursor + 1;
    flags = track->update_flags;
    value = *cursor;
    track->update_flags = flags | AKAO_VOICE_PARAM_ADSR_SUSTAIN;
    track->adsr_sustain_rate = value;
}

void sndTrackReadAdsrReleaseRate(AkaoTrack *track) {
    unsigned char *cursor = track->pc;
    unsigned int flags;
    unsigned char value;

    track->pc = cursor + 1;
    flags = track->update_flags;
    value = *cursor;
    track->update_flags = flags | AKAO_VOICE_PARAM_ADSR_RELEASE;
    track->adsr_release_rate = value;
}

void sndTrackReadAdsrAttack(AkaoTrack *track) {
    unsigned char *cursor = track->pc;
    unsigned int flags;
    unsigned char value;

    track->pc = cursor + 1;
    flags = track->update_flags;
    value = *cursor;
    track->update_flags = flags | AKAO_VOICE_PARAM_ADSR_ATTACK_MODE;
    track->adsr_attack = value;
}

void sndTrackReadAdsrSustain(AkaoTrack *track) {
    unsigned char *cursor = track->pc;
    unsigned int flags;
    unsigned char value;

    track->pc = cursor + 1;
    flags = track->update_flags;
    value = *cursor;
    track->update_flags = flags | AKAO_VOICE_PARAM_ADSR_SUSTAIN_MODE;
    track->adsr_sustain = value;
}

void sndTrackReadAdsrRelease(AkaoTrack *track) {
    unsigned char *cursor = track->pc;
    unsigned int flags;
    unsigned char value;

    track->pc = cursor + 1;
    flags = track->update_flags;
    value = *cursor;
    track->update_flags = flags | AKAO_VOICE_PARAM_ADSR_RELEASE_MODE;
    track->adsr_release = value;
}

void Akao_SetReleaseRate(AkaoTrack *track) {
    unsigned char *cursor;
    unsigned int flags;
    unsigned char value;

    track->flags |= AKAO_TRACK_FLAG_RELEASE_RATE_OVERRIDE;
    cursor = track->pc;
    track->pc = cursor + 1;
    flags = track->update_flags;
    value = *cursor;
    track->update_flags = flags | AKAO_VOICE_PARAM_ADSR_RELEASE;
    track->adsr_release_rate = value;
}

void Akao_ResetReleaseRate(AkaoTrack *track) {
    track->flags &= ~AKAO_TRACK_FLAG_RELEASE_RATE_OVERRIDE;
    track->adsr_release_rate = D_800B290C[track->note_pitch << 6];
    track->update_flags |= AKAO_VOICE_PARAM_ADSR_RELEASE;
}

void SeqOp_PushLoopPoint(AkaoTrack *track) {
    unsigned short index = (track->call_stack_index + 1) & 3;

    *(volatile short *)&track->call_stack_index = index;
    track->call_stack[index] = track->pc;
    index = track->call_stack_index;
    track->repeat_counters[index] = 0;
}

void SeqOp_LoopCounter(AkaoTrack *track) {
    unsigned char *cursor = track->pc;
    int value;
    unsigned short index;
    unsigned short counter;

    track->pc = cursor + 1;
    value = *cursor;
    if (value == 0) {
        value = 0x100;
    }

    index = track->call_stack_index;
    counter = track->repeat_counters[index] + 1;
    track->repeat_counters[index] = counter;

    if (counter != value) {
        index = track->call_stack_index;
        track->pc = track->call_stack[index];
    } else {
        index = track->call_stack_index;
        index = (index - 1) & 3;
        track->call_stack_index = index;
    }
}

void SeqOp_JumpIfLoopCount(AkaoTrack *ptr) {
    AkaoTrack *track;
    u8_3 *cursor;
    register int value asm("$3");
    u8_3 *target;
    int offset;

    track = ptr;
    asm volatile("" : "=r"(track) : "0"(track));
    cursor = track->pc;
    track->pc = cursor + 1;
    value = cursor[0];
    if (value == 0) {
        value = 0x100;
    }

    if (track->repeat_counters[track->call_stack_index] + 1 != value) {
        u8_3 *skip;

        skip = cursor + 3;
        track->pc = skip;
        return;
    }

    *(u8_3 * volatile *)&track->pc = cursor + 2;
    value = cursor[1];
    target = cursor + 3;
    track->pc = target;
    offset = (s16)(value | (cursor[2] << 8));
    track->pc = target + offset;
}

void SeqOp_JumpIfLoopCountPop(AkaoTrack *ptr) {
    AkaoTrack *track;
    u8_3 *cursor;
    register int value asm("$3");
    u8_3 *target;
    int offset;

    track = ptr;
    asm volatile("" : "=r"(track) : "0"(track));
    cursor = track->pc;
    track->pc = cursor + 1;
    value = cursor[0];
    if (value == 0) {
        value = 0x100;
    }

    if (track->repeat_counters[track->call_stack_index] + 1 != value) {
        u8_3 *skip;

        skip = cursor + 3;
        track->pc = skip;
        return;
    }

    *(u8_3 * volatile *)&track->pc = cursor + 2;
    value = cursor[1];
    target = cursor + 3;
    track->pc = target;
    offset = (s16)(value | (cursor[2] << 8));
    track->pc = target + offset;
    track->call_stack_index = (track->call_stack_index - 1) & 3;
}

void sndTrackReturn(AkaoTrack *track) {
    unsigned short index = track->call_stack_index;

    track->repeat_counters[index]++;
    track->pc = track->call_stack[track->call_stack_index];
}

void SeqOp_SetVolume(AkaoTrack *track) {
    unsigned char *cursor = track->pc;
    int value;

    track->pc = cursor + 1;
    value = *cursor;
    track->field_D2 = 0;
    track->pan_duration = value;
    track->field_56 = value;
    track->field_D0 = value;
}

void SeqOp_AdjustVolumeTarget(AkaoTrack *track) {
    char *cursor = (char *)track->pc;
    int value;

    track->pc = (unsigned char *)cursor + 1;
    value = *(signed char *)cursor;
    if (value != 0) {
        value += track->field_D0;
        if (value <= 0) {
            value = 1;
        } else if (value >= 0x100) {
            value = 0xFF;
        }
    }

    track->field_D2 = value;
}
