#include "pe1/akao.h"
#include "pe1/akao/voice_state.h"
#include "pe1/akao/voice_masks.h"

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

#include "common.h"
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
M2C_UNK func_80089960();
M2C_UNK func_80089B28();
M2C_UNK func_80089CF0();

void Akao_StepSequencerVoice(void *arg0) {
    void *arg0v;
    u8 *walk;
    s32 mask;
    s32 bit;
    u8 *pvoice;
    s32 kFFF5;
    register u8 *base asm("$16");
    s32 kFFFa;
    s32 tret;
    s32 tnor;
    s32 tcd;
    s32 tand;
    s32 t2dc;
    s32 tw1;
    s32 tw2;
    register s32 kFFFb asm("$3");
    s32 km102;
    s32 tv0;
    s32 tcde;
    s32 tlhu;
    s32 var_v0;
    void *p2c8a;
    s32 tld;
    void *p2c8b;
    void *p2c8c;
    void *pend;
    register s32 kffe asm("$4");

    arg0v = arg0;
    walk = (u8 *)arg0v;
    kFFFa = 0xFFFFFF;
    p2c8a = g_AkaoCurTrack;
    M2C_FIELD(p2c8a, void **, 0x2C) = arg0v;
    tld = M2C_FIELD(arg0v, s32 *, 0);
    mask = tld & kFFFa;
    tret = Akao_ForEachVoiceMasked(g_AkaoVoiceStateTable2, M2C_FIELD(p2c8a, s32 *, 0x6C));
    tnor = ~tret;
    tcd = ~g_SpuActiveVoiceMask & kFFFa;
    tand = tnor & tcd;
    p2c8b = g_AkaoCurTrack;
    t2dc = g_AkaoSeqPendingFlags;
    g_SpuPendingKeyOffMask |= tand;
    M2C_FIELD(p2c8b, s32 *, 0x18) = 0;
    if (t2dc & 1) {
        M2C_FIELD(p2c8b, s32 *, 4) = 0;
        M2C_FIELD(p2c8b, s32 *, 0x1C) = (M2C_FIELD(p2c8b, s32 *, 0x1C) | mask);
    } else {
        M2C_FIELD(p2c8b, s32 *, 0x1C) = 0;
        M2C_FIELD(p2c8b, s32 *, 4) = (M2C_FIELD(p2c8b, s32 *, 4) | mask);
    }
    walk += 4;
        kFFFb = 0xFFFFFF;
    tw1 = *(s32 *)walk;
    walk += 4;
        bit = 1;
    pvoice = (u8 *)g_AkaoVoiceStateTable;
    kFFF5 = 0xFFFFFF;
    base = pvoice + 0x116;
    km102 = -0x102;
        p2c8c = g_AkaoCurTrack;
    M2C_FIELD(p2c8c, s32 *, 8) = tw1 & kFFFb;
    tw2 = *(s32 *)walk;
    walk += 8;
    M2C_FIELD(p2c8c, s32 *, 0xC) = tw2 & kFFFb;
        tv0 = M2C_FIELD(p2c8c, s32 *, 0) & km102;
    tcde = g_AkaoVoiceKeyOnState & 0x100;
    *(volatile s32 *)p2c8c = tv0;
    M2C_FIELD(p2c8c, s32 *, 0) = tv0 | tcde;
    do {
        if (mask & bit) {
            tlhu = *(u16 *)walk;
            walk += 2;
            *(u8 **)pvoice = walk + tlhu;
            M2C_FIELD(base, s32 *, -0x26) = 0x18;
            M2C_FIELD(base, s16 *, -0xC0) = 4;
            M2C_FIELD(base, s16 *, -0xBE) = 2;
            M2C_FIELD(base, s16 *, -0xAA) = 0x7F00;
            M2C_FIELD(base, s32 *, -0xD2) = 0x3FFF0000;
            M2C_FIELD(base, s16 *, -0x3E) = 0x4000;
            M2C_FIELD(base, s16 *, -0x36) = 0;
            M2C_FIELD(base, s16 *, -0x38) = 0;
            M2C_FIELD(base, s16 *, -0x94) = 0;
            M2C_FIELD(base, s32 *, -0xE2) = 0;
            M2C_FIELD(base, s16 *, -0x32) = 0;
            M2C_FIELD(base, s16 *, -0x9C) = 0;
            M2C_FIELD(base, s16 *, -0x44) = 0;
            M2C_FIELD(base, s16 *, -0x46) = 0;
            M2C_FIELD(base, u16 *, -0xA0) = 0x8000;
            M2C_FIELD(base, s16 *, -0x9E) = 0;
            M2C_FIELD(base, s16 *, -0x94) = 0;
            M2C_FIELD(base, s16 *, -0xA2) = 0;
            M2C_FIELD(base, s16 *, -0xA4) = 0;
            M2C_FIELD(base, void **, -0x102) = arg0v;
            M2C_FIELD(base, s16 *, -0x92) = 0;
            M2C_FIELD(base, s16 *, -0x2A) = 0;
            M2C_FIELD(base, s32 *, -0xDE) = 0;
            M2C_FIELD(base, s16 *, -0x48) = 0;
            M2C_FIELD(base, s16 *, -0x62) = 0;
            M2C_FIELD(base, s16 *, -0x70) = 0;
            M2C_FIELD(base, s16 *, -0x82) = 0;
            M2C_FIELD(base, s16 *, -0x60) = 0;
            M2C_FIELD(base, s16 *, -0x6E) = 0;
            M2C_FIELD(base, s16 *, -0x80) = 0;
            M2C_FIELD(base, s16 *, -0x5A) = 0;
            M2C_FIELD(base, s16 *, -0x5C) = 0;
            Akao_SetNotePitch(pvoice, 0);
            var_v0 = ~bit;
        } else {
            M2C_FIELD(base, s16 *, -0xC0) = 3;
            M2C_FIELD(base, s16 *, -0xBE) = 1;
            *(void **)pvoice = g_AkaoDefaultVoiceProgram;
            M2C_FIELD(base, s16 *, 0) = 5;
            M2C_FIELD(base, s32 *, -0x22) = (M2C_FIELD(base, s32 *, -0x22) | 0x4400);
            __asm__ __volatile__("");
            var_v0 = ~bit;
        }
        kFFF5 &= var_v0;
        mask &= var_v0;
        base += 0x11C;
        pvoice += 0x11C;
        bit *= 2;
    } while (kFFF5 != 0);
        pend = g_AkaoCurTrack;
    M2C_FIELD(pend, s32 *, 0x20) = 0xFFFF0000;
    M2C_FIELD(pend, s32 *, 0x28) = 1;
    M2C_FIELD(pend, s16 *, 0x52) = 0;
    M2C_FIELD(pend, s32 *, 0x40) = 0;
    M2C_FIELD(pend, s16 *, 0x58) = 0;
    M2C_FIELD(pend, s32 *, 0x44) = 0;
    g_AkaoVoiceUpdateFlags = 0;
    M2C_FIELD(pend, s16 *, 0x62) = 0;
    M2C_FIELD(pend, s16 *, 0x60) = 0;
    M2C_FIELD(pend, s16 *, 0x5E) = 0;
    M2C_FIELD(pend, s16 *, 0x64) = 0;
    M2C_FIELD(pend, s32 *, 0x34) = 0;
    M2C_FIELD(pend, s32 *, 0x38) = 0;
    M2C_FIELD(pend, s32 *, 0x3C) = 0;
    M2C_FIELD(pend, s16 *, 0x56) = 0;
    M2C_FIELD(pend, s32 *, 0x10) = 0;
    kffe = 0xFFFFFF;
    __asm__ __volatile__("");
    M2C_FIELD(pend, s32 *, 0x14) = kffe;
    M2C_FIELD(pend, s32 *, 0x30) = 0;
    func_80089960(kffe);
    func_80089B28();
    func_80089CF0();
}

