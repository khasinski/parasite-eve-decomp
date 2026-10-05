/* MASPSX_FLAGS: --expand-div */
#include "pe1/akao.h"
#include "pe1/akao_script.h"
typedef signed char s8;
typedef signed int s32;

extern char *g_AkaoCurTrack;
extern unsigned int g_SpuActiveVoiceMask;


extern AkaoInstrument g_AkaoInstrumentTable[];
void SeqOp_SetVoiceInstrument(AkaoTrack *track_arg, AkaoInstrument *instrument_arg, int sample_header);


extern int g_AkaoVoiceMaskScratch;
extern unsigned int g_AkaoTrack34Mask;
extern unsigned int g_AkaoTrack38Mask;
extern unsigned int g_AkaoTrack3CMask;



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



void SeqOp_SetPanSlide(AkaoScriptState *state) {
    u8 *pc = state->pc;
    int duration;
    int delta;

    state->pc = pc + 1;
    duration = pc[0];
    if (duration == 0) {
        duration = 0x100;
    }
    state->pc = pc + 2;
    delta = ((pc[1] << 8) - state->pan_current) / duration;
    state->pan_duration = duration;
    state->pan_delta = delta;
}


extern void *g_AkaoSoundEntryTable[];

void SeqOp_StopPitchLFO(AkaoTrack *track) {
    track->pitch_lfo_value = 0;
    track->flags &= ~AKAO_TRACK_FLAG_PITCH_LFO;
    track->update_flags |= AKAO_VOICE_PARAM_PITCH;
}

void SeqOp_SetVolumeLFO(AkaoTrack *track) {
    u8 *pc;
    int value;
    int selector;
    int tmp;
    void *entry;

    track->flags |= AKAO_TRACK_FLAG_VOLUME_LFO;

    if (track->parent_track_id != 0) {
        track->volume_lfo_delay = 0;
        pc = track->pc;
        track->pc = pc + 1;
        value = pc[0];
        if (value != 0) {
            track->volume_lfo_target = value << 8;
        }
    } else {
        track->volume_lfo_delay = *track->pc++;
    }

    /* The chained assignment and the *(*pp)++ consume idiom are load-bearing:
     * they keep the stream pointer temp in $v0 as retail allocates it. */
    value = (track->volume_lfo_duration = *track->pc++);
    if (value == 0) {
        track->volume_lfo_duration = 0x100;
    }

    pc = track->pc;
    track->pc = pc + 1;
    selector = pc[0];
    tmp = track->volume_lfo_delay;
    track->volume_lfo_selector = selector;
    entry = g_AkaoSoundEntryTable[selector];
    track->volume_lfo_counter = tmp;
    track->volume_lfo_phase = 1;
    track->volume_lfo_table = entry;
}

void SeqOp_UpdateVolumeLFOTarget(AkaoTrack *track) {
    unsigned char *cursor = track->pc;

    track->pc = cursor + 1;
    track->volume_lfo_target = *cursor << 8;
}


void SeqOp_SetExpressionSlide(AkaoScriptState *state) {
    u8 *pc = state->pc;
    int duration;
    int delta;

    state->pc = pc + 1;
    duration = pc[0];
    if (duration == 0) {
        duration = 0x100;
    }
    state->pc = pc + 2;
    delta = ((pc[1] << 8) - state->expr_current) / duration;
    state->expr_duration = duration;
    state->expr_delta = delta;
}


extern void *g_AkaoSoundEntryTable[];

void SeqOp_ResetTrack(AkaoTrack *track) {
    track->volume_lfo_value = 0;
    track->flags &= ~AKAO_TRACK_FLAG_VOLUME_LFO;
    track->update_flags |= AKAO_VOICE_PARAM_VOLUME;
}

