#include "include_asm.h"
#include "pe1/akao.h"

extern unsigned int g_AkaoVoiceUpdateFlags;

void Spu_VoiceMaskCompose(AkaoTrack *track, int *mask_out, int mask, int mask_keep) {
    int bit;
    int idx;

    bit = 1;
    do {
        if ((mask & bit) != 0) {
            idx = track->assigned_voice_index;
            if ((unsigned int) idx < 0x18) {
                *mask_out |= 1 << idx;
            }
        }
        mask &= ~bit;
        track++;
        bit <<= 1;
    } while (mask != 0);

    *mask_out &= mask_keep;
}

INCLUDE_ASM("asm/USA/main/nonmatchings/akao/misc", Akao_SetVoicePitch);

void Seq_MarkTrack34MaskDirty(void) {
    g_AkaoVoiceUpdateFlags |= AKAO_GLOBAL_UPDATE_VOICE_MODES;
}

INCLUDE_ASM("asm/USA/main/nonmatchings/akao/misc", Akao_SetVoiceVolume);

void Seq_MarkTrack38MaskDirty(void) {
    g_AkaoVoiceUpdateFlags |= AKAO_GLOBAL_UPDATE_VOICE_MODES;
}

INCLUDE_ASM("asm/USA/main/nonmatchings/akao/misc", Akao_SetVoiceAdsr);

void Seq_MarkTrack3CMaskDirty(void) {
    g_AkaoVoiceUpdateFlags |= AKAO_GLOBAL_UPDATE_VOICE_MODES;
}