typedef unsigned short u16_1;

void Spu_ManageVoices(int arg0, int arg1);
void SeqOp_DeactivateVoice(char *ptr, int mask);
void Seq_MarkTrack34MaskDirty(void);
void Seq_MarkTrack38MaskDirty(void);
void Seq_MarkTrack3CMaskDirty(void);

void Akao_InitVoices(int arg0, char *arg1) {
    unsigned int i;
    int mode3;
    int mode1;
    char *fallback;
    int mode5;
    AkaoTrack *voice;

    if (arg0 == 0) {
        if (((AkaoSequencerBank *)g_AkaoCurTrack)->active_voice_mask != 0) {
            goto body;
        }
        return;
    } else if (arg0 != ((AkaoSequencerBank *)g_AkaoCurTrack)->bank_id) {
        return;
    }

body:
    ((AkaoSequencerBank *)g_AkaoCurTrack)->key_off_request_mask = 0xFFFFFF;

    i = 0;
    mode3 = 3;
    mode1 = 1;
    fallback = g_AkaoDefaultVoiceProgram;
    mode5 = 5;
    voice = (AkaoTrack *)arg1;
    do {
        i++;
        voice->field_56 = mode3;
        voice->pan_duration = mode1;
        voice->pc = (AkaoU8 *)fallback;
        voice->update_flags |= AKAO_VOICE_PARAM_ADSR_RELEASE;
        voice->adsr_release_rate = mode5;
        voice++;
    } while (i < 0x18);
}

