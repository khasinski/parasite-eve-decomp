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

void SeqOp_SetTrack38Mask(void *ptr, u32 mask) {
    if (*(unsigned short *)((char *)ptr + 0x54) == 0) {
        *(u32 *)(g_AkaoCurTrack + 0x38) |= mask;
    } else {
        g_AkaoTrack38Mask |= mask;
    }

    Seq_MarkTrack38MaskDirty();
}

void SeqOp_ClearMask(void *ptr, u32 mask) {
    char *track;

    if (*(unsigned short *)((char *)ptr + 0x54) == 0) {
        track = g_AkaoCurTrack;
        *(u32 *)(track + 0x38) &= ~mask;
        if ((*(u32 *)((char *)ptr + 0x38) & 0x800) != 0) {
            *(u32 *)(track + 0x30) &= ~(1 << *(unsigned short *)((char *)ptr + 0x5C));
        }
    } else {
        g_AkaoTrack38Mask &= ~mask;
    }

    Seq_MarkTrack38MaskDirty();
}

void SeqOp_EnableField84(void *arg0) {
    *(short *)((char *)arg0 + 0x84) = 1;
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
  g_AkaoVoiceUpdateFlags |= 0x10;
}

void sndTrackReadAdsrAttackRate(AkaoTrack *track) {
    unsigned char *cursor = track->pc;
    unsigned int flags;
    unsigned char value;

    track->pc = cursor + 1;
    flags = track->update_flags;
    value = *cursor;
    track->update_flags = flags | 0x900;
    track->adsr_attack_rate = value;
}

void sndTrackReadAdsrDecayRate(AkaoTrack *track) {
    unsigned char *cursor = track->pc;
    unsigned int flags;
    unsigned char value;

    track->pc = cursor + 1;
    flags = track->update_flags;
    value = *cursor;
    track->update_flags = flags | 0x1000;
    track->adsr_decay_rate = value;
}

void sndTrackReadAdsrSustainLevel(AkaoTrack *track) {
    unsigned char *cursor = track->pc;
    unsigned int flags;
    unsigned char value;

    track->pc = cursor + 1;
    flags = track->update_flags;
    value = *cursor;
    track->update_flags = flags | 0x8000;
    track->adsr_sustain_level = value;
}

void sndTrackReadAdsrSustainRate(AkaoTrack *track) {
    unsigned char *cursor = track->pc;
    unsigned int flags;
    unsigned char value;

    track->pc = cursor + 1;
    flags = track->update_flags;
    value = *cursor;
    track->update_flags = flags | 0x2200;
    track->adsr_sustain_rate = value;
}

void sndTrackReadAdsrReleaseRate(AkaoTrack *track) {
    unsigned char *cursor = track->pc;
    unsigned int flags;
    unsigned char value;

    track->pc = cursor + 1;
    flags = track->update_flags;
    value = *cursor;
    track->update_flags = flags | 0x4400;
    track->adsr_release_rate = value;
}

void sndTrackReadAdsrAttack(AkaoTrack *track) {
    unsigned char *cursor = track->pc;
    unsigned int flags;
    unsigned char value;

    track->pc = cursor + 1;
    flags = track->update_flags;
    value = *cursor;
    track->update_flags = flags | 0x100;
    track->adsr_attack = value;
}

void sndTrackReadAdsrSustain(AkaoTrack *track) {
    unsigned char *cursor = track->pc;
    unsigned int flags;
    unsigned char value;

    track->pc = cursor + 1;
    flags = track->update_flags;
    value = *cursor;
    track->update_flags = flags | 0x200;
    track->adsr_sustain = value;
}

void sndTrackReadAdsrRelease(AkaoTrack *track) {
    unsigned char *cursor = track->pc;
    unsigned int flags;
    unsigned char value;

    track->pc = cursor + 1;
    flags = track->update_flags;
    value = *cursor;
    track->update_flags = flags | 0x400;
    track->adsr_release = value;
}

void Akao_SetReleaseRate(AkaoTrack *track) {
    unsigned char *cursor;
    unsigned int flags;
    unsigned char value;

    track->flags |= 0x200;
    cursor = track->pc;
    track->pc = cursor + 1;
    flags = track->update_flags;
    value = *cursor;
    track->update_flags = flags | 0x4400;
    track->adsr_release_rate = value;
}

void Akao_ResetReleaseRate(AkaoTrack *track) {
    track->flags &= ~0x200;
    track->adsr_release_rate = D_800B290C[track->note_pitch << 6];
    track->update_flags |= 0x4400;
}