void SeqOp_LoadSoundEntry(AkaoTrack *track) {
    u8 *pc;
    int duration;
    int selector;

    track->flags |= AKAO_TRACK_FLAG_AUX_LFO;

    pc = track->pc;
    track->pc = pc + 1;
    duration = pc[0];
    track->aux_lfo_duration = duration;
    if (duration == 0) {
        track->aux_lfo_duration = 0x100;
    }

    pc = track->pc;
    track->pc = pc + 1;
    selector = pc[0];
    track->aux_lfo_selector = selector;
    {
        void *entry = g_AkaoSoundEntryTable[selector];
        track->aux_lfo_phase = 1;
        track->aux_lfo_table = entry;
    }
}

void SeqOp_SetPitchBase(AkaoTrack *track) {
    unsigned char *cursor = track->pc;

    track->pc = cursor + 1;
    track->aux_lfo_target = *cursor << 7;
}


void SeqOp_PitchSlide(AkaoScriptState *state) {
    u8 *pc = state->pc;
    int duration;
    int target_delta;

    state->pc = pc + 1;
    duration = pc[0];
    if (duration == 0) {
        duration = 0x100;
    }
    state->pc = pc + 2;
    target_delta = ((pc[1] << 7) - state->pitch_current) / duration;
    state->pitch_duration = duration;
    state->pitch_delta = target_delta;
}




extern char *g_AkaoCurTrack;
extern u32 g_AkaoVoiceUpdateFlags;
extern unsigned int g_AkaoTrack34Mask;

void Seq_MarkTrack34MaskDirty(void);

void SeqOp_ResetFlag4State(AkaoTrack *track) {
    track->aux_lfo_value = 0;
    track->flags &= ~AKAO_TRACK_FLAG_AUX_LFO;
    track->update_flags |= AKAO_VOICE_PARAM_VOLUME;
}

void SeqOp_SetTrack34Mask(AkaoTrack *track, u32 mask) {
    if (track->parent_track_id == 0) {
        ((AkaoTrack *)g_AkaoCurTrack)->voice_mask_a |= mask;
    } else {
        g_AkaoTrack34Mask |= mask;
    }

    g_AkaoVoiceUpdateFlags |= AKAO_VOICE_PARAM_PITCH;
    Seq_MarkTrack34MaskDirty();
}

void SeqOp_ClearTrack34Mask(AkaoTrack *track, u32 mask) {
    if (track->parent_track_id == 0) {
        ((AkaoTrack *)g_AkaoCurTrack)->voice_mask_a &= ~mask;
    } else {
        g_AkaoTrack34Mask &= ~mask;
    }

    g_AkaoVoiceUpdateFlags |= AKAO_VOICE_PARAM_PITCH;
    Seq_MarkTrack34MaskDirty();
    track->key_on_delay = 0;
}



extern unsigned int g_AkaoTrack3CMask;

void Seq_MarkTrack3CMaskDirty(void);

void SeqOp_SetMask(AkaoTrack *track, u32 mask) {
    if (track->parent_track_id == 0) {
        ((AkaoTrack *)g_AkaoCurTrack)->voice_mask_b |= mask;
    } else if ((track->flags & 0x10000) != 0) {
        g_AkaoTrack3CMask |= mask;
    }

    Seq_MarkTrack3CMaskDirty();
}

void SeqOp_ClearTrack3CMask(AkaoTrack *track, u32 mask) {
    if (track->parent_track_id == 0) {
        ((AkaoTrack *)g_AkaoCurTrack)->voice_mask_b &= ~mask;
    } else {
        g_AkaoTrack3CMask &= ~mask;
    }

    Seq_MarkTrack3CMaskDirty();
    track->key_off_delay = 0;
}


extern char *g_AkaoCurTrack;
extern unsigned int g_AkaoTrack38Mask;

void Seq_MarkTrack38MaskDirty(void);

extern u32_1 g_AkaoVoiceUpdateFlags;
extern u16 g_AkaoTrack5ATransposeValue;