void Spu_ManageVoices(int arg0, int arg1)
{
    u32 mask;
    u32 i;
    unsigned int mode_bits;
    AkaoTrack *field;
    AkaoTrack *voice;
    u32 id;
    u32 active;
    int control;
    int value;
    int flags;
    int best;
    control = arg1;
    mask = AKAO_SPU_VOICE_SFX_START_MASK;
    id = (u16_1) arg0;
    if (id == 0xFFFF)
    {
        return;
    }
    voice = (AkaoTrack *) g_AkaoVoiceChannelTable;
    mode_bits = control;
    active = g_SpuActiveVoiceMask;
    if ((mode_bits & 0x0FFFFFFF) != 0)
    {
        u32 flag1;
        u32 id;
        i = 0;
        flag1 = 0x100000;
        id = 0x200000;
        field = voice;
        do
        {
            if ((active & mask) != 0)
            {
                if ((field->key_on_mask & arg1) != 0)
                {
                    flags = field->flags;
                    value = flags & flag1;
                    if (value != 0)
                    {
                        value = flags | id;
                        field->flags = value;
                    }
                    else
                    {
                        g_SpuPendingKeyOffMask |= mask;
                        SeqOp_DeactivateVoice((char *) voice, mask);
                        field->flags = 0;
                    }
                }
            }
            i++;
            field++;
            voice++;
            mask <<= 1;
        }
        while (i < 12);
        goto finish;
    }
    if (arg1 < 0)
    {
        voice = ((AkaoTrack *) g_AkaoVoiceChannelTable) + id;
        mask <<= id;
        if ((active & mask) != 0)
        {
            Spu_ManageVoices(voice->key_off_mask, 0);
        }
        mask <<= 1;
        voice++;
        value = active & mask;
        if (value != 0)
        {
            Spu_ManageVoices(voice->key_off_mask, 0);
        }
        return;
    }
    value = mode_bits & 0x40000000;
    if (value != 0)
    {
        u32 flag1;
        int control;
        i = 0;
        do
        {
            value = voice->key_on_mask;
            if (value != 0)
            {
                active &= ~mask;
            }
            i++;
            voice++;
            mask <<= 1;
        }
        while (i < 12);
        voice = (AkaoTrack *) g_AkaoVoiceChannelTable;
        mask = AKAO_SPU_VOICE_SFX_START_MASK;
        best = 0;
        i = 0;
        do
        {
            if ((active & mask) != 0)
            {
                flags = *((int *) (&voice->field_50_duration));
                if (best < flags)
                {
                    best = flags;
                }
            }
            i++;
            voice++;
            mask <<= 1;
        }
        while (i < 12);
        voice = (AkaoTrack *) g_AkaoVoiceChannelTable;
        mask = AKAO_SPU_VOICE_SFX_START_MASK;
        i = 0;
        flag1 = 0x100000;
        control = 0x200000;
        field = voice;
        do
        {
            if ((active & mask) != 0)
            {
                if (best == (*((int *) (&field->field_50_duration))))
                {
                    flags = field->flags;
                    value = flags & flag1;
                    if (value != 0)
                    {
                        value = flags | control;
                        field->flags = value;
                    }
                    else
                    {
                        g_SpuPendingKeyOffMask |= mask;
                        SeqOp_DeactivateVoice((char *) voice, mask);
                        field->flags = 0;
                    }
                }
            }
            i++;
            field++;
            voice++;
            mask <<= 1;
        }
        while (i < 12);
        goto finish;
    }
    {
        u32 flag1;
        int control;
        i = 0;
        flag1 = 0x100000;
        control = 0x200000;
        field = voice;
        do
        {
            if ((active & mask) != 0)
            {
                if (field->key_off_mask == id)
                {
                    flags = field->flags;
                    value = flags & flag1;
                    if (value != 0)
                    {
                        value = flags | control;
                        field->flags = value;
                    }
                    else
                    {
                        g_SpuPendingKeyOffMask |= mask;
                        SeqOp_DeactivateVoice((char *) voice, mask);
                        field->flags = 0;
                    }
                }
            }
            i++;
            field++;
            voice++;
            mask <<= 1;
        }
        while (i < 12);
    }
finish:
    g_AkaoVoiceUpdateFlags |= AKAO_VOICE_PARAM_PITCH;

    Seq_MarkTrack34MaskDirty();
    Seq_MarkTrack38MaskDirty();
    Seq_MarkTrack3CMaskDirty();
}
#include "pe1/akao.h"

