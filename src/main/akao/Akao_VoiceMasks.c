#include "pe1/akao/voice_masks.h"

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

void Akao_SetVoicePitch(void) {
    unsigned secondary, primary, keep;
    AkaoSequencerBank *bank;
    int result = 0;
    keep = ~(g_SpuActiveVoiceMask | g_SpuStoppedVoiceMask);
    secondary = g_AkaoCurTrack[1].active_voice_mask & g_AkaoCurTrack[1].key_off_request_mask;
    if (secondary & g_AkaoCurTrack[1].pending_voice_mask) {
        g_AkaoCurTrack++;
        Spu_VoiceMaskCompose(g_AkaoVoiceStateTable2, &result, secondary & g_AkaoCurTrack->pending_voice_mask, keep);
        bank = g_AkaoCurTrack;
        secondary &= ~bank->pending_voice_mask;
        g_AkaoCurTrack--;
        bank->key_off_request_mask &= ~bank->pending_voice_mask;
    }
    primary = g_AkaoCurTrack->active_voice_mask & g_AkaoCurTrack->key_off_request_mask;
    if (primary & g_AkaoCurTrack->pending_voice_mask) {
        Spu_VoiceMaskCompose(g_AkaoVoiceStateTable, &result, primary & g_AkaoCurTrack->pending_voice_mask, keep);
        bank = g_AkaoCurTrack;
        primary &= ~bank->pending_voice_mask;
        /* Preserve the retail reload before clearing the bank requests. */
        __asm__("" ::: "memory");
        bank->key_off_request_mask &= ~bank->pending_voice_mask;
    }
    if (secondary) {
        g_AkaoCurTrack++;
        Spu_VoiceMaskCompose(g_AkaoVoiceStateTable2, &result, secondary, keep);
        g_AkaoCurTrack->key_off_request_mask = 0;
        g_AkaoCurTrack--;
    }
    if (primary) {
        Spu_VoiceMaskCompose(g_AkaoVoiceStateTable, &result, primary, keep);
        g_AkaoCurTrack->key_off_request_mask = 0;
    }
    result |= g_SpuPendingKeyOffMask;
    g_SpuPendingKeyOffMask = 0;
    if (result) Spu_WriteKeyOff(result);
}

void Seq_MarkTrack34MaskDirty(void) {
    g_AkaoVoiceUpdateFlags |= 0x100;
}

void Akao_SetVoiceVolume(void) {
    unsigned secondary, primary, keep;
    int result = 0;
    keep = ~(g_SpuActiveVoiceMask | g_SpuStoppedVoiceMask);
    secondary = g_AkaoCurTrack[1].active_voice_mask & g_AkaoCurTrack[1].key_off_dirty_mask;
    if (secondary & g_AkaoCurTrack[1].pending_voice_mask) {
        g_AkaoCurTrack++;
        Spu_VoiceMaskCompose(g_AkaoVoiceStateTable2, &result, secondary & g_AkaoCurTrack->pending_voice_mask, keep);
        secondary &= ~g_AkaoCurTrack->pending_voice_mask;
        g_AkaoCurTrack--;
    }
    primary = g_AkaoCurTrack->active_voice_mask & g_AkaoCurTrack->key_off_dirty_mask;
    if (primary & g_AkaoCurTrack->pending_voice_mask) {
        Spu_VoiceMaskCompose(g_AkaoVoiceStateTable, &result, primary & g_AkaoCurTrack->pending_voice_mask, keep);
        primary &= ~g_AkaoCurTrack->pending_voice_mask;
    }
    if (secondary) {
        g_AkaoCurTrack++;
        Spu_VoiceMaskCompose(g_AkaoVoiceStateTable2, &result, secondary, keep);
        g_AkaoCurTrack--;
    }
    if (primary) Spu_VoiceMaskCompose(g_AkaoVoiceStateTable, &result, primary, keep);
    result |= g_AkaoTrack34Mask;
    D_800C0DD4 = result;
    g_AkaoVoiceUpdateFlags |= 0x100;
}

void Seq_MarkTrack38MaskDirty(void) {
    g_AkaoVoiceUpdateFlags |= 0x100;
}

void Akao_SetVoiceAdsr(void) {
    unsigned secondary, primary, keep;
    int result = 0;
    keep = ~(g_SpuActiveVoiceMask | g_SpuStoppedVoiceMask);
    secondary = g_AkaoCurTrack[1].active_voice_mask & g_AkaoCurTrack[1].volume_dirty_mask;
    if (secondary & g_AkaoCurTrack[1].pending_voice_mask) {
        g_AkaoCurTrack++;
        Spu_VoiceMaskCompose(g_AkaoVoiceStateTable2, &result, secondary & g_AkaoCurTrack->pending_voice_mask, keep);
        secondary &= ~g_AkaoCurTrack->pending_voice_mask;
        g_AkaoCurTrack--;
    }
    primary = g_AkaoCurTrack->active_voice_mask & g_AkaoCurTrack->volume_dirty_mask;
    if (primary & g_AkaoCurTrack->pending_voice_mask) {
        Spu_VoiceMaskCompose(g_AkaoVoiceStateTable, &result, primary & g_AkaoCurTrack->pending_voice_mask, keep);
        primary &= ~g_AkaoCurTrack->pending_voice_mask;
    }
    if (secondary) {
        g_AkaoCurTrack++;
        Spu_VoiceMaskCompose(g_AkaoVoiceStateTable2, &result, secondary, keep);
        g_AkaoCurTrack--;
    }
    if (primary) Spu_VoiceMaskCompose(g_AkaoVoiceStateTable, &result, primary, keep);
    result |= g_AkaoTrack38Mask;
    D_800C0DD0 = result;
    g_AkaoVoiceUpdateFlags |= 0x100;
}

void Seq_MarkTrack3CMaskDirty(void) {
    g_AkaoVoiceUpdateFlags |= 0x100;
}

void Akao_SetVoiceStartAddr(void) {
    unsigned secondary, primary, keep;
    int result = 0;
    keep = ~(g_SpuActiveVoiceMask | g_SpuStoppedVoiceMask);
    secondary = g_AkaoCurTrack[1].active_voice_mask & g_AkaoCurTrack[1].adsr_dirty_mask;
    if (secondary & g_AkaoCurTrack[1].pending_voice_mask) {
        g_AkaoCurTrack++;
        Spu_VoiceMaskCompose(g_AkaoVoiceStateTable2, &result, secondary & g_AkaoCurTrack->pending_voice_mask, keep);
        secondary &= ~g_AkaoCurTrack->pending_voice_mask;
        g_AkaoCurTrack--;
    }
    primary = g_AkaoCurTrack->active_voice_mask & g_AkaoCurTrack->adsr_dirty_mask;
    if (primary & g_AkaoCurTrack->pending_voice_mask) {
        Spu_VoiceMaskCompose(g_AkaoVoiceStateTable, &result, primary & g_AkaoCurTrack->pending_voice_mask, keep);
        primary &= ~g_AkaoCurTrack->pending_voice_mask;
    }
    if (secondary) {
        g_AkaoCurTrack++;
        Spu_VoiceMaskCompose(g_AkaoVoiceStateTable2, &result, secondary, keep);
        g_AkaoCurTrack--;
    }
    if (primary) Spu_VoiceMaskCompose(g_AkaoVoiceStateTable, &result, primary, keep);
    result |= g_AkaoTrack3CMask;
    D_800C0DD8 = result;
    g_AkaoVoiceUpdateFlags |= 0x100;
}