extern u8_2 D_800B290C[];

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
      *((u16 *) (g_AkaoCurTrack + 0x5A)) = ((*((u16 *) (g_AkaoCurTrack + 0x5A))) + (value & 0x3F)) & 0x3F;
    }
    else
    {
      track = g_AkaoCurTrack;
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

    track->call_stack_index = index;
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

    *(u8_3 **)&track->pc = cursor + 2;
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

    *(u8_3 **)&track->pc = cursor + 2;
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
    track->fixed_note_length = 0;
    track->pan_duration = value;
    track->note_length = value;
    track->default_note_length = value;
}

void SeqOp_AdjustVolumeTarget(AkaoTrack *track) {
    char *cursor = (char *)track->pc;
    int value;

    track->pc = (unsigned char *)cursor + 1;
    value = *(signed char *)cursor;
    if (value != 0) {
        value += track->default_note_length;
        if (value <= 0) {
            value = 1;
        } else if (value >= 0x100) {
            value = 0xFF;
        }
    }

    track->fixed_note_length = value;
}

extern char *g_AkaoCurTrack;


void SeqOp_SetBranchTarget(AkaoScriptState *state) {
    u8 *pc = state->pc;
    u8 *target;
    int saved;
    int lo;
    int hi;
    int offset;

    saved = state->loop_counter;
    state->pc = pc + 1;
    lo = pc[0];
    target = pc + 2;
    state->pc = target;
    hi = pc[1];
    state->loop_counter_saved = saved;
    state->flags |= 8;
    offset = (s16)(lo | (hi << 8));
    state->branch_target = target + offset;
}

void SeqOp_ClearFlag3(AkaoTrack *track) {
    track->flags &= ~AKAO_TRACK_FLAG_BRANCH_ACTIVE;
}

void SeqOp_StreamPair(unsigned char **stream) {
    unsigned char *cursor = *stream;
    unsigned char value;
    AkaoTrack *track = (AkaoTrack *)g_AkaoCurTrack;

    *stream = cursor + 1;
    track->pitch_slide_duration = cursor[0];

    cursor = *stream;
    *stream = cursor + 1;
    value = cursor[0];
    track->repeat_counters[0] = 0;
    track->pitch_slide_current = 0;
    track->voice_index = value;
}

void SeqOp_ReadTrack64U16(unsigned char **stream) {
    unsigned char *cursor = *stream;
    AkaoTrack *track = (AkaoTrack *)g_AkaoCurTrack;

    *stream = cursor + 1;
    track->repeat_counters[1] = cursor[0];

    cursor = *stream;
    *stream = cursor + 1;
    track->repeat_counters[1] |= cursor[0] << 8;
}






extern u32 D_800B89D0;
extern u32 D_800B89D4;
extern u32 D_800B89D8;
extern u32 D_800B89DC;
extern u32 D_800B89E0;

void Seq_StartNestedStreams(void *ptr, void *first, void *second);

void SeqOp_ReadAdsrDecayAndSustain(void *ptr, int arg) {
    sndTrackReadAdsrDecayRate(ptr);
    ((void (*)())sndTrackReadAdsrSustainLevel)(ptr, arg);
}

void SeqOp_SetDurationBAAndMask(AkaoTrack *track) {
    unsigned char *cursor = track->pc;
    int value;

    track->pc = cursor + 1;
    value = *cursor;
    if (value != 0) {
        value++;
    } else {
        value = 0x101;
    }
    track->key_on_delay = value;
    ((void (*)())SeqOp_SetTrack34Mask)(track);
}

void SeqOp_SetDuration(AkaoTrack *track) {
    unsigned char *ptr = track->pc;
    int value;

    track->pc = ptr + 1;
    value = ptr[0];
    if (value == 0) {
        value = 0x101;
    } else {
        value++;
    }
    track->key_on_delay = value;
}

void SeqOp_SetRepeatCount(AkaoTrack *track) {
    unsigned char *cursor = track->pc;
    int value;

    track->pc = cursor + 1;
    value = *cursor;
    if (value != 0) {
        value++;
    } else {
        value = 0x101;
    }
    track->key_off_delay = value;
    ((void (*)())SeqOp_SetMask)(track);
}

