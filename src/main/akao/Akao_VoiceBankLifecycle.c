/* CC1_FLAGS: -fno-strength-reduce */
/* MASPSX_FLAGS: --expand-div */
#include "common.h"
#include "pe1/akao.h"
#include "pe1/akao/voice_state.h"
#include "pe1/akao/voice_masks.h"
#include "pe1/akao/commands.h"
#include "pe1/akao/tick.h"
#include "pe1/akao/seq_param.h"
#include "m2c_macros.h"

typedef unsigned short u16_1;
extern AkaoTrack D_800BC000_tracks[] asm("D_800BC000");
extern char *D_8009D2C8_bytes asm("D_8009D2C8");


void Akao_SetNotePitch(AkaoTrack *track, int arg);

void Akao_InitVoiceState(AkaoTrack *track, void *script) {
    track->volume_base = 0x6E00;
    track->pc = script;
    track->expression = 0;
    track->detune = 0;
    track->tremolo_duration = 0;
    track->voice_mask_a = 0;
    track->vibrato_delta = 0;
    track->pitch_slide_steps = 0;
    track->fixed_note_length = 0;
    track->default_note_length = 0;
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

#define NULL ((void *)0)
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


void Spu_ManageVoices(int arg0, int arg1);
void SeqOp_DeactivateVoice(char *ptr, int mask);
void Seq_MarkTrack34MaskDirty(void);
void Seq_MarkTrack38MaskDirty(void);
void Seq_MarkTrack3CMaskDirty(void);

void Akao_InitVoices(int arg0, char *arg1) {
    register unsigned int i asm("$6");
    register int mode3 asm("$10");
    register int mode1 asm("$9");
    register char *fallback asm("$8");
    register int mode5 asm("$7");
    AkaoTrack *voice;
    register u16 *field asm("$4");

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
    field = (u16 *)voice + 0x8B;
    do {
        i++;
        field[-0x60] = mode3;
        field[-0x5F] = mode1;
        voice->pc = (AkaoU8 *)fallback;
        *(u32 *)(field - 0x11) |= AKAO_VOICE_PARAM_ADSR_RELEASE;
        *field = mode5;
        voice++;
        field += 0x8E;
    } while (i < 0x18);
}

void Spu_ManageVoices(int arg0, int arg1)
{
    register u32 mask asm("$16");
    register u32 i asm("$17");
    register u32 *field asm("$18");
    register AkaoTrack *voice asm("$19");
    u32 id;
    u32 active;
    register int mode_bits asm("$22");
    int value;
    int flags;
    int best;
    mode_bits = arg1;
    __asm__("" : : "r"(mode_bits));
    mask = AKAO_SPU_VOICE_SFX_START_MASK;
    id = (u16_1) arg0;
    if (id == 0xFFFF)
    {
        return;
    }
    voice = (AkaoTrack *) g_AkaoVoiceChannelTable;
    active = g_SpuActiveVoiceMask;
    if ((mode_bits & 0x0FFFFFFF) != 0)
    {
        u32 flag1;
        u32 id;
        i = 0;
        flag1 = AKAO_TRACK_FLAG_SUSTAIN;
        id = AKAO_TRACK_FLAG_KEY_OFF_PENDING;
        field = (u32 *)voice + 0xE;
        do
        {
            if ((active & mask) != 0)
            {
                if ((field[-3] & mode_bits) != 0)
                {
                    flags = field[0];
                    value = flags & flag1;
                    if (value != 0)
                    {
                        value = flags | id;
                        field[0] = value;
                    }
                    else
                    {
                        g_SpuPendingKeyOffMask |= mask;
                        SeqOp_DeactivateVoice((char *) voice, mask);
                        field[0] = 0;
                    }
                }
            }
            i++;
            field += 0x47;
            voice++;
            mask <<= 1;
        }
        while (i < 12);
        goto finish;
    }
    if (mode_bits < 0)
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
        flag1 = AKAO_TRACK_FLAG_SUSTAIN;
        control = AKAO_TRACK_FLAG_KEY_OFF_PENDING;
        field = (u32 *)voice + 0xE;
        do
        {
            if ((active & mask) != 0)
            {
                if (best == field[6])
                {
                    flags = field[0];
                    value = flags & flag1;
                    if (value != 0)
                    {
                        value = flags | control;
                        field[0] = value;
                    }
                    else
                    {
                        g_SpuPendingKeyOffMask |= mask;
                        SeqOp_DeactivateVoice((char *) voice, mask);
                        field[0] = 0;
                    }
                }
            }
            i++;
            field += 0x47;
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
        flag1 = AKAO_TRACK_FLAG_SUSTAIN;
        control = AKAO_TRACK_FLAG_KEY_OFF_PENDING;
        field = (u32 *)voice + 0xE;
        do
        {
            if ((active & mask) != 0)
            {
                if (field[-4] == id)
                {
                    flags = field[0];
                    value = flags & flag1;
                    if (value != 0)
                    {
                        value = flags | control;
                        field[0] = value;
                    }
                    else
                    {
                        g_SpuPendingKeyOffMask |= mask;
                        SeqOp_DeactivateVoice((char *) voice, mask);
                        field[0] = 0;
                    }
                }
            }
            i++;
            field += 0x47;
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

extern unsigned int D_800BCD50;
extern unsigned int D_800BCD54;
extern unsigned int D_800BCD58;
extern unsigned int D_800BCD5C;
extern unsigned int D_800BCD60;
extern unsigned int D_800BCD6C;
extern unsigned int D_800BCD70;
extern unsigned int D_800BCD74;
extern unsigned int D_8009D2DC;

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
    track_reg->note_length = 2;
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
        track_reg = (AkaoTrack *)D_800BC000_tracks;
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



void Seq_ClearTrackVoiceId(AkaoTrack *track, unsigned int voice_id) {
    unsigned int i;
    int bit;
    int reset;
    char *state;
    int *assigned;

    if (voice_id < 24) {
        i = 0;
        reset = 24;
        state = D_8009D2C8_bytes;
        bit = 1;
        assigned = (int *)track + 0x3C;
        do {
            if (*assigned == voice_id) {
                *assigned = reset;
                *(u32 *)(state + 0x14) &= ~(bit << i);
            }
            i++;
            assigned += 0x47;
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
extern u16 *D_8009D240;
extern char *D_8009D260;

void func_8008AB1C(int *out0, int *out1, int sample_id) {
    u32 index;
    u16 value;
    int result;

    index = sample_id;
    index &= 0x3FF;
    index <<= 1;

    value = D_8009D240[index];
    if (value != 0xFFFF) {
        result = (int)(D_8009D260 + value);
    } else {
        result = 0;
    }
    *out0 = result;

    index++;
    value = D_8009D240[index];
    if (value != 0xFFFF) {
        result = (int)(D_8009D260 + value);
    } else {
        result = 0;
    }
    *out1 = result;
}




void func_8008AB9C(AkaoTrack *track) {
    u32 mask;
    u32 bit;
    unsigned int *flags;

    mask = D_8009D2C8->timing.active_voice_mask;
    if (mask != 0) {
        bit = 1;
        flags = (unsigned int *)track + 0x3D;
        do {
            if (mask & bit) {
                mask ^= bit;
                *flags |= AKAO_VOICE_PARAM_VOLUME;
            }
            flags += 0x47;
            bit <<= 1;
        } while (mask != 0);
    }
}

extern u32 D_800BCD50;


void func_8008ABF0(void) {
    register u32 mask asm("$5");
    register u32 bit asm("$4");
    register AkaoTrack *track asm("$2");
    register unsigned int *flags asm("$3");

    mask = D_800BCD50;
    track = D_800BC000_tracks;
    __asm__ __volatile__("" : : "r"(track));
    if (mask != 0) {
        bit = 0x1000;
        flags = (unsigned int *)track + 0x3D;
        do {
            if (mask & bit) {
                mask ^= bit;
                *flags |= AKAO_VOICE_PARAM_VOLUME;
            }
            flags += 0x47;
            bit <<= 1;
        } while (mask != 0);
    }
}

void Akao_UpdateVoiceMask(int new_base) {
    AkaoTrack *track;
    AkaoTrack *active;
    AkaoSequencerBank *bank;
    register unsigned int bit asm("$7");
    register unsigned int remaining asm("$8");
    unsigned int mask;
    unsigned int voice_mask;
    register unsigned int updated asm("$2");
    int delta;
    int backup_base;
    unsigned int old_status, allocation;
    AkaoSequencerBank *loaded_bank;
    unsigned int key_on_state;
    unsigned int valid_bits;
    register unsigned int pending asm("$3");
    unsigned int restore_mask;
    AkaoSequencerBank *read_bank;
    AkaoSequencerBank *clear_bank;
    AkaoSequencerBank *final_bank;
    register AkaoU16 *duration asm("$5");
    unsigned int magic_flags;
    register unsigned int reset_duration asm("$10");

    Util_CopyWords((unsigned int *)&g_AkaoTrackStateBackup,
                   (unsigned int *)g_AkaoCurTrack, 0x68);
    Util_CopyWords((unsigned int *)&g_AkaoVoiceStateBackup,
                   (unsigned int *)&g_AkaoVoiceStateTable, sizeof(AkaoVoiceBank));

    track = g_AkaoVoiceStateTable;
    remaining = AKAO_VOICE_COUNT;
    bit = 1;
    magic_flags = 0x1ff93;
    reset_duration = 4;
    duration = &track->pan_duration;
    /* Keep the loop constants live before loading the sequencer bank. */
    asm volatile("" : "=r"(remaining), "=r"(bit), "=r"(magic_flags),
                      "=r"(reset_duration), "=r"(duration)
                    : "0"(remaining), "1"(bit), "2"(magic_flags),
                      "3"(reset_duration), "4"(duration));
    loaded_bank = g_AkaoCurTrack;
    key_on_state = g_AkaoVoiceKeyOnState;
    asm("" : "=r"(bank) : "0"(loaded_bank), "r"(key_on_state));
    old_status = bank->status_flags;
    allocation = bank->allocated_voice_mask;
    bank->field_20[3] = new_base;
    key_on_state &= 0x100;
    old_status |= key_on_state;
    bank->status_flags = old_status;
    bank->key_on_request_mask = allocation;
    backup_base = g_AkaoTrackStateBackup.field_20[3];
    asm volatile("" : "=r"(backup_base) : "0"(backup_base));
    delta = new_base - backup_base;
    g_AkaoVoiceUpdateFlags |= 0x90;
    asm volatile("" ::: "memory");
    voice_mask = bank->active_voice_mask;
    do {
        /* The original loop addresses these fields from pan_duration. */
        active = (AkaoTrack *)((char *)duration - 0x58);
        if (voice_mask & bit) {
            track->pc += delta;
            active->branch_target += delta;
            active->call_stack[0] += delta;
            active->call_stack[1] += delta;
            active->call_stack[2] += delta;
            active->call_stack[3] += delta;
            duration[-1] += 2;
            duration[0] += 2;
            active->update_flags |= magic_flags;
            if ((bank->status_flags & 0x100) && duration[1] >= 0x20) {
                duration[1] += 0x30;
            }
        } else {
            register AkaoU8 *default_pc asm("$2");
            register AkaoU16 short_duration asm("$2");
            /* These pins keep the constants inside this branch. */
            duration[-1] = reset_duration;
            short_duration = 2;
            duration[0] = short_duration;
            default_pc = (AkaoU8 *)g_AkaoDefaultVoiceProgram;
            track->pc = default_pc;
        }
        --remaining;
        duration += sizeof(AkaoTrack) / sizeof(AkaoU16);
        track++;
        bit <<= 1;
    } while (remaining != 0);

    read_bank = g_AkaoCurTrack;
    updated = Akao_ForEachVoiceMasked(g_AkaoVoiceStateTable2,
                                      ((AkaoSequencerState *)read_bank)->secondary.active_voice_mask &
                                      ((AkaoSequencerState *)read_bank)->secondary.pending_voice_mask);
    valid_bits = 0xffffff;
    clear_bank = g_AkaoCurTrack;
    updated = ~updated;
    clear_bank->key_off_request_mask = 0;
    asm volatile("" ::: "memory");
    mask = ~g_SpuActiveVoiceMask;
    mask &= valid_bits;
    updated &= mask;
    pending = g_SpuPendingKeyOffMask;
    pending |= updated;
    g_SpuPendingKeyOffMask = pending;
    Seq_MarkTrack34MaskDirty();
    Seq_MarkTrack38MaskDirty();
    Seq_MarkTrack3CMaskDirty();
    g_AkaoSelectedBankId = 0;
    if (g_AkaoSeqPendingFlags & 1) {
        final_bank = g_AkaoCurTrack;
        restore_mask = final_bank->active_voice_mask;
        final_bank->active_voice_mask = 0;
        final_bank->pending_restore_mask = restore_mask;
    }
}


void Akao_UpdateVoiceMask(int arg0);
void Akao_StepSequencerVoice(void *arg0);

void Seq_SelectPlaybackBank(int *arg0) {
    u16 value;

    value = g_AkaoSelectedBankId;
    if ((value != 0) && (value == arg0[3])) {
        Akao_UpdateVoiceMask(arg0[1]);
    } else {
        Akao_StepSequencerVoice((void *)arg0[1]);
        ((AkaoTrack *)g_AkaoCurTrack)->parent_track_id = arg0[3];
    }
}

void Util_CopyWords(unsigned int *src, unsigned int *dst, unsigned int size);

void Seq_RestorePrimaryState(void) {
    u32 i;
    AkaoTrack *track;
    u16 *pitch;
    u32 flags;
    u16 value;

    if (g_AkaoCurTrack->active_voice_mask != 0) {
        Util_CopyWords((u32 *)g_AkaoCurTrack, (u32 *)&g_AkaoTrackStateBackup, sizeof(AkaoSequencerBank));
        Util_CopyWords((u32 *)g_AkaoVoiceStateTable, (u32 *)&g_AkaoVoiceStateBackup, sizeof(AkaoVoiceBank));

        flags = g_AkaoTrackStateBackup.status_flags;
        if ((flags & 0x100) != 0) {
            i = 0;
            track = g_AkaoVoiceStateBackup.tracks;
            pitch = &track->note_pitch;
            do {
                value = *pitch;
                if (value >= 0x50) {
                    *pitch = value - 0x30;
                }
                i++;
                pitch += sizeof(AkaoTrack) / sizeof(u16);
            } while (i < 24);
        }
    }
}


void Util_CopyWords(unsigned int *src, unsigned int *dst, unsigned int size);
void Akao_StepSequencerVoice(void *arg0);

extern int g_AkaoSeqLoopCounter;
void Seq_SelectPlaybackBank(int *arg0);


void Seq_RestoreSecondaryStateAndSelect(int *arg0) {
    AkaoSequencerBank *state;

    state = (AkaoSequencerBank *)g_AkaoCurTrack;
    if ((state->active_voice_mask != 0) && ((state + 1)->active_voice_mask == 0)) {
        Util_CopyWords((u32 *)state, (u32 *)(state + 1), sizeof(*state));
        Util_CopyWords((u32 *)g_AkaoVoiceStateTable,
                       (u32 *)&g_AkaoVoiceStateTable[AKAO_VOICE_COUNT], sizeof(AkaoVoiceBank));
    }

    Akao_StepSequencerVoice((void *)arg0[1]);
    ((AkaoTrack *)g_AkaoCurTrack)->parent_track_id = arg0[3];
}

void Seq_SelectPlaybackBankWithCountdown(int *arg0)
{
  int raw;
  register int value;
  Seq_SelectPlaybackBank(arg0);
  raw = arg0[4] & 0xFFFFFFFFFFFFFFFFu;
  value = 0;
  if (raw != 0)
  {
    value = raw - 1;
  }
  g_AkaoSeqLoopCounter = value;
}

void Seq_StartDefaultNestedStream(int *arg0) {
    int old1 = arg0[1];
    int old2 = arg0[2];

    arg0[1] = 0x400;
    arg0[2] = 0x1000000;
    arg0[3] = 0x80;
    arg0[4] = 0x7F;
    Seq_StartNestedStreams((AkaoNestedSource *)arg0, (void *)old1, (void *)old2);
}
void Akao_LoadSamplePairFromIndex(int *arg0, int *arg1, int arg2);

void Seq_StartIndexedNestedStreamWithDefaults(int *arg0) {
    int old1;
    int old2;

    Akao_LoadSamplePairFromIndex(&old1, &old2, arg0[1]);
    arg0[2] = 0x2000000;
    arg0[3] = 0x80;
    arg0[4] = 0x7F;
    Seq_StartNestedStreams((AkaoNestedSource *)arg0, (void *)old1, (void *)old2);
}

void Seq_StartIndexedNestedStream(int *arg0) {
    int old1;
    int old2;

    Akao_LoadSamplePairFromIndex(&old1, &old2, arg0[1]);
    Seq_StartNestedStreams((AkaoNestedSource *)arg0, (void *)old1, (void *)old2);
}


#define g_AkaoCurTrackBytes ((char *)g_AkaoCurTrack)
#define g_AkaoVoiceStateTableBytes ((char *)g_AkaoVoiceStateTable)
#define g_AkaoVoiceStateTable2Bytes ((char *)g_AkaoVoiceStateTable2)



void Seq_StartNestedStreams(AkaoNestedSource *arg0, void *arg1, void *arg2);

void Spu_ManageVoices(int arg0, int arg1);




void Seq_MarkDirtyTracks(char *arg0);

extern short g_AkaoGlobalPitchSlideCounter;
extern int D_8009D2B4;

void Seq_ApplyGlobalPitch(void);

void Seq_StartRelativeNestedStream(void *arg0) {
    char *base;
    u16 offset;
    void *arg1;
    void *arg2;

    base = *(char **)((char *)arg0 + 4);
    offset = *(u16 *)base;
    if (offset != 0xFFFF) {
        arg1 = (char *)(offset + (int)base) + 4;
    } else {
        arg1 = 0;
    }

    offset = *(u16 *)(base + 2);
    arg2 = 0;
    if (offset != 0xFFFF) {
        arg2 = (char *)(offset + (int)*(char **)((char *)arg0 + 4)) + 4;
    }

    *(void **)((char *)arg0 + 4) = *(void **)((char *)arg0 + 0x14);
    Seq_StartNestedStreams(arg0, arg1, arg2);
}

void Spu_ManageVoicesCmd(int *arg0) {
    Spu_ManageVoices(arg0[1], arg0[2]);
}

void Seq_SetTrackPitchImmediate(int *arg0) {
    int *msg;
    int selector;
    char *track;
    char *primary_track;
    char *next_track;
    char *base;

    msg = arg0;
    selector = msg[4];
    if (selector == 0 || selector == *(u16_1 *)(g_AkaoCurTrackBytes + 0x54)) {
        int value;
        base = g_AkaoVoiceStateTableBytes;
        value = msg[1];
        primary_track = g_AkaoCurTrackBytes;
        value &= 0x7F;
        value <<= 16;
        *(int *)(primary_track + 0x48) = value;
        *(u16_1 *)(primary_track + 0x50) = 0;
        Seq_MarkDirtyTracks(base);
    } else if (selector != 0) {
        track = g_AkaoCurTrackBytes;
        if (selector == *(u16_1 *)(track + 0xBC)) {
            int value;
            next_track = track + 0x68;
            base = g_AkaoVoiceStateTable2Bytes;
            value = msg[1];
            g_AkaoCurTrack = (AkaoSequencerBank *)next_track;
            *(u16_1 *)(track + 0xB8) = 0;
            value &= 0x7F;
            value <<= 16;
            *(int *)(track + 0xB0) = value;
            Seq_MarkDirtyTracks(base);
            g_AkaoCurTrack--;
        }
    }
}

void Seq_SlideTrackPitch(int *arg0) {
    register int raw asm("$2");
    int duration;
    register int target asm("$5");
    int selector;
    char *track;
    char *base;
    char *next_track;

    raw = arg0[1];
    duration = 1;
    if (raw != 0) {
        duration = raw;
    }

    raw = arg0[2];
    selector = arg0[4];
    raw &= 0x7F;
    target = raw << 16;

    if (selector != 0) {
        track = g_AkaoCurTrackBytes;
        if (selector != *(u16 *)(track + 0x54)) {
            goto secondary_track;
        }
    }

    {
        track = g_AkaoCurTrackBytes;
        target = (target - *(int *)(track + 0x48)) / duration;
        base = g_AkaoVoiceStateTableBytes;
        ((AkaoTrack *)track)->field_50_duration = duration;
        ((AkaoTrack *)track)->pitch_slide_step = target;
        Seq_MarkDirtyTracks(base);
    }
    goto done;

secondary_track:
    if (arg0 != 0) {
        if (selector == *(u16 *)(track + 0xBC)) {
            target = (target - *(int *)(track + 0xB0)) / duration;
            base = g_AkaoVoiceStateTable2Bytes;
            *(u16 *)(track + 0xB8) = duration;
            next_track = track + 0x68;
            g_AkaoCurTrack = (AkaoSequencerBank *)next_track;
            *(int *)(track + 0xB4) = target;
            Seq_MarkDirtyTracks(base);
            g_AkaoCurTrack--;
        }
    }

done:
    return;
}

void Seq_TrackPitchSetup(int *arg0) {
    register int raw asm("$2");
    int duration;
    int selector;
    char *track;
    int start;
    register int delta asm("$6");
    char *base;
    char *next_track;

    raw = arg0[1];
    duration = 1;
    if (raw != 0) {
        duration = raw;
    }

    selector = arg0[4];
    if (selector != 0) {
        track = g_AkaoCurTrackBytes;
        if (selector != *(u16 *)(track + 0x54)) {
            goto secondary_track;
        }
    }

    {
        start = arg0[2];
        track = g_AkaoCurTrackBytes;
        start &= 0x7F;
        start <<= 16;
        *(int *)(track + 0x48) = start;
        raw = arg0[3];
        raw &= 0x7F;
        delta = raw << 16;
        delta -= start;
        delta = delta / duration;
        base = g_AkaoVoiceStateTableBytes;
        ((AkaoTrack *)track)->field_50_duration = duration;
        ((AkaoTrack *)track)->pitch_slide_step = delta;
        Seq_MarkDirtyTracks(base);
    }
    goto done;

secondary_track:
    if (selector != 0) {
        if (selector == *(u16 *)(track + 0xBC)) {
            start = arg0[2];
            start &= 0x7F;
            start <<= 16;
            *(int *)(track + 0xB0) = start;
            raw = arg0[3];
            raw &= 0x7F;
            delta = raw << 16;
            delta -= start;
            delta = delta / duration;
            base = g_AkaoVoiceStateTable2Bytes;
            *(u16 *)(track + 0xB8) = duration;
            next_track = track + 0x68;
            g_AkaoCurTrack = (AkaoSequencerBank *)next_track;
            *(int *)(track + 0xB4) = delta;
            Seq_MarkDirtyTracks(base);
            g_AkaoCurTrack--;
        }
    }

done:
    return;
}

void Seq_SetGlobalPitchImmediate(void *arg0) {
    int value;

    value = *(u16 *)((char *)arg0 + 4);
    g_AkaoGlobalPitchSlideCounter = 0;
    D_8009D2B4 = value << 16;
    Seq_ApplyGlobalPitch();
}

extern short g_AkaoGlobalPitchSlideCounter;
extern int g_AkaoGlobalPitchSlideStep;
extern int D_8009D2B4;


extern u32 g_SpuActiveVoiceMask;

void Seq_SlideGlobalPitchToTarget(void *arg0) {
    int duration;
    int raw_duration;
    int value;

    raw_duration = *(int *)((char *)arg0 + 4);
    duration = 1;
    if (raw_duration != 0) {
        duration = raw_duration;
    }

    value = *(u16 *)((char *)arg0 + 8) << 16;
    g_AkaoGlobalPitchSlideCounter = duration;
    g_AkaoGlobalPitchSlideStep = (value - D_8009D2B4) / duration;
}

void Seq_SlideGlobalPitchFromStartToTarget(void *arg0) {
    int duration;
    int raw_duration;
    int start;
    int target;

    raw_duration = *(int *)((char *)arg0 + 4);
    duration = 1;
    if (raw_duration != 0) {
        duration = raw_duration;
    }

    target = *(u16 *)((char *)arg0 + 0xC) << 16;
    start = *(u16 *)((char *)arg0 + 8) << 16;
    g_AkaoGlobalPitchSlideCounter = duration;
    D_8009D2B4 = start;
    g_AkaoGlobalPitchSlideStep = (target - start) / duration;
}

void Spu_SetVoiceVolumeImmediateMasked(int *arg0) {
    u32 i;
    u32 mask;
    u32 active;
    register char *voice;
    register char *base;
    int value;

    base = (char *)g_AkaoVoiceChannelTable;
    active = g_SpuActiveVoiceMask;
    mask = AKAO_SPU_VOICE_SFX_START_MASK;
    if (arg0[2] != 0) {
        i = 0;
        voice = base + 0xF4;
        do {
            if ((active & mask) != 0) {
                if ((*(u32 *)(voice - 0xC8) & arg0[2]) != 0) {
                    value = (arg0[3] & 0x7F) << 8;
                    *(u16_1 *)(voice - 0x80) = 0;
                    *(u16_1 *)(voice - 0x1C) = value;
                    *(u32 *)voice |= AKAO_VOICE_PARAM_VOLUME;
                }
            }
            i++;
            voice += sizeof(AkaoTrack);
            mask <<= 1;
        } while (i < 12);
    } else {
        i = 0;
        voice = base + 0xF4;
        do {
            if ((active & mask) != 0) {
                if (*(int *)(voice - 0xCC) == arg0[1]) {
                    value = (arg0[3] & 0x7F) << 8;
                    *(u16_1 *)(voice - 0x80) = 0;
                    *(u16_1 *)(voice - 0x1C) = value;
                    *(u32 *)voice |= AKAO_VOICE_PARAM_VOLUME;
                }
            }
            i++;
            voice += sizeof(AkaoTrack);
            mask <<= 1;
        } while (i < 12);
    }
}

extern u32 g_SpuActiveVoiceMask;

static inline int ParameterDelta(int target, int current, int step) {
    return (short)(((target & 0x7F) << 8) - current) / (short)step;
}

void Spu_ParameterSlide(int *arg0) {
    char *base;
    u32 active;
    u32 mask;
    u32 i;
    char *voice;
    int step;
    int delta;

    base = (char *)g_AkaoVoiceChannelTable;
    active = g_SpuActiveVoiceMask;
    mask = AKAO_SPU_VOICE_SFX_START_MASK;

    if (arg0[2] != 0) {
        i = 0;
        voice = base + 0x74;
        do {
            if ((active & mask) != 0) {
                if ((*(u32 *)(voice - 0x48) & arg0[2]) != 0) {
                    step = 1;
                    if (arg0[3] != 0) {
                        step = arg0[3];
                    }
                    delta = ParameterDelta(arg0[4], *(u16 *)(voice + 0x64), step);
                    *(u16 *)(voice + 0x66) = delta;
                    *(u16 *)voice = step;
                }
            }
            i++;
            voice += sizeof(AkaoTrack);
            mask <<= 1;
        } while (i < 12);
    } else {
        i = 0;
        voice = base + 0x74;
        do {
            if ((active & mask) != 0) {
                if (*(int *)(voice - 0x4C) == arg0[1]) {
                    step = 1;
                    if (arg0[3] != 0) {
                        step = arg0[3];
                    }
                    delta = ParameterDelta(arg0[4], *(u16 *)(voice + 0x64), step);
                    *(u16 *)(voice + 0x66) = delta;
                    *(u16 *)voice = step;
                }
            }
            i++;
            voice += sizeof(AkaoTrack);
            mask <<= 1;
        } while (i < 12);
    }
}

extern char g_SpuVoiceControlTable[];
extern u32 g_SpuActiveVoiceMask;

void Spu_SetAllVoiceVolumeImmediate(int *arg0) {
    u32 mask;
    u32 active;
    u32 i;
    u32 block_flag;
    register char *voice;
    int value;
    int dirty;

    mask = AKAO_SPU_VOICE_SFX_START_MASK;
    active = g_SpuActiveVoiceMask;
    i = 0;
    block_flag = 0x02000000;
    voice = g_SpuVoiceControlTable;

    do {
        if ((active & mask) != 0) {
            if ((*(u32 *)(voice - 0xC8) & block_flag) == 0) {
                value = arg0[1];
                dirty = *(volatile u32 *)voice;
                *(u16 *)(voice - 0x80) = 0;
                value &= 0x7F;
                value <<= 8;
                dirty |= AKAO_VOICE_PARAM_VOLUME;
                *(u16 *)(voice - 0x1C) = value;
                *(u32 *)voice = dirty;
            }
        }
        i++;
        voice += sizeof(AkaoTrack);
        mask <<= 1;
    } while (i < 12);
}

extern char D_800BC074[];
extern u32 g_SpuActiveVoiceMask;

void Spu_SlideAllVoiceVolume(int *arg0) {
    u32 mask;
    u32 active;
    u32 block_flag;
    u32 i;
    register char *voice;
    int step;
    int delta;

    mask = AKAO_SPU_VOICE_SFX_START_MASK;
    active = g_SpuActiveVoiceMask;
    i = 0;
    block_flag = 0x02000000;
    voice = D_800BC074;

    do {
        if ((active & mask) != 0) {
            if ((*(u32 *)(voice - 0x48) & block_flag) == 0) {
                step = 1;
                if (arg0[1] != 0) {
                    step = arg0[1];
                }
                delta = (short)(((arg0[2] & 0x7F) << 8) - *(u16 *)(voice + 0x64));
                *(u16 *)voice = step;
                *(u16 *)(voice + 0x66) = delta / (short)step;
            }
        }
        i++;
        voice += sizeof(AkaoTrack);
        mask <<= 1;
    } while (i < 12);
}

void Spu_SetVoicePanImmediateMasked(int *arg0) {
    char *base;
    u32 active;
    u32 mask;
    u32 i;
    char *voice;
    int value;
    int dirty;

    base = (char *)g_AkaoVoiceChannelTable;
    active = g_SpuActiveVoiceMask;
    mask = AKAO_SPU_VOICE_SFX_START_MASK;

    if (arg0[2] != 0) {
        i = 0;
        voice = base + 0xF4;
        do {
            if ((active & mask) != 0) {
                if ((*(u32 *)(voice - 0xC8) & arg0[2]) != 0) {
                    value = ((u8 *)arg0)[0xC];
                    dirty = *(u32 *)voice;
                    *(u16 *)(voice - 0x7C) = 0;
                    value <<= 8;
                    dirty |= AKAO_VOICE_PARAM_VOLUME;
                    *(u16 *)(voice - 0x7E) = value;
                    *(u32 *)voice = dirty;
                }
            }
            i++;
            voice += sizeof(AkaoTrack);
            mask <<= 1;
        } while (i < 12);
    } else {
        i = 0;
        voice = base + 0xF4;
        do {
            if ((active & mask) != 0) {
                if (*(int *)(voice - 0xCC) == arg0[1]) {
                    value = ((u8 *)arg0)[0xC];
                    dirty = *(u32 *)voice;
                    *(u16 *)(voice - 0x7C) = 0;
                    value <<= 8;
                    dirty |= AKAO_VOICE_PARAM_VOLUME;
                    *(u16 *)(voice - 0x7E) = value;
                    *(u32 *)voice = dirty;
                }
            }
            i++;
            voice += sizeof(AkaoTrack);
            mask <<= 1;
        } while (i < 12);
    }
}

static inline int PanDelta(int target, int current, int step) {
    return (short)((target << 8) - current) / (short)step;
}

void Spu_SlideVoicePanMasked(int *arg0) {
    char *base;
    u32 active;
    u32 mask;
    u32 i;
    char *voice;
    int step;
    int delta;

    base = (char *)g_AkaoVoiceChannelTable;
    active = g_SpuActiveVoiceMask;
    mask = AKAO_SPU_VOICE_SFX_START_MASK;

    if (arg0[2] != 0) {
        i = 0;
        voice = base + 0x78;
        do {
            if ((active & mask) != 0) {
                if ((*(u32 *)(voice - 0x4C) & arg0[2]) != 0) {
                    step = 1;
                    if (arg0[3] != 0) {
                        step = arg0[3];
                    }
                    delta = PanDelta(((u8 *)arg0)[0x10], *(u16 *)(voice - 2), step);
                    *(u16 *)(voice + 0x64) = delta;
                    *(u16 *)voice = step;
                }
            }
            i++;
            voice += sizeof(AkaoTrack);
            mask <<= 1;
        } while (i < 12);
    } else {
        i = 0;
        voice = base + 0x78;
        do {
            if ((active & mask) != 0) {
                if (*(int *)(voice - 0x50) == arg0[1]) {
                    step = 1;
                    if (arg0[3] != 0) {
                        step = arg0[3];
                    }
                    delta = PanDelta(((u8 *)arg0)[0x10], *(u16 *)(voice - 2), step);
                    *(u16 *)(voice + 0x64) = delta;
                    *(u16 *)voice = step;
                }
            }
            i++;
            voice += sizeof(AkaoTrack);
            mask <<= 1;
        } while (i < 12);
    }
}

extern char g_SpuVoiceControlTable[];
extern u32 g_SpuActiveVoiceMask;

void Spu_SetAllVoicePanImmediate(int *arg0) {
    u32 mask;
    u32 active;
    u32 i;
    u32 block_flag;
    register char *voice;
    int value;
    int dirty;

    mask = AKAO_SPU_VOICE_SFX_START_MASK;
    active = g_SpuActiveVoiceMask;
    i = 0;
    block_flag = 0x02000000;
    voice = g_SpuVoiceControlTable;

    do {
        if ((active & mask) != 0) {
            if ((*(u32 *)(voice - 0xC8) & block_flag) == 0) {
                value = ((u8 *)arg0)[4];
                dirty = *(volatile u32 *)voice;
                *(u16 *)(voice - 0x7C) = 0;
                value <<= 8;
                dirty |= AKAO_VOICE_PARAM_VOLUME;
                *(u16 *)(voice - 0x7E) = value;
                *(u32 *)voice = dirty;
            }
        }
        i++;
        voice += sizeof(AkaoTrack);
        mask <<= 1;
    } while (i < 12);
}

extern char D_800BC078[];
extern u32 g_SpuActiveVoiceMask;

void Spu_SlideAllVoicePan(int *arg0) {
    u32 mask;
    u32 active;
    u32 i;
    u32 block_flag;
    char *voice;
    int step;
    int delta;

    mask = AKAO_SPU_VOICE_SFX_START_MASK;
    active = g_SpuActiveVoiceMask;
    i = 0;
    block_flag = 0x02000000;
    voice = D_800BC078;

    do {
        if ((active & mask) != 0) {
            if ((*(u32 *)(voice - 0x4C) & block_flag) == 0) {
                step = 1;
                if (arg0[1] != 0) {
                    step = arg0[1];
                }
                delta = PanDelta(((u8 *)arg0)[8], *(u16 *)(voice - 2), step);
                *(u16 *)(voice + 0x64) = delta;
                *(u16 *)voice = step;
            }
        }
        i++;
        voice += sizeof(AkaoTrack);
        mask <<= 1;
    } while (i < 12);
}

extern u32 g_SpuActiveVoiceMask;

void Spu_SetVoicePitchImmediateMasked(int *arg0) {
    register char *base;
    u32 active;
    u32 mask;
    u32 i;
    register char *voice;
    int value;
    int dirty;

    base = (char *)g_AkaoVoiceChannelTable;
    active = g_SpuActiveVoiceMask;
    mask = AKAO_SPU_VOICE_SFX_START_MASK;

    if (arg0[2] != 0) {
        i = 0;
        voice = base + 0xF4;
        do {
            if ((active & mask) != 0) {
                if ((*(u32 *)(voice - 0xC8) & arg0[2]) != 0) {
                    value = ((u8 *)arg0)[0xC];
                    dirty = *(volatile u32 *)voice;
                    *(u16 *)(voice - 0x84) = 0;
                    value <<= 8;
                    dirty |= AKAO_VOICE_PARAM_PITCH;
                    *(u32 *)(voice - 0xB8) = value;
                    *(u32 *)voice = dirty;
                }
            }
            i++;
            voice += sizeof(AkaoTrack);
            mask <<= 1;
        } while (i < 12);
    } else {
        i = 0;
        voice = base + 0xF4;
        do {
            if ((active & mask) != 0) {
                if (*(int *)(voice - 0xCC) == arg0[1]) {
                    value = ((u8 *)arg0)[0xC];
                    dirty = *(volatile u32 *)voice;
                    *(u16 *)(voice - 0x84) = 0;
                    value <<= 8;
                    dirty |= AKAO_VOICE_PARAM_PITCH;
                    *(u32 *)(voice - 0xB8) = value;
                    *(u32 *)voice = dirty;
                }
            }
            i++;
            voice += sizeof(AkaoTrack);
            mask <<= 1;
        } while (i < 12);
    }
}

extern u32 g_SpuActiveVoiceMask;

static inline short PitchDelta(int target, int current, int step) {
    return (short)((target << 8) - current) / (short)step;
}

void Spu_SlideVoicePitchMasked(int *arg0) {
    char *base;
    u32 active;
    u32 mask;
    u32 i;
    char *voice;
    int step;
    int delta;

    base = (char *)g_AkaoVoiceChannelTable;
    active = g_SpuActiveVoiceMask;
    mask = AKAO_SPU_VOICE_SFX_START_MASK;

    if (arg0[2] != 0) {
        i = 0;
        voice = base + 0x70;
        do {
            if ((active & mask) != 0) {
                if ((*(u32 *)(voice - 0x44) & arg0[2]) != 0) {
                    step = 1;
                    if (arg0[3] != 0) {
                        step = arg0[3];
                    }
                    delta = PitchDelta(((u8 *)arg0)[0x10], *(int *)(voice - 0x34), step);
                    *(short *)voice = step;
                    *(int *)(voice - 0x30) = (short)delta;
                }
            }
            i++;
            voice += sizeof(AkaoTrack);
            mask <<= 1;
        } while (i < 12);
    } else {
        i = 0;
        voice = base + 0x70;
        do {
            if ((active & mask) != 0) {
                if (*(int *)(voice - 0x48) == arg0[1]) {
                    step = 1;
                    if (arg0[3] != 0) {
                        step = arg0[3];
                    }
                    delta = PitchDelta(((u8 *)arg0)[0x10], *(int *)(voice - 0x34), step);
                    *(short *)voice = step;
                    *(int *)(voice - 0x30) = (short)delta;
                }
            }
            i++;
            voice += sizeof(AkaoTrack);
            mask <<= 1;
        } while (i < 12);
    }
}

extern char g_SpuVoiceControlTable[];
extern u32 g_SpuActiveVoiceMask;

void Spu_SetAllVoicePitchImmediate(int *arg0) {
    u32 mask;
    u32 active;
    u32 i;
    u32 block_flag;
    register char *voice;
    int value;
    int dirty;

    mask = AKAO_SPU_VOICE_SFX_START_MASK;
    active = g_SpuActiveVoiceMask;
    i = 0;
    block_flag = 0x02000000;
    voice = g_SpuVoiceControlTable;

    do {
        if ((active & mask) != 0) {
            if ((*(u32 *)(voice - 0xC8) & block_flag) == 0) {
                value = ((u8 *)arg0)[4];
                dirty = *(volatile u32 *)voice;
                *(u16 *)(voice - 0x84) = 0;
                value <<= 8;
                dirty |= AKAO_VOICE_PARAM_PITCH;
                *(u32 *)(voice - 0xB8) = value;
                *(u32 *)voice = dirty;
            }
        }
        i++;
        voice += sizeof(AkaoTrack);
        mask <<= 1;
    } while (i < 12);
}

extern char D_800BC070[];
extern u32 g_SpuActiveVoiceMask;

extern short g_AkaoGlobalD2D0SlideCounter;
extern int D_8009D2D0;

void Spu_SlideAllVoicePitch(int *arg0) {
    u32 mask;
    u32 active;
    u32 i;
    u32 block_flag;
    register char *voice;
    int step;
    int delta;

    mask = AKAO_SPU_VOICE_SFX_START_MASK;
    active = g_SpuActiveVoiceMask;
    i = 0;
    block_flag = 0x02000000;
    voice = D_800BC070;

    do {
        if ((active & mask) != 0) {
            if ((*(u32 *)(voice - 0x44) & block_flag) == 0) {
                step = 1;
                if (arg0[1] != 0) {
                    step = arg0[1];
                }
                delta = (short)((((u8 *)arg0)[8] << 8) - *(int *)(voice - 0x34));
                *(short *)voice = step;
                *(int *)(voice - 0x30) = (short)(delta / (short)step);
            }
        }
        i++;
        voice += sizeof(AkaoTrack);
        mask <<= 1;
    } while (i < 12);
}

void Akao_SetGlobalD2D0Immediate(AkaoGlobalParamCommand *cmd) {
    int value = cmd->value;

    g_AkaoGlobalD2D0SlideCounter = 0;
    D_8009D2D0 = value << 16;
}


extern short g_AkaoGlobalD2D0SlideCounter;
extern int g_AkaoGlobalD2D0SlideStep;
extern int D_8009D2D0;

extern short g_AkaoGlobalD2CCSlideCounter;
extern int D_8009D2CC;

void Akao_SlideGlobalD2D0ToTarget(AkaoGlobalSlideCommand *cmd) {
    int raw_duration = cmd->duration;
    int duration = 1;
    int step;

    if (raw_duration != 0) {
        duration = raw_duration;
    }

    step = ((cmd->target << 16) - D_8009D2D0) / duration;
    g_AkaoGlobalD2D0SlideCounter = duration;
    g_AkaoGlobalD2D0SlideStep = step;
}

void Akao_SlideGlobalD2D0FromStartToTarget(AkaoGlobalSlideRangeCommand *cmd) {
    int duration;
    int start = cmd->start;
    int step;

    if (start == 0) {
        duration = 1;
    } else {
        duration = cmd->duration;
    }

    start <<= 24;
    start >>= 8;
    D_8009D2D0 = start;
    step = ((cmd->target << 16) - start) / duration;
    g_AkaoGlobalD2D0SlideCounter = duration;
    g_AkaoGlobalD2D0SlideStep = step;
}

void Akao_SetGlobalD2CCImmediate(AkaoGlobalParamCommand *cmd) {
    int value = cmd->value;

    g_AkaoGlobalD2CCSlideCounter = 0;
    D_8009D2CC = value << 16;
}

extern short g_AkaoGlobalD2CCSlideCounter;
extern int g_AkaoGlobalD2CCSlideStep;
extern int D_8009D2CC;

void Akao_InitVoices(int arg0, char *arg1);

extern unsigned int g_SpuActiveVoiceMask;
extern unsigned int g_SpuPendingKeyOffMask;
extern unsigned int g_AkaoVoiceUpdateFlags;

void SeqOp_DeactivateVoice(char *ptr, int mask);
void Seq_MarkTrack34MaskDirty(void);
void Seq_MarkTrack38MaskDirty(void);
void Seq_MarkTrack3CMaskDirty(void);

void Akao_SlideGlobalD2CCToTarget(AkaoGlobalSlideCommand *cmd) {
    int raw_duration = cmd->duration;
    int duration = 1;
    int step;

    if (raw_duration != 0) {
        duration = raw_duration;
    }

    step = ((cmd->target << 16) - D_8009D2CC) / duration;
    g_AkaoGlobalD2CCSlideCounter = duration;
    g_AkaoGlobalD2CCSlideStep = step;
}

void Akao_SlideGlobalD2CCFromStartToTarget(AkaoGlobalSlideRangeCommand *cmd) {
    int duration;
    int start = cmd->start;
    int step;

    if (start == 0) {
        duration = 1;
    } else {
        duration = cmd->duration;
    }

    start <<= 24;
    start >>= 8;
    D_8009D2CC = start;
    step = ((cmd->target << 16) - start) / duration;
    g_AkaoGlobalD2CCSlideCounter = duration;
    g_AkaoGlobalD2CCSlideStep = step;
}

void Akao_InitPrimarySecondaryVoices(void) {
    AkaoTrack *base = g_AkaoVoiceStateTable;

    Akao_InitVoices(0, (char *)&base[0]);
    g_AkaoCurTrack++;
    Akao_InitVoices(0, (char *)&base[AKAO_VOICE_COUNT]);
    g_AkaoCurTrack--;
}

void Akao_InitPrimarySecondaryVoicesWithMode(AkaoValueCommand *cmd) {
    AkaoTrack *base = g_AkaoVoiceStateTable;
    int value = cmd->field_4;

    Akao_InitVoices(value, (char *)&base[0]);
    if (cmd->field_4 != 0) {
        g_AkaoCurTrack++;
        Akao_InitVoices(cmd->field_4, (char *)&base[AKAO_VOICE_COUNT]);
        g_AkaoCurTrack--;
    }
}

void Spu_StopActiveVoices(void) {
    char *voice = g_AkaoVoiceChannelTable;
    unsigned int mask = AKAO_SPU_VOICE_SFX_START_MASK;
    unsigned int i = 0;
    unsigned int skip_flag = 0x02000000;
    char *field_38 = voice + 0x38;

    do {
        if (g_SpuActiveVoiceMask & mask) {
            if ((*(int *)(field_38 - 0xC) & skip_flag) == 0) {
                g_SpuPendingKeyOffMask |= mask;
                SeqOp_DeactivateVoice(voice, mask);
                *(int *)field_38 = 0;
            }
        }
        i++;
        field_38 += sizeof(AkaoTrack);
        voice += sizeof(AkaoTrack);
        mask <<= 1;
    } while (i < 0xC);

    g_AkaoVoiceUpdateFlags |= AKAO_VOICE_PARAM_PITCH;
    Seq_MarkTrack34MaskDirty();
    Seq_MarkTrack38MaskDirty();
    Seq_MarkTrack3CMaskDirty();
}

extern int g_AkaoPlaybackMode;
extern AkaoSequencerBank *g_AkaoCurTrack;

void Seq_MarkDirtyTracks(char *arg0);
void Spu_MarkActiveVoicesDirty(void);

extern AkaoTrackUpdateSlot g_AkaoTrackStateArray[];

void Seq_SetPlaybackMode1AndRefreshVoices(void) {
    AkaoTrack *base = g_AkaoVoiceStateTable;

    g_AkaoPlaybackMode = 1;
    Seq_MarkDirtyTracks((char *)base);
    g_AkaoCurTrack++;
    Seq_MarkDirtyTracks((char *)&base[AKAO_VOICE_COUNT]);
    g_AkaoCurTrack--;
    Spu_MarkActiveVoicesDirty();
}

void Seq_SetPlaybackMode4AndRefreshVoices(void) {
    AkaoTrack *base = g_AkaoVoiceStateTable;

    g_AkaoPlaybackMode = 4;
    Seq_MarkDirtyTracks((char *)base);
    g_AkaoCurTrack++;
    Seq_MarkDirtyTracks((char *)&base[AKAO_VOICE_COUNT]);
    g_AkaoCurTrack--;
    Spu_MarkActiveVoicesDirty();
}

void Seq_SetPlaybackMode2AndRefreshVoices(void) {
    AkaoTrack *base = g_AkaoVoiceStateTable;

    g_AkaoPlaybackMode = 2;
    Seq_MarkDirtyTracks((char *)base);
    g_AkaoCurTrack++;
    Seq_MarkDirtyTracks((char *)&base[AKAO_VOICE_COUNT]);
    g_AkaoCurTrack--;
    Spu_MarkActiveVoicesDirty();
}

void Seq_SetGlobalD2B8AndDirtyAllTracks(AkaoValueCommand *cmd) {
    unsigned int i = 0;
    int value = cmd->field_4;
    AkaoTrackUpdateSlot *slot;

    slot = g_AkaoTrackStateArray;
    g_AkaoVoicePortamentoResetMask = value;
    for (; i < 0x18; i++, slot++) {
        slot->update_flags |= AKAO_VOICE_PARAM_VOLUME;
    }
}

void Seq_SetCurrentTrackField56(AkaoValueCommand *arg0) {
    ((AkaoTrack *)g_AkaoCurTrack)->note_length = arg0->field_4;
}
extern unsigned int g_SpuActiveVoiceMask;
extern unsigned int g_AkaoSeqPendingFlags;

void AkaoSpuVoice_SetVolume(unsigned int index, unsigned int left, unsigned int right);
void AkaoSpuVoice_SetPitch(unsigned int index, unsigned int value);
void AkaoSpuVoice_SetAdsrAttack(unsigned int index, unsigned int left, unsigned int right);
void AkaoSpuVoice_SetAdsrSustainRate(unsigned int index, unsigned int left, unsigned int right);

void Seq_DeactivatePendingTracks(void) {
    unsigned int pending;
    unsigned int mask;
    unsigned int bit;
    unsigned int index;

    if (g_AkaoCurTrack->active_voice_mask != 0) {
        mask = ~g_SpuActiveVoiceMask & 0xFFFFFF;
        if (mask != 0) {
            bit = 1;
            index = 0;
            do {
                if (mask & bit) {
                    AkaoSpuVoice_SetVolume(index, 0, 0);
                    AkaoSpuVoice_SetPitch(index, 0);
                    AkaoSpuVoice_SetAdsrAttack(index, 0x7F, 1);
                    AkaoSpuVoice_SetAdsrSustainRate(index, 0x7F, 3);
                    mask &= ~bit;
                }
                bit <<= 1;
                index++;
            } while (mask != 0);
        }

        pending = g_AkaoCurTrack->active_voice_mask;
        g_AkaoCurTrack->active_voice_mask = 0;
        g_AkaoCurTrack->pending_restore_mask = pending;
    }

    g_AkaoSeqPendingFlags |= 1;
}

extern unsigned int g_AkaoSeqPendingFlags;
extern AkaoTrackUpdateSlot g_AkaoTrackStateArray[];

void Seq_MarkTrack34MaskDirty(void);
void Seq_MarkTrack38MaskDirty(void);
void Seq_MarkTrack3CMaskDirty(void);

extern u32 g_SpuActiveVoiceMask;
extern u32 g_SpuStoppedVoiceMask;
void AkaoSpuVoice_SetVolume(unsigned int index, unsigned int left, unsigned int right);
void AkaoSpuVoice_SetPitch(unsigned int index, unsigned int value);
void AkaoSpuVoice_SetAdsrAttack(unsigned int index, unsigned int left, unsigned int right);
void AkaoSpuVoice_SetAdsrSustainRate(unsigned int index, unsigned int left, unsigned int right);

void Seq_RestorePendingTracks(void) {
    unsigned int pending = g_AkaoCurTrack->pending_restore_mask;
    unsigned int saved;
    unsigned int mask;
    AkaoTrackUpdateSlot *slot;

    if (pending != 0) {
        mask = 1;
        slot = g_AkaoTrackStateArray;
        do {
            if (pending & mask) {
                pending &= ~mask;
                slot->update_flags |= AKAO_VOICE_PARAM_RESUME;
            }
            mask <<= 1;
            slot++;
        } while (pending != 0);

        saved = g_AkaoCurTrack->pending_restore_mask;
        g_AkaoCurTrack->pending_restore_mask = 0;
        g_AkaoCurTrack->active_voice_mask = saved;
        Seq_MarkTrack34MaskDirty();
        Seq_MarkTrack38MaskDirty();
        Seq_MarkTrack3CMaskDirty();
    }

    g_AkaoSeqPendingFlags &= ~1;
}

void Spu_VoiceStopAll(void)
{
  u32 pending;
  u32 bit;
  u32 voice_index;
  register u32 *active_ptr;
  u32 active;
  u32 new_var;
  u32 inverse;
  char *voice;
  u32 i;
  pending = g_SpuActiveVoiceMask;
  if (pending != 0)
  {
    voice = g_AkaoVoiceChannelTable;
    bit = AKAO_SPU_VOICE_SFX_START_MASK;
    i = 0;
    do
    {
      if ((pending & bit) != 0)
      {
        if (((*((u32 *) (voice + 0x2C))) & 0x02000000) != 0)
        {
          pending &= ~bit;
        }
      }
      i++;
      voice += sizeof(AkaoTrack);
      bit <<= 1;
    }
    while (i < 12);
    bit = AKAO_SPU_VOICE_SFX_START_MASK;
    voice_index = AKAO_SPU_VOICE_SFX_START_INDEX;
    active_ptr = &g_SpuActiveVoiceMask;
    g_SpuStoppedVoiceMask = pending;
    new_var = *active_ptr;
    active = new_var;
    inverse = ~pending;
    active &= inverse;
    *active_ptr = active;
    if (pending != 0)
    {
      do
      {
        if ((pending & bit) != 0)
        {
          AkaoSpuVoice_SetVolume(voice_index, 0, 0);
          AkaoSpuVoice_SetPitch(voice_index, 0);
          AkaoSpuVoice_SetAdsrAttack(voice_index, 0x7F, 1);
          AkaoSpuVoice_SetAdsrSustainRate(voice_index, 0x7F, 3);
          pending &= ~bit;
        }
        bit <<= 1;
        voice_index++;
      }
      while (pending != 0);
    }
  }
  g_AkaoSeqPendingFlags |= 2;
}
extern unsigned int g_SpuActiveVoiceMask;
extern unsigned int g_SpuStoppedVoiceMask;
extern unsigned int g_AkaoSeqPendingFlags;
void Seq_MarkTrack34MaskDirty(void);
void Seq_MarkTrack38MaskDirty(void);
void Seq_MarkTrack3CMaskDirty(void);


extern AkaoQueueEntry D_800B8628[];
extern int g_AkaoMessageQueueCount;

void Seq_GetGlobalPitch(unsigned int *out);

void Spu_RestoreStoppedVoices(void) {
    unsigned int pending = g_SpuStoppedVoiceMask;
    unsigned int saved;
    unsigned int mask;
    int *slot;

    if (pending != 0) {
        mask = AKAO_SPU_VOICE_SFX_START_MASK;
        slot = (int *)g_SpuVoiceControlTable;
        do {
            if (pending & mask) {
                pending &= ~mask;
                *slot |= AKAO_VOICE_PARAM_RESUME;
            }
            mask <<= 1;
            slot = (int *)((char *)slot + sizeof(AkaoTrack));
        } while (pending != 0);

        saved = g_SpuStoppedVoiceMask;
        g_SpuStoppedVoiceMask = 0;
        g_SpuActiveVoiceMask = saved;
        Seq_MarkTrack34MaskDirty();
        Seq_MarkTrack38MaskDirty();
        Seq_MarkTrack3CMaskDirty();
    }

    g_AkaoSeqPendingFlags &= ~2;
}

void Akao_MessageNoop(AkaoQueueEntry *entry) {
}

void Akao_ProcessMessageQueue(void) {
    AkaoQueueEntry *entry = D_800B8628;

    if (g_AkaoMessageQueueCount != 0) {
        do {
            Akao_MessageHandlers[entry->opcode.id](entry);
            g_AkaoMessageQueueCount--;
            entry++;
        } while (g_AkaoMessageQueueCount != 0);
    }
}

void Akao_AllocMessageSlot(AkaoQueueEntry **out_msg) {
    *out_msg = D_800B8628;
    *out_msg = &D_800B8628[g_AkaoMessageQueueCount];
    g_AkaoMessageQueueCount++;
}

void Seq_SetParamWithReset(unsigned int arg0) {
    unsigned int value;

    Seq_GetGlobalPitch(&value);
    if (value != arg0) {
        SpuSetReverb(0);
        SPU_StepDmaRead(arg0 | 0x100);
        SpuSetReverb(1);
    }
}

int Akao_EnqueueStagedCommand(void) {
    AkaoQueueEntry *msg;
    unsigned short *data;
    unsigned int opcode;
    int *staged;
    int result;
    int param;
    AkaoSequenceCounter sequence;

    result = 0;
    D_8009D268 = 1;
    opcode = g_AkaoCmd.opcode;
    switch (opcode) {
    case 0x10:
    case 0x12:
    case 0x19:
        staged = &g_AkaoCmd.opcode;
        data = g_AkaoCmd.arg0.sample_header;
        if (Spu_ValidateSampleHeader(data) != 0) {
            result = -1;
            break;
        }
        data += 2;
        result = *data;
        data += 2;
        param = *data;
        data += 4;
        if (D_8009D2C8->timing.bank_id != result) {
            Seq_SetParamWithReset(param);
            Akao_AllocMessageSlot(&msg);
            msg->arg0.sample_data = data;
            msg->arg2 = result;
            if (*staged == 0x12)
                msg->arg3 = g_AkaoCmd.arg1;
            msg->opcode.word = *staged;
        } else {
            result = 0;
        }
        break;
    case 0x24:
        Akao_AllocMessageSlot(&msg);
        msg->arg0.value = g_AkaoCmd.arg0.value;
        msg->arg1 = g_AkaoCmd.arg1;
        sequence = D_8009CDF0;
        msg->arg2 = g_AkaoCmd.arg2;
        D_8009CDF0.value = ((sequence.value + 1) & 0x1FF) + 0x400;
        msg->arg3 = g_AkaoCmd.arg3;
        result = sequence.value;
        msg->sequence = result;
        msg->opcode.word = opcode;
        break;
    case 0xD8:
        Akao_AllocMessageSlot(&msg);
        msg->arg0.value = g_AkaoCmd.arg0.value;
        msg->opcode.word = 0xD0;
        Akao_AllocMessageSlot(&msg);
        msg->arg0.value = g_AkaoCmd.arg0.value;
        msg->opcode.word = 0xD4;
        break;
    case 0xD9:
        Akao_AllocMessageSlot(&msg);
        msg->arg0.value = g_AkaoCmd.arg0.value;
        msg->arg1 = g_AkaoCmd.arg1;
        msg->opcode.word = 0xD1;
        Akao_AllocMessageSlot(&msg);
        msg->arg0.value = g_AkaoCmd.arg0.value;
        msg->arg1 = g_AkaoCmd.arg1;
        msg->opcode.word = 0xD5;
        break;
    case 0xDA:
        Akao_AllocMessageSlot(&msg);
        msg->arg0.value = g_AkaoCmd.arg0.value;
        msg->arg1 = g_AkaoCmd.arg1;
        msg->arg2 = g_AkaoCmd.arg2;
        msg->opcode.word = 0xD2;
        Akao_AllocMessageSlot(&msg);
        msg->arg0.value = g_AkaoCmd.arg0.value;
        msg->arg1 = g_AkaoCmd.arg1;
        msg->arg2 = g_AkaoCmd.arg2;
        msg->opcode.word = 0xD6;
        break;
    case 0x99:
        Akao_AllocMessageSlot(&msg);
        msg->opcode.word = 0x9B;
        Akao_AllocMessageSlot(&msg);
        msg->opcode.word = 0x9D;
        break;
    case 0x98:
        Akao_AllocMessageSlot(&msg);
        msg->opcode.word = 0x9A;
        Akao_AllocMessageSlot(&msg);
        msg->opcode.word = 0x9C;
        break;
    default:
        Akao_AllocMessageSlot(&msg);
        msg->arg0.value = g_AkaoCmd.arg0.value;
        msg->arg1 = g_AkaoCmd.arg1;
        msg->arg2 = g_AkaoCmd.arg2;
        msg->arg3 = g_AkaoCmd.arg3;
        msg->opcode.word = g_AkaoCmd.opcode;
        break;
    }
    D_8009D268 = 0;
    return result;
}
