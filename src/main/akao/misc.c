#include "include_asm.h"
#include "pe1/akao.h"

extern unsigned int g_AkaoVoiceUpdateFlags;
extern unsigned int D_800BCD50;
extern unsigned int D_800BCD5C;
extern unsigned int D_800BCD60;
extern unsigned int D_800BCD6C;
extern unsigned int D_800BCD70;
extern unsigned int D_800C0DD4;
extern unsigned int D_800C0DD0;
extern unsigned int *D_8009D2C8;
extern AkaoTrack D_800BA560[];
extern AkaoTrack D_800B8AC0[];

void Spu_WriteKeyOff(unsigned int mask);

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
    unsigned int mask_out;
    register unsigned int mask_keep asm("$18");
    register unsigned int pending_secondary asm("$17");
    register unsigned int pending_primary asm("$16");
    unsigned int mask;
    register unsigned int *state asm("$8");
    unsigned int blocked_a;
    unsigned int blocked_b;

    state = D_8009D2C8;
    blocked_a = D_800BCD50;
    blocked_b = D_800BCD60;
    mask_out = 0;
    mask_keep = ~(blocked_a | blocked_b);
    pending_secondary = state[0x1B] & state[0x20];
    mask = pending_secondary & state[0x1C];
    if (mask != 0) {
        D_8009D2C8 = (unsigned int *)((char *)state + 0x68);
        Spu_VoiceMaskCompose(D_800BA560, (int *)&mask_out, mask, mask_keep);
        state = D_8009D2C8;
        D_8009D2C8 = (unsigned int *)((char *)state - 0x68);
        pending_secondary &= ~state[2];
        state[6] &= ~state[2];
    }

    state = D_8009D2C8;
    pending_primary = state[1] & state[6];
    mask = pending_primary & state[2];
    if (mask != 0) {
        Spu_VoiceMaskCompose(D_800B8AC0, (int *)&mask_out, mask, mask_keep);
        state = D_8009D2C8;
        pending_primary &= ~state[2];
        state[6] &= ~state[2];
    }

    if (pending_secondary != 0) {
        state = D_8009D2C8;
        D_8009D2C8 = (unsigned int *)((char *)state + 0x68);
        Spu_VoiceMaskCompose(D_800BA560, (int *)&mask_out, pending_secondary, mask_keep);
        D_8009D2C8[6] = 0;
        D_8009D2C8 = (unsigned int *)((char *)D_8009D2C8 - 0x68);
    }

    if (pending_primary != 0) {
        Spu_VoiceMaskCompose(D_800B8AC0, (int *)&mask_out, pending_primary, mask_keep);
        D_8009D2C8[6] = 0;
    }

    mask_out |= D_800BCD5C;
    D_800BCD5C = 0;
    if (mask_out != 0) {
        Spu_WriteKeyOff(mask_out);
    }
}

void Seq_MarkTrack34MaskDirty(void) {
    g_AkaoVoiceUpdateFlags |= AKAO_GLOBAL_UPDATE_VOICE_MODES;
}

void Akao_SetVoiceVolume(void) {
    unsigned int mask_out;
    unsigned int mask_keep;
    unsigned int pending_secondary;
    unsigned int pending_primary;
    unsigned int mask;
    unsigned int *state;

    state = D_8009D2C8;
    mask_out = 0;
    mask_keep = ~(D_800BCD50 | D_800BCD60);
    pending_secondary = state[0x1B] & state[0x27];
    mask = pending_secondary & state[0x1C];
    if (mask != 0) {
        D_8009D2C8 = (unsigned int *)((char *)state + 0x68);
        Spu_VoiceMaskCompose(D_800BA560, (int *)&mask_out, mask, mask_keep);
        state = D_8009D2C8;
        D_8009D2C8 = (unsigned int *)((char *)state - 0x68);
        pending_secondary &= ~state[2];
    }

    state = D_8009D2C8;
    pending_primary = state[1] & state[0xD];
    mask = pending_primary & state[2];
    if (mask != 0) {
        Spu_VoiceMaskCompose(D_800B8AC0, (int *)&mask_out, mask, mask_keep);
        pending_primary &= ~D_8009D2C8[2];
    }

    if (pending_secondary != 0) {
        state = D_8009D2C8;
        D_8009D2C8 = (unsigned int *)((char *)state + 0x68);
        Spu_VoiceMaskCompose(D_800BA560, (int *)&mask_out, pending_secondary, mask_keep);
        D_8009D2C8 = (unsigned int *)((char *)D_8009D2C8 - 0x68);
    }

    if (pending_primary != 0) {
        Spu_VoiceMaskCompose(D_800B8AC0, (int *)&mask_out, pending_primary, mask_keep);
    }

    mask_out |= D_800BCD6C;
    D_800C0DD4 = mask_out;
    g_AkaoVoiceUpdateFlags |= 0x100;
}

void Seq_MarkTrack38MaskDirty(void) {
    g_AkaoVoiceUpdateFlags |= AKAO_GLOBAL_UPDATE_VOICE_MODES;
}

void Akao_SetVoiceAdsr(void) {
    unsigned int mask_out;
    unsigned int mask_keep;
    unsigned int pending_secondary;
    unsigned int pending_primary;
    unsigned int mask;
    unsigned int *state;

    state = D_8009D2C8;
    mask_out = 0;
    mask_keep = ~(D_800BCD50 | D_800BCD60);
    pending_secondary = state[0x1B] & state[0x28];
    mask = pending_secondary & state[0x1C];
    if (mask != 0) {
        D_8009D2C8 = (unsigned int *)((char *)state + 0x68);
        Spu_VoiceMaskCompose(D_800BA560, (int *)&mask_out, mask, mask_keep);
        state = D_8009D2C8;
        D_8009D2C8 = (unsigned int *)((char *)state - 0x68);
        pending_secondary &= ~state[2];
    }

    state = D_8009D2C8;
    pending_primary = state[1] & state[0xE];
    mask = pending_primary & state[2];
    if (mask != 0) {
        Spu_VoiceMaskCompose(D_800B8AC0, (int *)&mask_out, mask, mask_keep);
        pending_primary &= ~D_8009D2C8[2];
    }

    if (pending_secondary != 0) {
        state = D_8009D2C8;
        D_8009D2C8 = (unsigned int *)((char *)state + 0x68);
        Spu_VoiceMaskCompose(D_800BA560, (int *)&mask_out, pending_secondary, mask_keep);
        D_8009D2C8 = (unsigned int *)((char *)D_8009D2C8 - 0x68);
    }

    if (pending_primary != 0) {
        Spu_VoiceMaskCompose(D_800B8AC0, (int *)&mask_out, pending_primary, mask_keep);
    }

    mask_out |= D_800BCD70;
    D_800C0DD0 = mask_out;
    g_AkaoVoiceUpdateFlags |= 0x100;
}

void Seq_MarkTrack3CMaskDirty(void) {
    g_AkaoVoiceUpdateFlags |= AKAO_GLOBAL_UPDATE_VOICE_MODES;
}