void SeqOp_SetDurationAlt(AkaoTrack *track) {
    unsigned char *ptr = track->pc;
    int value;

    track->pc = ptr + 1;
    value = ptr[0];
    if (value == 0) {
        value = 0x101;
    } else {
        value++;
    }
    track->key_off_delay = value;
}

void SeqOp_StopAndClearTrack(AkaoTrack *track, int arg) {
    track->flags &= ~AKAO_TRACK_FLAG_STOP_CLEAR_MASK;
    ((void (*)())SeqOp_ClearTrack34Mask)(track);
    ((void (*)())SeqOp_ClearTrack3CMask)(track, arg);
    ((void (*)())SeqOp_ClearMask)(track, arg);
    track->tremolo_phase &= 0xFFFA;
}

void SeqOp_SetFlag4(AkaoTrack *track) {
    track->flags |= AKAO_TRACK_FLAG_10;
}

void SeqOp_ClearFlag10(AkaoTrack *track) {
    track->flags &= ~AKAO_TRACK_FLAG_10;
}

void SeqOp_SetFlag20(AkaoTrack *track) {
    track->flags |= AKAO_TRACK_FLAG_20;
}

void SeqOp_ClearFlag20(AkaoTrack *track) {
    track->flags &= ~AKAO_TRACK_FLAG_20;
}

void SeqOp_LoadNestedStreams(AkaoTrack *track) {
    u8 *cursor;
    u32 offset;
    u8 *first;
    u8 *second;

    cursor = *(u8 **)track;
    offset = (cursor[1] << 8) | cursor[0];
    if (offset != 0) {
        first = cursor + offset + 2;
    } else {
        first = 0;
    }

    cursor += 2;
    offset = (cursor[1] << 8) | cursor[0];
    if (offset != 0) {
        second = cursor + offset + 2;
    } else {
        second = 0;
    }

    D_800B89D4 = 0;
    D_800B89D8 = 0;
    D_800B89DC = *(u16 *)((char *)track + 0x76) >> 8;
    D_800B89E0 = *(int *)((char *)track + 0x44) >> 23;
    Seq_StartNestedStreams(&D_800B89D0, first, second);
    *(u8 **)track += 4;
}





void SeqOp_NoteOnWithVoiceAlloc(AkaoTrack *track, s32 arg1, s32 arg2, s16 arg3) {
    register s16 voice_index asm("$7");
    register s32 voice_mask asm("$6");
    s32 used_mask;
    register s32 valid_mask asm("$8");
    s32 flags;
    unsigned char *cursor;

    voice_mask = arg2;
    voice_index = arg3;
    cursor = track->pc;
    track->pc = cursor + 1;
    flags = track->flags;
    track->pitch_slide_current = *cursor << 8;
    track->pitch_slide_duration = 0;
    if (!(flags & AKAO_TRACK_FLAG_VOICE_ALLOCATED)) {
        voice_index = 0;
        voice_mask = 1;
        valid_mask = 0xFFFFFF;
        used_mask = *(s32 *)(g_AkaoCurTrack + 4) | *(s32 *)(g_AkaoCurTrack + 0x30);
loop_2:
        if (used_mask & voice_mask) {
            voice_mask *= 2;
            voice_index += 1;
            if (!(voice_mask & valid_mask)) {

            } else {
                goto loop_2;
            }
        }
        if (voice_mask & 0xFFFFFF) {
            *(s32 *)(g_AkaoCurTrack + 0x30) |= voice_mask;
            track->voice_index = voice_index;
            track->flags |= AKAO_TRACK_FLAG_VOICE_ALLOCATED;
        }
    }
    ((void (*)())SeqOp_SetTrack38Mask)();
}


extern u32 *D_8009D2C8;



