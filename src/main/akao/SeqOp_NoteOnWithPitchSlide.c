#include "common.h"
#include "pe1/akao.h"
/* MASPSX_FLAGS: --expand-div */

extern u32 *D_8009D2C8;

void SeqOp_SetTrack38Mask(void *track);

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
    value = ((int)((u32)seq[0] << 24) >> 16) - pitch_base;
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

    SeqOp_SetTrack38Mask(base);
}
