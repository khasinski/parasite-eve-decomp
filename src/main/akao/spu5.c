#include "common.h"
/* MASPSX_FLAGS: --expand-div */
#include "pe1/akao.h"

#include "pe1/akao/voice_masks.h"

void Spu_SetVoicePanImmediateMasked(int *arg0) {
    AkaoTrack *base;
    u32 active;
    u32 mask;
    u32 i;
    AkaoTrack *voice;
    int value;
    int dirty;

    base = g_AkaoVoiceChannelTable;
    active = g_SpuActiveVoiceMask;
    mask = AKAO_SPU_VOICE_SFX_START_MASK;

    if (arg0[2] != 0) {
        i = 0;
        voice = base;
        do {
            if ((active & mask) != 0) {
                if ((voice->key_on_mask & arg0[2]) != 0) {
                    value = ((u8 *)arg0)[0xC];
                    dirty = voice->update_flags;
                    voice->panpot_slide_duration = 0;
                    value <<= 8;
                    dirty |= AKAO_VOICE_PARAM_VOLUME;
                    voice->panpot = value;
                    voice->update_flags = dirty;
                }
            }
            i++;
            voice++;
            mask <<= 1;
        } while (i < 12);
    } else {
        i = 0;
        voice = base;
        do {
            if ((active & mask) != 0) {
                if (voice->key_off_mask == arg0[1]) {
                    value = ((u8 *)arg0)[0xC];
                    dirty = voice->update_flags;
                    voice->panpot_slide_duration = 0;
                    value <<= 8;
                    dirty |= AKAO_VOICE_PARAM_VOLUME;
                    voice->panpot = value;
                    voice->update_flags = dirty;
                }
            }
            i++;
            voice++;
            mask <<= 1;
        } while (i < 12);
    }
}

static inline int PanDelta(int target, int current, int step) {
    return (short)((target << 8) - current) / (short)step;
}

void Spu_SlideVoicePanMasked(int *arg0) {
    AkaoTrack *base;
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
        voice = base;
        do {
            if ((active & mask) != 0) {
                if ((voice->key_on_mask & arg0[2]) != 0) {
                    step = 1;
                    if (arg0[3] != 0) {
                        step = arg0[3];
                    }
                    delta = PanDelta(((u8 *)arg0)[0x10], voice->panpot, step);
                    voice->panpot_delta = delta;
                    voice->panpot_slide_duration = step;
                }
            }
            i++;
            voice++;
            mask <<= 1;
        } while (i < 12);
    } else {
        i = 0;
        voice = base;
        do {
            if ((active & mask) != 0) {
                if (voice->key_off_mask == arg0[1]) {
                    step = 1;
                    if (arg0[3] != 0) {
                        step = arg0[3];
                    }
                    delta = PanDelta(((u8 *)arg0)[0x10], voice->panpot, step);
                    voice->panpot_delta = delta;
                    voice->panpot_slide_duration = step;
                }
            }
            i++;
            voice++;
            mask <<= 1;
        } while (i < 12);
    }
}