void SeqOp_NoteOnWithPitchSlide(void *track) {
    register AkaoTrack *base asm("$6");
    register u8 *seq_first asm("$2");
    u8 *seq;
    int value;
    int step_count;
    int pitch_base;
    int mask;
    register int voice_index asm("$7");
    int check;
    base = (AkaoTrack *)track;
    asm volatile("" : "=r"(base) : "0"(base));

    seq_first = base->pc;
    base->pc = seq_first + 1;
    step_count = seq_first[0];
    base->pitch_slide_duration = step_count;
    if (step_count == 0) {
        step_count = 0x100;
        base->pitch_slide_duration = step_count;
    }

    seq = base->pc;
    pitch_base = base->pitch_slide_current & 0xFF00;
    base->pc = seq + 1;
    value = ((int)(seq[0] << 24) >> 16) - pitch_base;
    base->pitch_slide_current = pitch_base;
    base->pitch_slide_delta = value / base->pitch_slide_duration;

    if ((base->flags & AKAO_TRACK_FLAG_VOICE_ALLOCATED) == 0) {
        voice_index = 0;
        mask = 1;
        check = (int)D_8009D2C8;
        {
            register unsigned int limit asm("$8");
            unsigned int used = ((u32 *)check)[1] | ((u32 *)check)[12];
            limit = 0xFFFFFF;
scan_voice:
            if ((used & mask) != 0) {
                mask <<= 1;
                voice_index++;
                if ((mask & limit) != 0)
                    goto scan_voice;
            }
            check = mask & 0xFFFFFF;
        }

        if (check != 0) {
            D_8009D2C8[0xC] |= mask;
            base->voice_index = voice_index;
            base->flags |= AKAO_TRACK_FLAG_VOICE_ALLOCATED;
        }
    }

    ((void (*)())SeqOp_SetTrack38Mask)(base);
}


extern u32 g_AkaoVoiceUpdateFlags;
extern int g_AkaoSeqLoopCounter;


void Seq_MarkTrack34MaskDirty(void);
void Seq_MarkTrack38MaskDirty(void);
void Seq_MarkTrack3CMaskDirty(void);

void SeqOp_SetSustainFlag(AkaoTrack *track) {
    track->flags |= AKAO_TRACK_FLAG_SUSTAIN;
}

void SeqOp_StopVoice(AkaoTrack *track, u32 arg1) {
    char *state;
    u32 mask;

    if (track->parent_track_id == 0) {
        state = g_AkaoCurTrack;
        mask = arg1 ^ ~0xFF000000u;

        *(u32 *)(state + 4) &= mask;
        if (*(u32 *)(state + 4) == 0) {
            g_AkaoSeqLoopCounter = 0;
            *(u16 *)(state + 0x54) = 0;
        }

        state = g_AkaoCurTrack;
        {
            u32 temp0;
            u32 temp1;

            temp0 = *(u32 *)(state + 8);
            temp1 = *(u32 *)(state + 0x34);
            temp0 &= mask;
            *(u32 *)(state + 8) = temp0;
            temp0 = *(u32 *)(state + 0xC);
            temp1 &= mask;
            *(u32 *)(state + 0x34) = temp1;
            temp1 = *(u32 *)(state + 0x3C);
            temp0 &= mask;
            *(u32 *)(state + 0xC) = temp0;
            temp0 = *(u32 *)(state + 0x38);
            temp1 &= mask;
            *(u32 *)(state + 0x3C) = temp1;
            temp0 &= mask;
            *(u32 *)(state + 0x38) = temp0;
        }

        if ((track->flags & AKAO_TRACK_FLAG_VOICE_ALLOCATED) != 0) {
            *(u32 *)(state + 0x30) &= ~(1 << track->voice_index);
        }
    } else {
        ((void (*)())SeqOp_DeactivateVoice)((char *)track);
    }

    *(u32 *)((char *)track + 0x38) = 0;
    g_AkaoVoiceUpdateFlags |= AKAO_VOICE_PARAM_PITCH;
    Seq_MarkTrack34MaskDirty();
    Seq_MarkTrack38MaskDirty();
    Seq_MarkTrack3CMaskDirty();
}

void SeqOp_Noop(AkaoTrack *track, u32 mask) {
    SeqOp_StopVoice(track, mask);
}