void SeqOp_PushLoopPoint(void *ptr) {
    unsigned short index = (*(unsigned short *)((char *)ptr + 0xCE) + 1) & 3;

    *(volatile short *)((char *)ptr + 0xCE) = index;
    *(void **)((char *)ptr + 4 + index * 4) = *(void **)ptr;
    index = *(unsigned short *)((char *)ptr + 0xCE);
    *(short *)((char *)ptr + 0x62 + index * 2) = 0;
}

void SeqOp_LoopCounter(void *ptr) {
    unsigned char *cursor = *(unsigned char **)ptr;
    int value;
    unsigned short index;
    unsigned short counter;

    *(unsigned char **)ptr = cursor + 1;
    value = *cursor;
    if (value == 0) {
        value = 0x100;
    }

    index = *(unsigned short *)((char *)ptr + 0xCE);
    counter = *(unsigned short *)((char *)ptr + 0x62 + index * 2) + 1;
    *(short *)((char *)ptr + 0x62 + index * 2) = counter;

    if (counter != value) {
        index = *(unsigned short *)((char *)ptr + 0xCE);
        *(void **)ptr = *(void **)((char *)ptr + 4 + index * 4);
    } else {
        index = *(unsigned short *)((char *)ptr + 0xCE);
        index = (index - 1) & 3;
        *(short *)((char *)ptr + 0xCE) = index;
    }
}

void SeqOp_JumpIfLoopCount(void *ptr) {
    void *track;
    u8_3 *cursor;
    register int value asm("$3");
    u8_3 *target;
    int offset;

    track = ptr;
    asm volatile("" : "=r"(track) : "0"(track));
    cursor = *(u8_3 **)track;
    *(u8_3 **)track = cursor + 1;
    value = cursor[0];
    if (value == 0) {
        value = 0x100;
    }

    if (*(u16_3 *)((char *)track + 0x62 + (*(u16_3 *)((char *)track + 0xCE) * 2)) + 1 != value) {
        u8_3 *skip;

        skip = cursor + 3;
        *(u8_3 **)track = skip;
        return;
    }

    *(u8_3 * volatile *)track = cursor + 2;
    value = cursor[1];
    target = cursor + 3;
    *(u8_3 **)track = target;
    offset = (s16)(value | (cursor[2] << 8));
    *(u8_3 **)track = target + offset;
}

void SeqOp_JumpIfLoopCountPop(void *ptr) {
    void *track;
    u8_3 *cursor;
    register int value asm("$3");
    u8_3 *target;
    int offset;

    track = ptr;
    asm volatile("" : "=r"(track) : "0"(track));
    cursor = *(u8_3 **)track;
    *(u8_3 **)track = cursor + 1;
    value = cursor[0];
    if (value == 0) {
        value = 0x100;
    }

    if (*(u16_3 *)((char *)track + 0x62 + (*(u16_3 *)((char *)track + 0xCE) * 2)) + 1 != value) {
        u8_3 *skip;

        skip = cursor + 3;
        *(u8_3 **)track = skip;
        return;
    }

    *(u8_3 * volatile *)track = cursor + 2;
    value = cursor[1];
    target = cursor + 3;
    *(u8_3 **)track = target;
    offset = (s16)(value | (cursor[2] << 8));
    *(u8_3 **)track = target + offset;
    *(u16_3 *)((char *)track + 0xCE) = (*(u16_3 *)((char *)track + 0xCE) - 1) & 3;
}

void sndTrackReturn(AkaoTrack *track) {
    unsigned short index = track->call_stack_index;

    track->repeat_counters[index]++;
    track->pc = track->call_stack[track->call_stack_index];
}

void SeqOp_SetVolume(void *ptr) {
    unsigned char *cursor = *(unsigned char **)ptr;
    int value;

    *(unsigned char **)ptr = cursor + 1;
    value = *cursor;
    *(short *)((char *)ptr + 0xD2) = 0;
    *(short *)((char *)ptr + 0x58) = value;
    *(short *)((char *)ptr + 0x56) = value;
    *(short *)((char *)ptr + 0xD0) = value;
}

void SeqOp_AdjustVolumeTarget(void *ptr) {
    char *cursor = *(char **)ptr;
    int value;

    *(char **)ptr = cursor + 1;
    value = *(signed char *)cursor;
    if (value != 0) {
        value += *(short *)((char *)ptr + 0xD0);
        if (value <= 0) {
            value = 1;
        } else if (value >= 0x100) {
            value = 0xFF;
        }
    }

    *(short *)((char *)ptr + 0xD2) = value;
}