extern unsigned int D_800BCD50;
extern unsigned int D_800BCD54;
extern unsigned int D_800BCD58;
extern unsigned int D_800BCD5C;
extern unsigned int D_800BCD60;
extern unsigned int D_800BCD6C;
extern unsigned int D_800BCD70;
extern unsigned int D_800BCD74;
extern unsigned int D_8009D2DC;
extern AkaoNestedVoiceSlot D_800BC000[];

void Akao_InitVoiceState(AkaoTrack *track, void *script);

void Seq_StartNestedTrack(AkaoTrack *track, AkaoNestedSource *source, unsigned int voice_mask, void *script) {
    AkaoTrack *track_reg;
    unsigned int mask = voice_mask;
    int pan;
    int pan_target;
    unsigned int *active_mask;
    unsigned int old_active_mask;
    unsigned int old_pending_mask;

    track_reg = track;
    track_reg->key_off_mask = source->key_off_mask;
    track_reg->key_on_mask = source->key_on_mask;
    pan = source->pan << 8;
    track_reg->panpot_slide_duration = 0;
    track_reg->panpot = pan;
    pan_target = source->pan_target;
    track_reg->field_56 = 2;
    track_reg->pan_duration = 1;
    track_reg->parent_track_id = 1;
    track_reg->panpot_duration = 0;
    *(int *)&track_reg->field_50_duration = -2;
    track_reg->pan_target = (pan_target & 0x7F) << 8;

    Akao_InitVoiceState(track_reg, script);

    active_mask = &D_800BCD50;
    old_active_mask = *active_mask;
    old_pending_mask = D_800BCD5C;
    old_active_mask |= mask;
    old_pending_mask |= mask;
    *active_mask = old_active_mask;

    mask = ~mask;
    D_800BCD5C = old_pending_mask;
    D_800BCD54 &= mask;
    D_800BCD58 &= mask;
    D_800BCD6C &= mask;
    D_800BCD70 &= mask;
    D_800BCD74 &= mask;

    if ((D_8009D2DC & 2) != 0) {
        unsigned int busy_mask;
        unsigned int *active_loop_mask;
        int count;

        mask = 0x1000;
        track_reg = (AkaoTrack *)D_800BC000;
        count = 0xC;
        busy_mask = 0x02000000;
        active_loop_mask = active_mask;
        do {
            if ((track_reg->key_on_mask & busy_mask) == 0) {
                unsigned int not_mask;
                unsigned int active_value;
                unsigned int off_value;

                not_mask = ~mask;
                off_value = D_800BCD60;
                active_value = *active_loop_mask;
                active_value &= not_mask;
                off_value |= mask;
                *active_loop_mask = active_value;
                D_800BCD60 = off_value;
            }
            count--;
            track_reg++;
            mask <<= 1;
        } while (count != 0);
    }
}
#include "pe1/akao.h"


