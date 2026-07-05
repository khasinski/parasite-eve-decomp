/* MASPSX_FLAGS: --expand-div */

typedef unsigned char u8;
typedef signed char s8;

#include "pe1/akao.h"

typedef unsigned char u8_2;
typedef int s32;

typedef unsigned char u8_4;
typedef int s32_4;

typedef unsigned char u8_6;
typedef unsigned short u16;
typedef unsigned int u32;

extern char *g_AkaoCurTrack;
extern int g_AkaoInstrumentTable;
extern int g_AkaoPitchPeriodTable;
extern unsigned int g_SpuActiveVoiceMask;

void SeqOp_SetVoiceInstrument(void *track_arg, void *src_arg, int arg2);

void Akao_SetExpressionSlide(AkaoTrack *track) {
    register u8 *pc asm("$2");
    u8 *pc2;
    int duration;
    int current;
    int next;

    pc = track->pc;
    track->pc = pc + 1;
    duration = pc[0];
    track->expression_duration = duration;
    if (duration == 0) {
        duration = 0x100;
        track->expression_duration = duration;
    }

    pc2 = track->pc;
    track->pc = pc2 + 1;
    next = ((s8)pc2[0]) << 23;
    current = track->expression_value & 0xFFFF0000;
    track->expression_value = current;
    track->expression_delta = (next - current) / track->expression_duration;
}

void sndTrackSkip2(int *arg0)
{
    *arg0 += 2;
}

void sndTrackNoop(void)
{
}

void Akao_SetPanTarget(AkaoTrack *track)
{
    unsigned char *cursor = track->pc;
    unsigned int flags;
    unsigned char value;

    track->pc = cursor + 1;
    value = *cursor;
    flags = track->flags;
    track->panpot_duration = 0;
    track->pan_target = value << 8;

    if (flags & 0x100) {
        track->update_flags |= 3;
    }
}

void Akao_SetPanTargetSlide(AkaoTrack *track) {
    int duration;
    int field_d8;
    int next;
    int delta;

    duration = (track->panpot_duration = *track->pc++);
    if (duration == 0) {
        duration = 0x100;
        track->panpot_duration = duration;
    }

    field_d8 = (unsigned short)track->pan_target & 0xFF00;
    next = *track->pc++;
    delta = ((next << 8) - (short)field_d8) / track->panpot_duration;
    track->pan_target = field_d8;
    track->pan_delta = delta;
}

void Akao_SetPanpot(AkaoTrack *track)
{
    unsigned char *cursor = track->pc;
    int value;

    track->pc = cursor + 1;
    value = *cursor;
    track->panpot_slide_duration = 0;
    value += 0x40;
    track->update_flags |= 3;
    value &= 0xFF;
    track->panpot = value << 8;
}

void Akao_SetPanSlide(AkaoTrack *track) {
    int duration;
    int field_76;
    int next;
    int delta;

    duration = (track->panpot_slide_duration = *track->pc++);
    if (duration == 0) {
        duration = 0x100;
        track->panpot_slide_duration = duration;
    }

    field_76 = track->panpot & 0xFF00;
    next = *track->pc++;
    delta = ((((next + 0x40) & 0xFF) << 8) - field_76) / track->panpot_slide_duration;
    track->panpot = field_76;
    track->panpot_delta = delta;
}

void sndTrackReadPanpot(AkaoTrack *track) {
    unsigned char *cursor = track->pc;

    track->pc = cursor + 1;
    track->panpot_step = *cursor;
}

void sndTrackIncPanpot(AkaoTrack *track) {
    track->panpot_step = (track->panpot_step + 1) & 0xF;
}

void sndTrackDecPanpot(AkaoTrack *track) {
    track->panpot_step = (track->panpot_step - 1) & 0xF;
}

void Akao_PlayNote(void *ptr, unsigned int mask) {
    u8_6 *pc;
    unsigned int note;
    unsigned int old_note;
    int table;

    pc = *(u8_6 **)ptr;
    *(u8_6 **)ptr = pc + 1;
    note = *pc;

    if (*(u16 *)((char *)ptr + 0x54) == 0) {
        if ((*(u32 *)g_AkaoCurTrack & 0x100) != 0) {
            if (note >= 0x20) {
                note += 0x30;
            }
        }
    }

    table = (int)&g_AkaoInstrumentTable + (note << 6);
    old_note = *(u16 *)((char *)ptr + 0x5A);
    *(u16 *)((char *)ptr + 0x5A) = note;
    if (old_note != 0xFF) {
        if (*(u16 *)((char *)ptr + 0x54) == 0) {
            if ((*(u32 *)(g_AkaoCurTrack + 0x14) & mask & g_SpuActiveVoiceMask) != 0) {
                goto call;
            }
        }

        *(u32 *)((char *)ptr + 0xF4) |= 0x10;
        *(int *)((char *)ptr + 0x30) =
            (unsigned int)(*(int *)((char *)ptr + 0x30) * *(int *)(table + 0x10)) /
            *(unsigned int *)((int)&g_AkaoPitchPeriodTable + (old_note << 6));
    }

call:
    SeqOp_SetVoiceInstrument(ptr, table, *(int *)table);
    *(u32 *)((char *)ptr + 0x38) &= ~0x1000;
}

void Akao_TieNote(void *ptr, unsigned int mask) {
    u8_6 *pc;
    int note;
    unsigned int old_note;
    int table;

    pc = *(u8_6 **)ptr;
    *(u8_6 **)ptr = pc + 1;
    note = *pc;

    if (*(u16 *)((char *)ptr + 0x54) == 0) {
        if ((*(u32 *)g_AkaoCurTrack & 0x100) != 0) {
            if (note >= 0x20) {
                note += 0x30;
            }
        }
    }

    table = (int)&g_AkaoInstrumentTable + (note << 6);
    old_note = *(u16 *)((char *)ptr + 0x5A);
    *(u16 *)((char *)ptr + 0x5A) = note;
    if (old_note != 0xFF) {
        if (*(u16 *)((char *)ptr + 0x54) == 0) {
            if ((*(u32 *)(g_AkaoCurTrack + 0x14) & mask & g_SpuActiveVoiceMask) != 0) {
                goto call;
            }
        }

        *(u32 *)((char *)ptr + 0xF4) |= 0x10;
        *(int *)((char *)ptr + 0x30) =
            (unsigned int)(*(int *)((char *)ptr + 0x30) * *(int *)(table + 0x10)) /
            *(unsigned int *)((int)&g_AkaoPitchPeriodTable + (old_note << 6));
    }

call:
    SeqOp_SetVoiceInstrument(ptr, table, 0x1010);
    *(u32 *)((char *)ptr + 0x38) &= ~0x1000;
}
