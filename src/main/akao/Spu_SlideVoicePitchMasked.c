#include "common.h"
/* MASPSX_FLAGS: --expand-div */
#include "pe1/akao.h"

extern char g_AkaoVoiceChannelTable[];
extern u32 g_SpuActiveVoiceMask;

static inline short PitchDelta(int target, int current, int step) {
    return (short)((target << 8) - current) / (short)step;
}

void Spu_SlideVoicePitchMasked(int *arg0) {
    char *base;
    u32 active;
    u32 mask;
    u32 i;
    AkaoTrack *voice;
    int step;
    int delta;

    base = g_AkaoVoiceChannelTable;
    active = g_SpuActiveVoiceMask;
    mask = AKAO_SPU_VOICE_SFX_START_MASK;

    if (arg0[2] != 0) {
        i = 0;
        voice = (AkaoTrack *)base;
        do {
            if ((active & mask) != 0) {
                if ((voice->key_on_mask & arg0[2]) != 0) {
                    step = 1;
                    if (arg0[3] != 0) {
                        step = arg0[3];
                    }
                    delta = PitchDelta(((u8 *)arg0)[0x10], (int)voice->voice_mask_b, step);
                    voice->pan_base = (short)delta;
                    voice->field_70 = step;
                }
            }
            i++;
            voice++;
            mask <<= 1;
        } while (i < 12);
    } else {
        i = 0;
        voice = (AkaoTrack *)base;
        do {
            if ((active & mask) != 0) {
                if ((int)voice->key_off_mask == arg0[1]) {
                    step = 1;
                    if (arg0[3] != 0) {
                        step = arg0[3];
                    }
                    delta = PitchDelta(((u8 *)arg0)[0x10], (int)voice->voice_mask_b, step);
                    voice->pan_base = (short)delta;
                    voice->field_70 = step;
                }
            }
            i++;
            voice++;
            mask <<= 1;
        } while (i < 12);
    }
}
