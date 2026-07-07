#include "include_asm.h"

#include "pe1/akao.h"

#include "pe1/akao_script.h"

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
    track->field_E2 = 0;
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
    register AkaoTrack *track asm("$5");
    AkaoU8 *pc;
    void *table;
    int duration;
    int target;
    register int lfo_target asm("$3");
    unsigned int masked;
    unsigned int depth;
    register int scale asm("$4");
    register int scaled asm("$2");
    int product;
    int use_base_scale;
    int selector;

    track = ptr;
    __asm__("" : "=r"(track) : "0"(track));
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
        register AkaoU8 *pc_zero asm("$2");

        pc_zero = track->pc;
        track->pc = pc_zero + 1;
        track->pitch_lfo_delay = pc_zero[0];
    }

    {
        register AkaoU8 *pc_duration asm("$2");

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

    /* Match note: keep pitch_lfo_depth in $v1 across the selector reload. */
    asm volatile("srl $3,%0,7" : : "r"(product));
    selector = track->pitch_lfo_selector;
    asm volatile("sh $3,0x92(%0)" : : "r"(track));

    table = D_8009C080[selector];
    track->pitch_lfo_counter = track->pitch_lfo_delay;
    track->pitch_lfo_phase = 1;
    track->pitch_lfo_table = table;
}
