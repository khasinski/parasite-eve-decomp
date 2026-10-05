/* MASPSX_FLAGS: --expand-div */
#include "common.h"
#include "pe1/akao.h"
extern char *g_AkaoCurTrack;
extern unsigned int g_SpuActiveVoiceMask;


extern AkaoInstrument g_AkaoInstrumentTable[];
void SeqOp_SetVoiceInstrument(AkaoTrack *track_arg, AkaoInstrument *instrument_arg, int sample_header);


extern int g_AkaoVoiceMaskScratch;
extern int g_AkaoTrack34Mask;
extern int g_AkaoTrack38Mask;
extern int g_AkaoTrack3CMask;



typedef unsigned char u8_1;

typedef unsigned int u32_1;

void SeqOp_SetVoiceInstrument(AkaoTrack *track_arg, AkaoInstrument *instrument_arg, int sample_header) {
    AkaoTrack *track = track_arg;
    AkaoInstrument *instrument = instrument_arg;
    int flags;
    int value;
    u32 flags2;
    int tail_value;

    track->sample_header = sample_header;
    track->sample_data = instrument->loop_address;
    track->adsr_attack_rate = instrument->adsr_attack_rate;
    track->adsr_decay_rate = instrument->adsr_decay_rate;
    track->adsr_sustain_level = instrument->adsr_sustain_level;
    track->adsr_sustain_rate = instrument->adsr_sustain_rate;
    track->adsr_attack = instrument->adsr_attack_mode;

    flags = track->flags;
    value = instrument->adsr_sustain_mode;
    track->adsr_sustain = value;

    if ((flags & AKAO_TRACK_FLAG_RELEASE_RATE_OVERRIDE) != 0) {
        track->update_flags |= AKAO_VOICE_PARAM_INSTRUMENT_NO_RELEASE;
    } else {
        unsigned int v = instrument->adsr_release_rate;
        AkaoU16 *dst = &track->adsr_release_rate;

        *dst = v;
        flags2 = track->update_flags;
        tail_value = instrument->adsr_release_mode;
        track->update_flags = flags2 | AKAO_VOICE_PARAM_INSTRUMENT_FULL;
        track->adsr_release = tail_value;
    }
}