extern char *D_8009D2C8;

void Seq_ClearTrackVoiceId(AkaoTrack *track, unsigned int voice_id) {
    unsigned int i;
    int bit;
    int reset;
    char *state;

    if (voice_id < 24) {
        i = 0;
        reset = 24;
        state = D_8009D2C8;
        bit = 1;
        do {
            if (track->assigned_voice_index == voice_id) {
                track->assigned_voice_index = reset;
                *(u32 *)(state + 0x14) &= ~(bit << i);
            }
            i++;
            track++;
        } while (i < 24);
    }
}

void Seq_StartNestedStreams(AkaoNestedSource *source, void *first, void *second)
{
    int remaining;
    unsigned bit;
    AkaoTrack *track;
    int active;
    unsigned mask;

    if (!first && !second) {
        return;
    }
    if (source->key_on_mask) {
        Spu_ManageVoices(0, source->key_on_mask);
    }
    do {
        track = &g_AkaoVoiceChannelTable[11];
        active = g_SpuActiveVoiceMask;
        bit = 0x800000;
        mask = active;
        /* Paired streams need adjacent slots; search from the highest voice. */
        if (first && second) {
            remaining = 11;
            --track;
            bit = 0x400000;
            while (1) {
                if (!(mask & (bit | (bit << 1)))) break;
                --remaining;
                --track;
                bit >>= 1;
                if (!remaining) goto exhausted;
            }
        } else {
            remaining = 12;
            while (1) {
                if (!(mask & bit)) break;
                --remaining;
                --track;
                bit >>= 1;
                if (!remaining) break;
            }
        }
        if (!remaining) {
exhausted:
            Spu_ManageVoices(0, 0x40000000);
            if (active == g_SpuActiveVoiceMask) active = 0x80000000;
        }
    } while (!remaining && active >= 0);
    if (active < 0) {
        return;
    }
    if (first) {
        Seq_StartNestedTrack(track, source, bit, first);
        Seq_ClearTrackVoiceId(g_AkaoVoiceStateTable, track->assigned_voice_index);
    }
    if (second) {
        if (first) {
            ++track;
            bit <<= 1;
        }
        Seq_StartNestedTrack(track, source, bit, second);
        Seq_ClearTrackVoiceId(g_AkaoVoiceStateTable, track->assigned_voice_index);
        if (first) track->flags |= 0x10000;
    }
    g_AkaoVoiceUpdateFlags |= 0x10;
    Seq_MarkTrack34MaskDirty();
    Seq_MarkTrack38MaskDirty();
    Seq_MarkTrack3CMaskDirty();
}
