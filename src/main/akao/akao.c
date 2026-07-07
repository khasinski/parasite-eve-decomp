#include "pe1/akao.h"

void Akao_SetNotePitch(AkaoTrack *track, int arg);

void Akao_InitVoiceState(AkaoTrack *track, void *script) {
    track->volume_base = 0x6E00;
    track->pc = script;
    track->expression = 0;
    track->detune = 0;
    track->tremolo_duration = 0;
    track->voice_mask_a = 0;
    track->vibrato_delta = 0;
    track->field_7A = 0;
    track->field_D2 = 0;
    track->field_D0 = 0;
    track->expression_value = 0x32000000;
    track->expression_duration = 0;
    track->call_stack_index = 0;
    track->flags = 0;
    track->aux_lfo_value = 0;
    track->tremolo_phase = 0;
    track->aux_lfo_target = 0;
    track->volume_lfo_target = 0;
    track->pitch_lfo_target = 0;
    track->aux_lfo_slide_duration = 0;
    track->volume_lfo_slide_duration = 0;
    track->pitch_lfo_slide_duration = 0;
    track->key_off_delay = 0;
    track->key_on_delay = 0;
    Akao_SetNotePitch(track, 0);
}

unsigned int Akao_ForEachVoiceMasked(AkaoTrack *track, unsigned int mask) {
    unsigned int bit;
    unsigned int result;
    unsigned int index;

    index = 0;
    result = 0;
    for (bit = 1; index < 24; index++, track++) {
        if ((mask & (bit << index)) != 0) {
            unsigned int target = track->assigned_voice_index;

            if (target < 24) {
                result |= bit << target;
            }
        }
    }
    return result;
}

void Spu_RebaseStreamAddrs(unsigned char *ptr, int value, int count) {
    unsigned char *next = ptr + 4;
    int delta = value - *(int *)ptr;

    do {
        *(int *)ptr += delta;
        *(int *)next += delta;
        ptr += 0x40;
        next += 0x40;
        count--;
    } while (count != 0);
}