void Akao_SetNotePitch(AkaoTrack *track, int arg1) {
    AkaoInstrument *instrument;

    track->note_pitch = arg1;
    instrument = &g_AkaoInstrumentTable[arg1];
    SeqOp_SetVoiceInstrument(track, instrument, instrument->start_address);
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

void SeqOp_SetPitchSlideTarget(void *ptr) {
    u8_1 *pc;

    {
        int value;
        u8_1 *pc0;
        char *track;

        pc0 = *(u8_1 **)ptr;
        *(u8_1 **)ptr = pc0 + 1;
        track = g_AkaoCurTrack;
        value = (*(u16 *)(track + 0x52) = *pc0);
        if (value == 0) {
            value = 0x100;
            *(u16 *)(track + 0x52) = value;
        }
    }

    {
        int value;
        int high;
        char *track;
        int current;

        pc = *(u8_1 **)ptr;
        *(u8_1 **)ptr = pc + 1;
        value = pc[0];
        *(u8_1 **)ptr = pc + 2;
        high = pc[1];

        track = g_AkaoCurTrack;
        value <<= 16;
        high <<= 24;
        value |= high;

        current = *(u32_1 *)(track + 0x20);
        current &= 0xFFFF0000;
        *(u32_1 *)(track + 0x20) = current;
        *(u32_1 *)(track + 0x24) = (value - current) / *(u16 *)(track + 0x52);
    }
}


extern unsigned int g_AkaoVoiceUpdateFlags;


void SeqOp_SetTempoSlide(void *ptr)
{
    unsigned char *stream = *(unsigned char **)ptr;
    char *track;
    unsigned int value;

    *(unsigned char **)ptr = stream + 1;
    value = *stream << 16;
    *(unsigned char **)ptr = stream + 2;
    track = g_AkaoCurTrack;
    value |= stream[1] << 24;
    *(short *)(track + 0x58) = 0;
    g_AkaoVoiceUpdateFlags |= AKAO_GLOBAL_UPDATE_TEMPO;
    *(unsigned int *)(track + 0x40) = value;
}


static inline u8 **SetDuration(u8 **cursor) {
    u8 *pc = *cursor;
    char *track;
    int value;

    *cursor = pc + 1;
    track = g_AkaoCurTrack;
    value = pc[0];
    *(u16 *)(track + 0x58) = value;
    if (value == 0) {
        *(u16 *)(track + 0x58) = 0x100;
    }
    return cursor;
}

void SeqOp_SetVolumeSlideTarget(void *ptr) {
    void *stream;
    u8 *pc;
    int value;

    stream = SetDuration((u8 **)ptr);

    {
        int high;
        char *track;
        int current;

        pc = *(u8 **)stream;
        *(u8 **)stream = pc + 1;
        value = pc[0];
        *(u8 **)stream = pc + 2;
        high = pc[1];

        track = g_AkaoCurTrack;
        value <<= 16;
        high <<= 24;
        value |= high;

        current = *(u32 *)(track + 0x40);
        current &= 0xFFFF0000;
        *(u32 *)(track + 0x40) = current;
        *(u32 *)(track + 0x44) = (value - current) / *(u16 *)(track + 0x58);
    }
}


#define READ_S16(addr) ((short)(*(addr)++ | (*(addr)++ << 8)))

typedef struct TrackLike
{
  char pad[0x56];
  unsigned short limit;
} TrackLike;


void sndTrackJumpRelative(unsigned char **stream) {
    *stream += READ_S16(*stream);
}

void SeqOp_BranchIfLoopLimitReached(unsigned char **stream)
{
  register unsigned char **cursor;
  unsigned char *ptr;
  unsigned char *target;
  int value;
  int lo;
  unsigned char **new_var;
  int offset;
  cursor = stream;
  ptr = *cursor;
  *cursor = ptr + 1;
  value = ptr[0];
  new_var = cursor;
  if (((int) ((TrackLike *)g_AkaoCurTrack)->limit) >= value)
  {
    *cursor = ptr + 2;
    lo = ptr[1];
    target = ptr + 3;
    *cursor = target;
    offset = lo | (ptr[2] << 8);
    target += (short) offset;
    *cursor = target;
  }
  else
  {
    *new_var = ptr + 3;
  }
}


void sndTrackReadVolume(AkaoTrack *track) {
    unsigned char *stream = track->pc;
    unsigned int flags;
    int value;

    track->pc = stream + 1;
    flags = track->update_flags;
    value = *stream;
    track->update_flags = flags | AKAO_VOICE_PARAM_VOLUME;
    track->volume_base = value << 8;
}

static inline int ReadVolumeByte(u8 **cursor) {
    u8 *pc = *cursor;
    *cursor = pc + 1;
    return pc[0];
}

void sndTrackSlideVolume(AkaoTrack *track) {
    u8 *pc;
    int duration;
    int current;
    int next;

    duration = ReadVolumeByte(&track->pc);
    track->volume_duration = duration;
    if (duration == 0) {
        duration = 0x100;
        track->volume_duration = duration;
    }

    pc = track->pc;
    current = track->volume_base & 0x7F00;
    track->pc = pc + 1;
    next = pc[0] << 8;
    track->volume_base = current;
    track->volume_delta = (next - current) / track->volume_duration;
}


void SeqOp_SetVolumeOrExpression(AkaoTrack *track) {
    AkaoU32 new_var;
    register unsigned char *cursor;
    int value;

    if ((track->flags & AKAO_TRACK_FLAG_BRANCH_ACTIVE) != 0) {
        cursor = track->pc;
        track->pc = cursor + 1;
        if (1) {
            value = *cursor;
            track->volume = value << 7;
        }
    } else {
        register unsigned char *cursor2;
        register unsigned char *next;
        register unsigned int flags;
        register int value2;

        cursor2 = track->pc;
        next = cursor2 + 1;
        track->pc = next;
        new_var = track->update_flags;
        value2 = *((signed char *)cursor2);
        flags = new_var;
        track->expression_duration = 0;
        flags |= AKAO_VOICE_PARAM_VOLUME;
        value2 <<= 23;
        track->update_flags = flags;
        track->expression_value = value2;
    }
}


typedef unsigned char u8_2;

typedef unsigned char u8_4;
typedef int s32_4;

typedef unsigned char u8_6;


extern AkaoInstrument g_AkaoInstrumentTable[];
extern int g_AkaoPitchPeriodTable;


void SeqOp_SetVoiceInstrument(AkaoTrack *track_arg, AkaoInstrument *instrument_arg, int sample_header);

void Akao_SetExpressionSlide(AkaoTrack *track) {
    u8 *pc;
    u8 *pc2;
    int duration;
    int current;
    int next;

    pc = track->pc;
    track->pc = pc + 1;
    duration = (track->expression_duration = *pc);
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
        track->update_flags |= AKAO_VOICE_PARAM_VOLUME;
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
    track->update_flags |= AKAO_VOICE_PARAM_VOLUME;
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

void Akao_PlayNote(AkaoTrack *track, unsigned int mask) {
    u8_6 *pc;
    unsigned int note;
    unsigned int old_note;
    int table;

    pc = track->pc;
    track->pc = pc + 1;
    note = *pc;

    if (track->parent_track_id == 0) {
        if ((*(u32 *)g_AkaoCurTrack & 0x100) != 0) {
            if (note >= 0x20) {
                note += 0x30;
            }
        }
    }

    table = (int)&g_AkaoInstrumentTable + (note << 6);
    old_note = track->note_pitch;
    track->note_pitch = note;
    if (old_note != 0xFF) {
        if (track->parent_track_id == 0) {
            if ((*(u32 *)(g_AkaoCurTrack + 0x14) & mask & g_SpuActiveVoiceMask) != 0) {
                goto call;
            }
        }

        track->update_flags |= AKAO_VOICE_PARAM_PITCH;
        track->pitch_base =
            (unsigned int)(track->pitch_base * ((AkaoInstrument *)table)->pitch[0]) /
            *(unsigned int *)((int)&g_AkaoPitchPeriodTable + (old_note << 6));
    }

call:
    SeqOp_SetVoiceInstrument(track, (AkaoInstrument *)table, *(int *)table);
    track->flags &= ~AKAO_TRACK_FLAG_PENDING_NOTE_PITCH;
}

void Akao_TieNote(AkaoTrack *track, unsigned int mask) {
    u8_6 *pc;
    int note;
    unsigned int old_note;
    int table;

    pc = track->pc;
    track->pc = pc + 1;
    note = *pc;

    if (track->parent_track_id == 0) {
        if ((*(u32 *)g_AkaoCurTrack & 0x100) != 0) {
            if (note >= 0x20) {
                note += 0x30;
            }
        }
    }

    table = (int)&g_AkaoInstrumentTable + (note << 6);
    old_note = track->note_pitch;
    track->note_pitch = note;
    if (old_note != 0xFF) {
        if (track->parent_track_id == 0) {
            if ((*(u32 *)(g_AkaoCurTrack + 0x14) & mask & g_SpuActiveVoiceMask) != 0) {
                goto call;
            }
        }

        track->update_flags |= AKAO_VOICE_PARAM_PITCH;
        track->pitch_base =
            (unsigned int)(track->pitch_base * ((AkaoInstrument *)table)->pitch[0]) /
            *(unsigned int *)((int)&g_AkaoPitchPeriodTable + (old_note << 6));
    }

call:
    SeqOp_SetVoiceInstrument(track, (AkaoInstrument *)table, 0x1010);
    track->flags &= ~AKAO_TRACK_FLAG_PENDING_NOTE_PITCH;
}


extern AkaoInstrument g_AkaoInstrumentTable[];
extern void *D_8009C080[];

void SeqOp_SetReturnPoint(AkaoTrack *track) {
    unsigned char *cursor;
    unsigned char *next_cursor;
    int value;

    cursor = track->pc;
    value = *track->pc++;
    next_cursor = cursor + 2;
    track->pc = next_cursor;
    value |= cursor[1] << 8;

    track->note_pitch = 0xFF;
    track->current_note = 0;
    track->flags |= AKAO_TRACK_FLAG_PENDING_NOTE_PITCH;
    track->repeat_target = next_cursor + (short)value;
}

void SeqOp_LoadInstrumentFromIndex(void *ptr)
{
  int value;
  register int base;

  base = ((AkaoTrack *)ptr)->note_pitch;
  base = ((int)&g_AkaoInstrumentTable) + (base << 6);
  value = ((AkaoInstrument *)base)->adsr_attack_rate;
  ((AkaoTrack *)ptr)->adsr_attack_rate = value;
  value = ((AkaoInstrument *)base)->adsr_decay_rate;
  ((AkaoTrack *)ptr)->adsr_decay_rate = value;
  value = ((AkaoInstrument *)base)->adsr_sustain_level;
  ((AkaoTrack *)ptr)->adsr_sustain_level = value;
  value = ((AkaoInstrument *)base)->adsr_sustain_rate;
  ((AkaoTrack *)ptr)->adsr_sustain_rate = value;
  value = ((AkaoInstrument *)base)->adsr_release_rate;
  ((AkaoTrack *)ptr)->adsr_release_rate = value;
  value = ((AkaoInstrument *)base)->adsr_attack_mode;
  ((AkaoTrack *)ptr)->adsr_attack = value;
  value = ((AkaoInstrument *)base)->adsr_sustain_mode;
  ((AkaoTrack *)ptr)->adsr_sustain = value;
  value = ((AkaoTrack *)ptr)->update_flags;
  base = ((AkaoInstrument *)base)->adsr_release_mode;
  value |= AKAO_VOICE_PARAM_ADSR_ALL;
  ((AkaoTrack *)ptr)->update_flags = value;
  ((AkaoTrack *)ptr)->adsr_release = base;
}

void sndTrackReadExpression(AkaoTrack *track) {
    unsigned char *cursor = track->pc;

    track->pc = cursor + 1;
    track->expression = (signed char)*cursor;
}

void sndTrackAdjustExpression(AkaoTrack *track) {
    unsigned char *cursor = track->pc;

    track->pc = cursor + 1;
    track->expression += (signed char)*cursor;
}

void SeqOp_SetVibratoParams(AkaoTrack *track) {
    unsigned char *cursor;
    unsigned char *next_cursor;
    int value;

    cursor = track->pc;
    track->pc = cursor + 1;
    value = *cursor;
    track->vibrato_duration = value;
    if (value == 0) {
        track->vibrato_duration = 0x100;
    }

    next_cursor = track->pc;
    track->pc = next_cursor + 1;
    track->vibrato_delta = (signed char)*next_cursor;
}

void SeqOp_SetTremoloParams(AkaoTrack *track) {
    unsigned char *cursor;
    int value;

    cursor = track->pc;
    track->pc = cursor + 1;
    value = *cursor;
    track->tremolo_duration = value;
    if (value == 0) {
        track->tremolo_duration = 0x100;
    }

    track->tremolo_delta = 0;
    track->tremolo_counter = 0;
    track->tremolo_phase = 1;
}

void SeqOp_StopTremolo(AkaoTrack *track) {
    track->tremolo_duration = 0;
}

void sndTrackReadDetune(AkaoTrack *track) {
    unsigned char *cursor = track->pc;

    track->pc = cursor + 1;
    track->detune = (signed char)*cursor;
}

void sndTrackAdjustDetune(AkaoTrack *track) {
    unsigned char *cursor = track->pc;

    track->pc = cursor + 1;
    track->detune += (signed char)*cursor;
}

void SeqOp_SetPitchLFO(void *ptr) {
    AkaoTrack *track;
    AkaoU8 *pc;
    void *table;
    int duration;
    int target;
    int lfo_target;
    unsigned int masked;
    unsigned int depth;
    register unsigned int lfo_depth asm("$3");
    int scale;
    int scaled;
    int product;
    int use_base_scale;
    int selector;

    track = ptr;
    track->flags |= AKAO_TRACK_FLAG_PITCH_LFO;

    if (track->parent_track_id != 0) {
        pc = track->pc;
        track->pitch_lfo_delay = 0;
        track->pc = pc + 1;
        target = pc[0];
        if (target != 0) {
            track->pitch_lfo_target = target << 8;
        }
    } else {
        AkaoU8 *pc_zero;
        pc_zero = track->pc;
        track->pc = pc_zero + 1;
        track->pitch_lfo_delay = pc_zero[0];
    }

    {
        AkaoU8 *pc_duration;
        pc_duration = track->pc;
        track->pc = pc_duration + 1;
        duration = pc_duration[0];
    }
    track->pitch_lfo_duration = duration;
    if (duration == 0) {
        track->pitch_lfo_duration = 0x100;
    }

    pc = track->pc;
    scale = *(AkaoU16 *)&track->pitch_base;
    track->pc = pc + 1;
    selector = pc[0];

    lfo_target = track->pitch_lfo_target;
    track->pitch_lfo_selector = selector;
    masked = lfo_target & 0x7F00;
    depth = masked >> 8;
    use_base_scale = lfo_target & 0x8000;
    if (use_base_scale == 0) {
        scaled = ((scale << 4) - scale) >> 8;
        product = depth * scaled;
    } else {
        product = depth * scale;
    }

    lfo_depth = (unsigned int)product >> 7;
    /* Finish the depth calculation before reloading the stored selector. */
    asm("" : : "r"(lfo_depth) : "memory");
    selector = track->pitch_lfo_selector;
    track->pitch_lfo_depth = lfo_depth;

    table = D_8009C080[selector];
    track->pitch_lfo_counter = track->pitch_lfo_delay;
    track->pitch_lfo_phase = 1;
    track->pitch_lfo_table = table;
}

void SeqOp_UpdatePitchLFOTarget(AkaoTrack *track) {
    AkaoU8 *pc;
    int value;
    unsigned int masked;
    unsigned int depth;
    int scale;
    int scaled;
    int product;
    int use_base_scale;

    pc = track->pc;
    track->pc = pc + 1;
    value = pc[0] << 8;
    masked = value & 0x7F00;
    asm volatile("" : "=r"(masked) : "0"(masked));
    depth = masked >> 8;
    track->pitch_lfo_target = value;
    use_base_scale = value & 0x8000;
    scale = track->pitch_base;
    if (use_base_scale == 0) {
        scaled = ((scale << 4) - scale) >> 8;
        product = depth * scaled;
    } else {
        product = depth * scale;
    }
    track->pitch_lfo_depth = (unsigned int)product >> 7;
}
