/* CC1_FLAGS: -fno-strength-reduce */

#include "common.h"
#include "pe1/akao.h"
#include "pe1/akao/commands.h"
#include "pe1/akao/globals.h"
#include "pe1/akao/queue.h"
#include "pe1/akao/seq_param.h"
#include "pe1/akao/tick.h"
#include "pe1/akao/voice_masks.h"
#include "pe1/akao/voice_state.h"
#include "pe1/akao_counters.h"
#include "m2c_macros.h"

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

void Akao_StepSequencerVoice(void *arg0) {
    void *arg0v;
    u8 *walk;
    s32 mask;
    s32 bit;
    u8 *pvoice;
    s32 kFFF5;
    u8 *base;
    s32 kFFFa;
    s32 tret;
    s32 tnor;
    s32 tcd;
    s32 tand;
    s32 t2dc;
    s32 tw1;
    s32 tw2;
    s32 kFFFb;
    s32 km102;
    s32 tlhu;
    void *p2c8a;
    s32 tld;
    AkaoSequencerBank *p2c8b;
    AkaoSequencerBank *p2c8c;
    AkaoSequencerBank *pend;
    register s32 all_voices;

    arg0v = arg0;
    walk = arg0v;
    kFFFa = 0xFFFFFF;
    p2c8a = g_AkaoCurTrack;
    ((AkaoSequencerBank *)p2c8a)->sequence_base = arg0v;
    tld = *(s32 *)arg0v; /* sequence header word 0: channel mask */
    mask = tld & kFFFa;
    tret = Akao_ForEachVoiceMasked(g_AkaoVoiceStateTable2, ((AkaoSequencerState *)p2c8a)->secondary.active_voice_mask);
    tnor = ~tret;
    tcd = ~g_SpuActiveVoiceMask & kFFFa;
    tand = tnor & tcd;
    p2c8b = g_AkaoCurTrack;
    t2dc = g_AkaoSeqPendingFlags;
    g_SpuPendingKeyOffMask |= tand;
    *(s32 *)&p2c8b->key_off_request_mask = 0;
    if (t2dc & 1) {
        *(s32 *)&p2c8b->active_voice_mask = 0;
        *(s32 *)&p2c8b->pending_restore_mask = (*(s32 *)&p2c8b->pending_restore_mask | mask);
    } else {
        *(s32 *)&p2c8b->pending_restore_mask = 0;
        *(s32 *)&p2c8b->active_voice_mask = (*(s32 *)&p2c8b->active_voice_mask | mask);
    }
    walk += 4;
        kFFFb = 0xFFFFFF;
    tw1 = *(s32 *)walk;
    walk += 4;
        bit = 1;
    pvoice = (u8 *)g_AkaoVoiceStateTable;
    km102 = -0x102;
        p2c8c = g_AkaoCurTrack;
    p2c8c->pending_voice_mask = tw1 & kFFFb;
    tw2 = *(s32 *)walk;
    walk += 8;
    p2c8c->yield_voice_mask = tw2 & kFFFb;
    kFFF5 = 0xFFFFFF;
    base = pvoice + 0x116;
    p2c8c->status_flags &= km102;
    p2c8c->status_flags |= g_AkaoVoiceKeyOnState & 0x100;
    do {
        AkaoTrack *voice = (AkaoTrack *)(base - 0x116);
        if (mask & bit) {
            tlhu = *(u16 *)walk;
            walk += 2;
            *(u8 **)pvoice = walk + tlhu;
            voice->assigned_voice_index = 0x18;
            voice->note_length = 4;
            voice->pan_duration = 2;
            voice->volume_base = 0x7F00;
            voice->expression_value = 0x3FFF0000;
            voice->pan_target = 0x4000;
            voice->detune = 0;
            voice->expression = 0;
            voice->tremolo_duration = 0;
            voice->voice_mask_a = 0;
            voice->vibrato_delta = 0;
            voice->pitch_slide_steps = 0;
            voice->fixed_note_length = 0;
            voice->default_note_length = 0;
            voice->panpot = 0x8000;
            voice->panpot_slide_duration = 0;
            voice->tremolo_duration = 0;
            voice->panpot_duration = 0;
            voice->expression_duration = 0;
            voice->branch_target = arg0v;
            voice->tremolo_phase = 0;
            voice->aux_lfo_value = 0;
            voice->flags = 0;
            voice->call_stack_index = 0;
            voice->aux_lfo_target = 0;
            voice->volume_lfo_target = 0;
            voice->pitch_lfo_target = 0;
            voice->aux_lfo_slide_duration = 0;
            voice->volume_lfo_slide_duration = 0;
            voice->pitch_lfo_slide_duration = 0;
            voice->key_off_delay = 0;
            voice->key_on_delay = 0;
            Akao_SetNotePitch(pvoice, 0);
        } else {
            voice->note_length = 3;
            voice->pan_duration = 1;
            *(void **)pvoice = g_AkaoDefaultVoiceProgram;
            voice->adsr_release_rate = 5;
            voice->update_flags = (voice->update_flags | 0x4400);
        }
        kFFF5 &= ~bit;
        mask &= ~bit;
        base += 0x11C;
        pvoice += 0x11C;
        bit *= 2;
    } while (kFFF5 != 0);
        pend = g_AkaoCurTrack;
    pend->tempo = 0xFFFF0000;
    pend->tick_accumulator = 1;
    pend->tempo_slide_duration = 0;
    pend->volume = 0;
    pend->volume_slide_duration = 0;
    *(s32 *)&pend->volume_delta = 0;
    g_AkaoVoiceUpdateFlags = 0;
    pend->tick = 0;
    pend->ticks_per_beat = 0;
    pend->beat = 0;
    pend->measure = 0;
    pend->key_off_dirty_mask = 0;
    pend->volume_dirty_mask = 0;
    pend->adsr_dirty_mask = 0;
    pend->condition_value = 0;
    pend->key_on_request_mask = 0;
    all_voices = 0xFFFFFF;
    pend->allocated_voice_mask = all_voices;
    pend->reserved_voice_mask = 0;
    Seq_MarkTrack34MaskDirty();
    Seq_MarkTrack38MaskDirty();
    Seq_MarkTrack3CMaskDirty();
}

void Akao_InitVoices(int arg0, char *arg1) {
    unsigned int i;
    int mode3;
    int mode1;
    char *fallback;
    int mode5;
    AkaoTrack *voice;
    u16 *field;
    {
    if (arg0 == 0) {
        if ((g_AkaoCurTrack)->active_voice_mask != 0) {
            goto init_voices_continue;
        }
        return;
    } else if (arg0 != (g_AkaoCurTrack)->bank_id) {
        return;
    }
    }
init_voices_continue:

    (g_AkaoCurTrack)->key_off_request_mask = 0xFFFFFF;

    i = 0;
    mode3 = 3;
    mode1 = 1;
    fallback = g_AkaoDefaultVoiceProgram;
    mode5 = 5;
    voice = (AkaoTrack *)arg1;
    field = &voice->adsr_release_rate;
    do {
        AkaoTrack *parameters = AKAO_TRACK_OF(field, adsr_release_rate);
        i++;
        parameters->note_length = mode3;
        parameters->pan_duration = mode1;
        voice->pc = fallback;
        parameters->update_flags |= AKAO_VOICE_PARAM_ADSR_RELEASE;
        parameters->adsr_release_rate = mode5;
        voice++;
        field += sizeof(AkaoTrack) / sizeof(*field);
    } while (i < 0x18);
}

void Spu_ManageVoices(int arg0, int mode_bits)
{
    u32 mask;
    u32 i;
    AkaoTrack *voice;
    u32 id;
    u32 active;
    int value;
    int flags;
    int best;
    mask = AKAO_SPU_VOICE_SFX_START_MASK;
    id = (u16) arg0;
    if (id == 0xFFFF)
    {
        return;
    }
    voice = g_AkaoVoiceChannelTable;
    active = g_SpuActiveVoiceMask;
    if ((mode_bits & 0x0FFFFFFF) != 0)
    {
        u32 flag1;
        u32 *field;
        u32 id;
        i = 0;
        flag1 = AKAO_TRACK_FLAG_SUSTAIN;
        id = AKAO_TRACK_FLAG_KEY_OFF_PENDING;
        field = &voice->flags;
        do
        {
            AkaoTrack *parameters = AKAO_TRACK_OF(field, flags);
            if ((active & mask) != 0)
            {
                if ((parameters->key_on_mask & mode_bits) != 0)
                {
                    flags = parameters->flags;
                    value = flags & flag1;
                    if (value != 0)
                    {
                        value = flags | id;
                        parameters->flags = value;
                    }
                    else
                    {
                        g_SpuPendingKeyOffMask |= mask;
                        SeqOp_DeactivateVoice(voice, mask);
                        parameters->flags = 0;
                    }
                }
            }
            i++;
            field += sizeof(AkaoTrack) / sizeof(*field);
            voice++;
            mask <<= 1;
        }
        while (i < 12);
    }
    else if (mode_bits < 0)
    {
        voice = (g_AkaoVoiceChannelTable) + id;
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
    else
    {
    value = mode_bits & 0x40000000;
    if (value != 0)
    {
        u32 flag1;
        u32 *field;
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
        voice = g_AkaoVoiceChannelTable;
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
        voice = g_AkaoVoiceChannelTable;
        mask = AKAO_SPU_VOICE_SFX_START_MASK;
        i = 0;
        flag1 = AKAO_TRACK_FLAG_SUSTAIN;
        control = AKAO_TRACK_FLAG_KEY_OFF_PENDING;
        field = &voice->flags;
        do
        {
            AkaoTrack *parameters = AKAO_TRACK_OF(field, flags);
            if ((active & mask) != 0)
            {
                if (best == *(u32 *)&parameters->field_50_duration)
                {
                    flags = parameters->flags;
                    value = flags & flag1;
                    if (value != 0)
                    {
                        value = flags | control;
                        parameters->flags = value;
                    }
                    else
                    {
                        g_SpuPendingKeyOffMask |= mask;
                        SeqOp_DeactivateVoice(voice, mask);
                        parameters->flags = 0;
                    }
                }
            }
            i++;
            field += sizeof(AkaoTrack) / sizeof(*field);
            voice++;
            mask <<= 1;
        }
        while (i < 12);
    }
    else
    {
        u32 flag1;
        u32 *field;
        int control;
        i = 0;
        flag1 = AKAO_TRACK_FLAG_SUSTAIN;
        control = AKAO_TRACK_FLAG_KEY_OFF_PENDING;
        field = &voice->flags;
        do
        {
            AkaoTrack *parameters = AKAO_TRACK_OF(field, flags);
            if ((active & mask) != 0)
            {
                if (parameters->key_off_mask == id)
                {
                    flags = parameters->flags;
                    value = flags & flag1;
                    if (value != 0)
                    {
                        value = flags | control;
                        parameters->flags = value;
                    }
                    else
                    {
                        g_SpuPendingKeyOffMask |= mask;
                        SeqOp_DeactivateVoice(voice, mask);
                        parameters->flags = 0;
                    }
                }
            }
            i++;
            field += sizeof(AkaoTrack) / sizeof(*field);
            voice++;
            mask <<= 1;
        }
        while (i < 12);
    }
    }

    g_AkaoVoiceUpdateFlags |= AKAO_VOICE_PARAM_PITCH;

    Seq_MarkTrack34MaskDirty();
    Seq_MarkTrack38MaskDirty();
    Seq_MarkTrack3CMaskDirty();
}

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
    pan = source->pan.value << 8;
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

    active_mask = &g_SpuActiveVoiceMask;
    old_active_mask = *active_mask;
    old_pending_mask = g_SpuPendingKeyOffMask;
    old_active_mask |= mask;
    old_pending_mask |= mask;
    *active_mask = old_active_mask;

    mask = ~mask;
    g_SpuPendingKeyOffMask = old_pending_mask;
    g_AkaoVoiceMaskScratch &= mask;
    g_AkaoSfxAllocatedVoiceMask &= mask;
    g_AkaoTrack34Mask &= mask;
    g_AkaoTrack38Mask &= mask;
    g_AkaoTrack3CMask &= mask;

    if ((g_AkaoSeqPendingFlags & 2) != 0) {
        unsigned int busy_mask;
        unsigned int *active_loop_mask;
        int count;

        mask = 0x1000;
        track_reg = g_AkaoVoiceChannelTable;
        count = 0xC;
        busy_mask = 0x02000000;
        active_loop_mask = active_mask;
        do {
            if ((track_reg->key_on_mask & busy_mask) == 0) {
                unsigned int not_mask;
                unsigned int active_value;
                unsigned int off_value;

                not_mask = ~mask;
                off_value = g_SpuStoppedVoiceMask;
                active_value = *active_loop_mask;
                active_value &= not_mask;
                off_value |= mask;
                *active_loop_mask = active_value;
                g_SpuStoppedVoiceMask = off_value;
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
    AkaoTickBank *state;
    int *assigned;

    if (voice_id < 24) {
        i = 0;
        reset = 24;
        state = D_8009CD88;
        bit = 1;
        assigned = (int *)track + 0x3C;
        do {
            if (*assigned == voice_id) {
                *assigned = reset;
                state->timing.allocated_voice_mask &= ~(bit << i);
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

void Akao_LoadSamplePairFromIndex(int *out0, int *out1, int sample_id) {
    u32 index;
    u16 value;
    int result;

    index = sample_id;
    index &= 0x3FF;
    index <<= 1;

    value = g_AkaoSampleOffsetTable[index];
    if (value != 0xFFFF) {
        result = (int)(g_AkaoSampleDataBase + value);
    } else {
        result = 0;
    }
    *out0 = result;

    index++;
    value = g_AkaoSampleOffsetTable[index];
    if (value != 0xFFFF) {
        result = (int)(g_AkaoSampleDataBase + value);
    } else {
        result = 0;
    }
    *out1 = result;
}

void Seq_MarkDirtyTracks(AkaoTrack *track) {
    u32 mask;
    u32 bit;
    unsigned int *flags;

    mask = D_8009CD88->timing.active_voice_mask;
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

void Spu_MarkActiveVoicesDirty(void) {
    u32 mask;
    u32 bit;
    AkaoTrack *track;
    unsigned int *flags;
    mask = g_SpuActiveVoiceMask;
    track = g_AkaoVoiceChannelTable;
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

void Akao_UpdateVoiceMask(AkaoU8 *new_base) {
    AkaoTrack *track;
    AkaoTrack *active;
    AkaoSequencerBank *bank;
    unsigned int bit;
    unsigned int remaining;
    unsigned int mask;
    unsigned int voice_mask;
    unsigned int updated;
    int delta;
    AkaoU8 *backup_base;
    unsigned int old_status, allocation;
    AkaoSequencerBank *loaded_bank;
    unsigned int key_on_state;
    unsigned int valid_bits;
    unsigned int restore_mask;
    AkaoSequencerBank *read_bank;
    AkaoSequencerBank *clear_bank;
    AkaoSequencerBank *final_bank;
    AkaoU16 *duration;
    unsigned int magic_flags;
    register unsigned int reset_duration asm("$10");

    Akao_CopyWords((unsigned int *)&g_AkaoTrackStateBackup,
                   (unsigned int *)g_AkaoCurTrack, 0x68);
    Akao_CopyWords((unsigned int *)&g_AkaoVoiceStateBackup,
                   (unsigned int *)&g_AkaoVoiceStateTable, sizeof(AkaoVoiceBank));

    track = g_AkaoVoiceStateTable;
    remaining = AKAO_VOICE_COUNT;
    bit = 1;
    magic_flags = 0x1ff93;
    reset_duration = 4;
    duration = &track->pan_duration;
    /* Keep the reset value live before loading the sequencer bank. */
    asm volatile("" : "=r"(reset_duration) : "0"(reset_duration));
    loaded_bank = g_AkaoCurTrack;
    key_on_state = g_AkaoVoiceKeyOnState;
    asm("" : "=r"(bank) : "0"(loaded_bank));
    old_status = bank->status_flags;
    allocation = bank->allocated_voice_mask;
    bank->sequence_base = new_base;
    key_on_state &= 0x100;
    old_status |= key_on_state;
    bank->status_flags = old_status;
    bank->key_on_request_mask = allocation;
    backup_base = g_AkaoTrackStateBackup.sequence_base;
    asm volatile("" : "=r"(backup_base) : "0"(backup_base));
    delta = new_base - backup_base;
    g_AkaoVoiceUpdateFlags |= 0x90;
    asm volatile("" ::: "memory");
    voice_mask = bank->active_voice_mask;
    do {
        /* The original loop addresses these fields from pan_duration. */
        active = AKAO_TRACK_OF(duration, pan_duration);
        if (voice_mask & bit) {
            track->pc += delta;
            active->branch_target += delta;
            active->call_stack[0] += delta;
            active->call_stack[1] += delta;
            active->call_stack[2] += delta;
            active->call_stack[3] += delta;
            active->note_length += 2;
            active->pan_duration += 2;
            active->update_flags |= magic_flags;
            if ((bank->status_flags & 0x100) && active->note_pitch >= 0x20) {
                active->note_pitch += 0x30;
            }
        } else {
            register AkaoU8 *default_pc asm("$2");
            register AkaoU16 short_duration asm("$2");
            /* These pins keep the constants inside this branch. */
            active->note_length = reset_duration;
            short_duration = 2;
            active->pan_duration = short_duration;
            default_pc = g_AkaoDefaultVoiceProgram;
            track->pc = default_pc;
        }
        --remaining;
        duration += sizeof(AkaoTrack) / sizeof(AkaoU16);
        track++;
        bit <<= 1;
    } while (remaining != 0);

    read_bank = g_AkaoCurTrack;
    updated = ~Akao_ForEachVoiceMasked(g_AkaoVoiceStateTable2,
                                       ((AkaoSequencerState *)read_bank)->secondary.active_voice_mask &
                                       ((AkaoSequencerState *)read_bank)->secondary.pending_voice_mask);
    valid_bits = 0xffffff;
    clear_bank = g_AkaoCurTrack;
    *(unsigned int *)&clear_bank->key_off_request_mask = 0;
    mask = ~g_SpuActiveVoiceMask;
    mask &= valid_bits;
    g_SpuPendingKeyOffMask |= updated & mask;
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

void Akao_UpdateVoiceMask(AkaoU8 *new_base);
void Akao_StepSequencerVoice(void *arg0);

void Seq_SelectPlaybackBank(AkaoQueueEntry *entry) {
    u16 value;

    value = g_AkaoSelectedBankId;
    if ((value != 0) && (value == entry->arg2)) {
        Akao_UpdateVoiceMask(entry->arg0.sequence);
    } else {
        Akao_StepSequencerVoice(entry->arg0.sequence);
        ((AkaoTrack *)g_AkaoCurTrack)->parent_track_id = entry->arg2;
    }
}

void Seq_RestorePrimaryState(void) {
    u32 i;
    AkaoTrack *track;
    u16 *pitch;
    u32 flags;
    u16 value;

    if (g_AkaoCurTrack->active_voice_mask != 0) {
        Akao_CopyWords((u32 *)g_AkaoCurTrack, (u32 *)&g_AkaoTrackStateBackup, sizeof(AkaoSequencerBank));
        Akao_CopyWords((u32 *)g_AkaoVoiceStateTable, (u32 *)&g_AkaoVoiceStateBackup, sizeof(AkaoVoiceBank));

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

void Seq_SelectPlaybackBank(AkaoQueueEntry *entry);

void Seq_RestoreSecondaryStateAndSelect(AkaoQueueEntry *entry) {
    AkaoSequencerBank *state;

    state = g_AkaoCurTrack;
    if ((state->active_voice_mask != 0) && ((state + 1)->active_voice_mask == 0)) {
        Akao_CopyWords((u32 *)state, (u32 *)(state + 1), sizeof(*state));
        Akao_CopyWords((u32 *)g_AkaoVoiceStateTable,
                       (u32 *)&g_AkaoVoiceStateTable[AKAO_VOICE_COUNT], sizeof(AkaoVoiceBank));
    }

    Akao_StepSequencerVoice(entry->arg0.sequence);
    ((AkaoTrack *)g_AkaoCurTrack)->parent_track_id = entry->arg2;
}

void Seq_SelectPlaybackBankWithCountdown(AkaoQueueEntry *entry)
{
  int raw;
  register int value;
  Seq_SelectPlaybackBank(entry);
  raw = entry->arg3 & 0xFFFFFFFFFFFFFFFFu;
  value = 0;
  if (raw != 0)
  {
    value = raw - 1;
  }
  g_AkaoSeqLoopCounter = value;
}

void Seq_StartDefaultNestedStream(AkaoNestedSource *source) {
    int old1 = source->key_off_mask;
    int old2 = source->key_on_mask;

    source->key_off_mask = 0x400;
    source->key_on_mask = 0x1000000;
    /* Retail initializes the pan byte and its padding as one word. */
    source->pan.word = 0x80;
    source->pan_target = 0x7F;
    Seq_StartNestedStreams(source, (void *)old1, (void *)old2);
}

void Seq_StartIndexedNestedStreamWithDefaults(AkaoNestedSource *source) {
    /* Keep the word view for retail store scheduling in this wrapper. */
    int *arg0 = (int *)source;
    int old1;
    int old2;

    Akao_LoadSamplePairFromIndex(&old1, &old2, arg0[1]);
    arg0[2] = 0x2000000;
    arg0[3] = 0x80;
    arg0[4] = 0x7F;
    Seq_StartNestedStreams(source, (void *)old1, (void *)old2);
}

void Seq_StartIndexedNestedStream(AkaoNestedSource *source) {
    int old1;
    int old2;

    Akao_LoadSamplePairFromIndex(&old1, &old2, source->key_off_mask);
    Seq_StartNestedStreams(source, (void *)old1, (void *)old2);
}



void Seq_ApplyGlobalPitch(void);

void Seq_StartRelativeNestedStream(AkaoRelativeNestedCommand *cmd) {
    AkaoU32 table;
    u16 offset;
    void *arg1;
    void *arg2;

    /* The command's key-off word carries the stream table address here;
     * each table entry is a byte offset from the table to a stream. */
    table = cmd->source.key_off_mask;
    offset = ((AkaoU16 *)table)[0];
    if (offset != 0xFFFF) {
        arg1 = (AkaoU8 *)(offset + table) + 4;
    } else {
        arg1 = 0;
    }

    offset = ((AkaoU16 *)table)[1];
    arg2 = 0;
    if (offset != 0xFFFF) {
        arg2 = (AkaoU8 *)(offset + cmd->source.key_off_mask) + 4;
    }

    cmd->source.key_off_mask = cmd->key_off_mask;
    Seq_StartNestedStreams(&cmd->source, arg1, arg2);
}

void Spu_ManageVoicesCmd(int *arg0) {
    Spu_ManageVoices(arg0[1], arg0[2]);
}

void Seq_SetTrackPitchImmediate(AkaoBankPitchCommand *arg0) {
    AkaoBankPitchCommand *msg;
    int selector;
    AkaoSequencerBank *track;
    AkaoSequencerBank *primary_track;
    AkaoSequencerBank *next_track;
    AkaoTrack *base;

    msg = arg0;
    selector = msg->bank_selector;
    if (selector == 0 || selector == g_AkaoCurTrack->bank_id) {
        int value;
        base = g_AkaoVoiceStateTable;
        value = msg->pitch;
        primary_track = g_AkaoCurTrack;
        value &= 0x7F;
        value <<= 16;
        primary_track->pitch_current = value;
        primary_track->pitch_slide_duration = 0;
        Seq_MarkDirtyTracks(base);
    } else if (selector != 0) {
        track = g_AkaoCurTrack;
        if (selector == ((AkaoSequencerState *)track)->secondary.bank_id) {
            int value;
            next_track = track + 1;
            base = g_AkaoVoiceStateTable2;
            value = msg->pitch;
            g_AkaoCurTrack = next_track;
            ((AkaoSequencerState *)track)->secondary.pitch_slide_duration = 0;
            value &= 0x7F;
            value <<= 16;
            ((AkaoSequencerState *)track)->secondary.pitch_current = value;
            Seq_MarkDirtyTracks(base);
            g_AkaoCurTrack--;
        }
    }
}

void Seq_SlideTrackPitch(AkaoBankPitchSlideCommand *arg0) {
    int duration;
    int target;
    int selector;
    AkaoSequencerBank *track;

    duration = 1;
    if (arg0->duration != 0) {
        duration = arg0->duration;
    }

    target = (arg0->target & 0x7F) << 16;
    selector = arg0->bank_selector;

    if (selector == 0 || selector == g_AkaoCurTrack->bank_id) {
        track = g_AkaoCurTrack;
        target -= track->pitch_current;
        target /= duration;
        track->pitch_slide_duration = duration;
        track->pitch_delta = target;
        Seq_MarkDirtyTracks(g_AkaoVoiceStateTable);
    } else if (arg0 != 0) {
        track = g_AkaoCurTrack;
        if (selector == ((AkaoSequencerState *)track)->secondary.bank_id) {
            target -= ((AkaoSequencerState *)track)->secondary.pitch_current;
            target /= duration;
            ((AkaoSequencerState *)track)->secondary.pitch_slide_duration = duration;
            g_AkaoCurTrack = track + 1;
            ((AkaoSequencerState *)track)->secondary.pitch_delta = target;
            Seq_MarkDirtyTracks(g_AkaoVoiceStateTable2);
            g_AkaoCurTrack--;
        }
    }
}

void Seq_TrackPitchSetup(AkaoBankPitchRangeCommand *arg0) {
    int raw;
    /* Shared scratch for a target word or the secondary-bank step. */
    int value;
    int duration;
    int selector;
    AkaoSequencerBank *track;
    int start;
    register int delta asm("$6");
    AkaoSequencerState *state;
    AkaoTrack *base;
    AkaoSequencerBank *next_track;

    raw = arg0->duration;
    duration = 1;
    if (raw != 0) {
        duration = raw;
    }

    selector = arg0->bank_selector;
    if (selector == 0 || ((track = g_AkaoCurTrack), selector == track->bank_id)) {
        start = arg0->start;
        track = g_AkaoCurTrack;
        start &= 0x7F;
        start <<= 16;
        track->pitch_current = start;
        value = arg0->target;
        raw = value;
        raw &= 0x7F;
        delta = raw << 16;
        delta -= start;
        base = g_AkaoVoiceStateTable;
        track->pitch_slide_duration = duration;
        track->pitch_delta = delta / duration;
        Seq_MarkDirtyTracks(base);
    } else {
        if (selector != 0) {
            if (selector == ((AkaoSequencerState *)track)->secondary.bank_id) {
                start = arg0->start;
                start &= 0x7F;
                start <<= 16;
                ((AkaoSequencerState *)track)->secondary.pitch_current = start;
                raw = arg0->target;
                raw &= 0x7F;
                state = (AkaoSequencerState *)track;
                delta = raw << 16;
                delta -= start;
                base = g_AkaoVoiceStateTable2;
                state->secondary.pitch_slide_duration = duration;
                value = 1;
                next_track = track + value;
                g_AkaoCurTrack = next_track;
                state->secondary.pitch_delta = delta / duration;
                Seq_MarkDirtyTracks(base);
                g_AkaoCurTrack--;
            }
        }
    }
}

void Seq_SetGlobalPitchImmediate(AkaoGlobalPitchCommand *arg0) {
    int value;

    value = arg0->pitch;
    g_AkaoGlobalPitchSlideCounter = 0;
    g_AkaoCdInputVolumeAccum = value << 16;
    Seq_ApplyGlobalPitch();
}

void Seq_SlideGlobalPitchToTarget(AkaoGlobalPitchSlideCommand *arg0) {
    int duration;
    int raw_duration;
    int value;

    raw_duration = arg0->duration;
    duration = 1;
    if (raw_duration != 0) {
        duration = raw_duration;
    }

    value = arg0->target << 16;
    g_AkaoGlobalPitchSlideCounter = duration;
    g_AkaoGlobalPitchSlideStep = (value - g_AkaoCdInputVolumeAccum) / duration;
}

void Seq_SlideGlobalPitchFromStartToTarget(AkaoGlobalPitchRangeCommand *arg0) {
    int duration;
    int raw_duration;
    int start;
    int target;

    raw_duration = arg0->duration;
    duration = 1;
    if (raw_duration != 0) {
        duration = raw_duration;
    }

    target = arg0->target << 16;
    start = arg0->start << 16;
    g_AkaoGlobalPitchSlideCounter = duration;
    g_AkaoCdInputVolumeAccum = start;
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
            AkaoTrack *parameters = (AkaoTrack *)(voice - 0xF4);
            if ((active & mask) != 0) {
                if ((parameters->key_on_mask & arg0[2]) != 0) {
                    value = (arg0[3] & 0x7F) << 8;
                    parameters->panpot_duration = 0;
                    parameters->pan_target = value;
                    parameters->update_flags |= AKAO_VOICE_PARAM_VOLUME;
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
            AkaoTrack *parameters = (AkaoTrack *)(voice - 0xF4);
            if ((active & mask) != 0) {
                if (parameters->key_off_mask == arg0[1]) {
                    value = (arg0[3] & 0x7F) << 8;
                    parameters->panpot_duration = 0;
                    parameters->pan_target = value;
                    parameters->update_flags |= AKAO_VOICE_PARAM_VOLUME;
                }
            }
            i++;
            voice += sizeof(AkaoTrack);
            mask <<= 1;
        } while (i < 12);
    }
}

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
            AkaoTrack *parameters = (AkaoTrack *)(voice - 0x74);
            if ((active & mask) != 0) {
                if ((parameters->key_on_mask & arg0[2]) != 0) {
                    step = 1;
                    if (arg0[3] != 0) {
                        step = arg0[3];
                    }
                    delta = ParameterDelta(arg0[4], (u16)parameters->pan_target, step);
                    parameters->pan_delta = delta;
                    parameters->panpot_duration = step;
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
            AkaoTrack *parameters = (AkaoTrack *)(voice - 0x74);
            if ((active & mask) != 0) {
                if (parameters->key_off_mask == arg0[1]) {
                    step = 1;
                    if (arg0[3] != 0) {
                        step = arg0[3];
                    }
                    delta = ParameterDelta(arg0[4], (u16)parameters->pan_target, step);
                    parameters->pan_delta = delta;
                    parameters->panpot_duration = step;
                }
            }
            i++;
            voice += sizeof(AkaoTrack);
            mask <<= 1;
        } while (i < 12);
    }
}

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
    voice = (char *)&g_AkaoVoiceChannelTable[0].update_flags;

    do {
            AkaoTrack *parameters = (AkaoTrack *)(voice - 0xF4);
        if ((active & mask) != 0) {
            if ((parameters->key_on_mask & block_flag) == 0) {
                value = arg0[1];
                dirty = parameters->update_flags;
                parameters->panpot_duration = 0;
                value &= 0x7F;
                value <<= 8;
                dirty |= AKAO_VOICE_PARAM_VOLUME;
                parameters->pan_target = value;
                parameters->update_flags = dirty;
            }
        }
        i++;
        voice += sizeof(AkaoTrack);
        mask <<= 1;
    } while (i < 12);
}

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
    voice = (char *)&g_AkaoVoiceChannelTable[0].panpot_duration;

    do {
            AkaoTrack *parameters = (AkaoTrack *)(voice - 0x74);
        if ((active & mask) != 0) {
            if ((parameters->key_on_mask & block_flag) == 0) {
                step = 1;
                if (arg0[1] != 0) {
                    step = arg0[1];
                }
                delta = (short)(((arg0[2] & 0x7F) << 8) - (u16)parameters->pan_target);
                parameters->panpot_duration = step;
                parameters->pan_delta = delta / (short)step;
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
            AkaoTrack *parameters = (AkaoTrack *)(voice - 0xF4);
            if ((active & mask) != 0) {
                if ((parameters->key_on_mask & arg0[2]) != 0) {
                    value = ((u8 *)arg0)[0xC];
                    dirty = parameters->update_flags;
                    parameters->panpot_slide_duration = 0;
                    value <<= 8;
                    dirty |= AKAO_VOICE_PARAM_VOLUME;
                    parameters->panpot = value;
                    parameters->update_flags = dirty;
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
            AkaoTrack *parameters = (AkaoTrack *)(voice - 0xF4);
            if ((active & mask) != 0) {
                if (parameters->key_off_mask == arg0[1]) {
                    value = ((u8 *)arg0)[0xC];
                    dirty = parameters->update_flags;
                    parameters->panpot_slide_duration = 0;
                    value <<= 8;
                    dirty |= AKAO_VOICE_PARAM_VOLUME;
                    parameters->panpot = value;
                    parameters->update_flags = dirty;
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
            AkaoTrack *parameters = (AkaoTrack *)(voice - 0x78);
            if ((active & mask) != 0) {
                if ((parameters->key_on_mask & arg0[2]) != 0) {
                    step = 1;
                    if (arg0[3] != 0) {
                        step = arg0[3];
                    }
                    delta = PanDelta(((u8 *)arg0)[0x10], parameters->panpot, step);
                    parameters->panpot_delta = delta;
                    parameters->panpot_slide_duration = step;
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
            AkaoTrack *parameters = (AkaoTrack *)(voice - 0x78);
            if ((active & mask) != 0) {
                if (parameters->key_off_mask == arg0[1]) {
                    step = 1;
                    if (arg0[3] != 0) {
                        step = arg0[3];
                    }
                    delta = PanDelta(((u8 *)arg0)[0x10], parameters->panpot, step);
                    parameters->panpot_delta = delta;
                    parameters->panpot_slide_duration = step;
                }
            }
            i++;
            voice += sizeof(AkaoTrack);
            mask <<= 1;
        } while (i < 12);
    }
}

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
    voice = (char *)&g_AkaoVoiceChannelTable[0].update_flags;

    do {
            AkaoTrack *parameters = (AkaoTrack *)(voice - 0xF4);
        if ((active & mask) != 0) {
            if ((parameters->key_on_mask & block_flag) == 0) {
                value = ((u8 *)arg0)[4];
                dirty = parameters->update_flags;
                parameters->panpot_slide_duration = 0;
                value <<= 8;
                dirty |= AKAO_VOICE_PARAM_VOLUME;
                parameters->panpot = value;
                parameters->update_flags = dirty;
            }
        }
        i++;
        voice += sizeof(AkaoTrack);
        mask <<= 1;
    } while (i < 12);
}

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
    voice = (char *)&g_AkaoVoiceChannelTable[0].panpot_slide_duration;

    do {
            AkaoTrack *parameters = (AkaoTrack *)(voice - 0x78);
        if ((active & mask) != 0) {
            if ((parameters->key_on_mask & block_flag) == 0) {
                step = 1;
                if (arg0[1] != 0) {
                    step = arg0[1];
                }
                delta = PanDelta(((u8 *)arg0)[8], parameters->panpot, step);
                parameters->panpot_delta = delta;
                parameters->panpot_slide_duration = step;
            }
        }
        i++;
        voice += sizeof(AkaoTrack);
        mask <<= 1;
    } while (i < 12);
}

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
            AkaoTrack *parameters = (AkaoTrack *)(voice - 0xF4);
            if ((active & mask) != 0) {
                if ((parameters->key_on_mask & arg0[2]) != 0) {
                    value = ((u8 *)arg0)[0xC];
                    dirty = parameters->update_flags;
                    parameters->pitch_offset_duration = 0;
                    value <<= 8;
                    dirty |= AKAO_VOICE_PARAM_PITCH;
                    parameters->pitch_offset = value;
                    parameters->update_flags = dirty;
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
            AkaoTrack *parameters = (AkaoTrack *)(voice - 0xF4);
            if ((active & mask) != 0) {
                if (parameters->key_off_mask == arg0[1]) {
                    value = ((u8 *)arg0)[0xC];
                    dirty = parameters->update_flags;
                    parameters->pitch_offset_duration = 0;
                    value <<= 8;
                    dirty |= AKAO_VOICE_PARAM_PITCH;
                    parameters->pitch_offset = value;
                    parameters->update_flags = dirty;
                }
            }
            i++;
            voice += sizeof(AkaoTrack);
            mask <<= 1;
        } while (i < 12);
    }
}

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
            AkaoTrack *parameters = (AkaoTrack *)(voice - 0x70);
            if ((active & mask) != 0) {
                if ((parameters->key_on_mask & arg0[2]) != 0) {
                    step = 1;
                    if (arg0[3] != 0) {
                        step = arg0[3];
                    }
                    delta = PitchDelta(((u8 *)arg0)[0x10], parameters->pitch_offset, step);
                    parameters->pitch_offset_duration = step;
                    parameters->pitch_offset_step = (short)delta;
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
            AkaoTrack *parameters = (AkaoTrack *)(voice - 0x70);
            if ((active & mask) != 0) {
                if (parameters->key_off_mask == arg0[1]) {
                    step = 1;
                    if (arg0[3] != 0) {
                        step = arg0[3];
                    }
                    delta = PitchDelta(((u8 *)arg0)[0x10], parameters->pitch_offset, step);
                    parameters->pitch_offset_duration = step;
                    parameters->pitch_offset_step = (short)delta;
                }
            }
            i++;
            voice += sizeof(AkaoTrack);
            mask <<= 1;
        } while (i < 12);
    }
}

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
    voice = (char *)&g_AkaoVoiceChannelTable[0].update_flags;

    do {
            AkaoTrack *parameters = (AkaoTrack *)(voice - 0xF4);
        if ((active & mask) != 0) {
            if ((parameters->key_on_mask & block_flag) == 0) {
                value = ((u8 *)arg0)[4];
                dirty = parameters->update_flags;
                parameters->pitch_offset_duration = 0;
                value <<= 8;
                dirty |= AKAO_VOICE_PARAM_PITCH;
                parameters->pitch_offset = value;
                parameters->update_flags = dirty;
            }
        }
        i++;
        voice += sizeof(AkaoTrack);
        mask <<= 1;
    } while (i < 12);
}

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
    voice = (char *)&g_AkaoVoiceChannelTable[0].pitch_offset_duration;

    do {
            AkaoTrack *parameters = (AkaoTrack *)(voice - 0x70);
        if ((active & mask) != 0) {
            if ((parameters->key_on_mask & block_flag) == 0) {
                step = 1;
                if (arg0[1] != 0) {
                    step = arg0[1];
                }
                delta = (short)((((u8 *)arg0)[8] << 8) - parameters->pitch_offset);
                parameters->pitch_offset_duration = step;
                parameters->pitch_offset_step = (short)(delta / (short)step);
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
    g_AkaoTempoScaleAccum = value << 16;
}

void Akao_SlideGlobalD2D0ToTarget(AkaoGlobalSlideCommand *cmd) {
    int raw_duration = cmd->duration;
    int duration = 1;
    int step;

    if (raw_duration != 0) {
        duration = raw_duration;
    }

    step = ((cmd->target << 16) - g_AkaoTempoScaleAccum) / duration;
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
    g_AkaoTempoScaleAccum = start;
    step = ((cmd->target << 16) - start) / duration;
    g_AkaoGlobalD2D0SlideCounter = duration;
    g_AkaoGlobalD2D0SlideStep = step;
}

void Akao_SetGlobalD2CCImmediate(AkaoGlobalParamCommand *cmd) {
    int value = cmd->value;

    g_AkaoGlobalD2CCSlideCounter = 0;
    g_AkaoGlobalPitchDepth = value << 16;
}

void Akao_InitVoices(int arg0, char *arg1);

void Akao_SlideGlobalD2CCToTarget(AkaoGlobalSlideCommand *cmd) {
    int raw_duration = cmd->duration;
    int duration = 1;
    int step;

    if (raw_duration != 0) {
        duration = raw_duration;
    }

    step = ((cmd->target << 16) - g_AkaoGlobalPitchDepth) / duration;
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
    g_AkaoGlobalPitchDepth = start;
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
    int value = cmd->value;

    Akao_InitVoices(value, (char *)&base[0]);
    if (cmd->value != 0) {
        g_AkaoCurTrack++;
        Akao_InitVoices(cmd->value, (char *)&base[AKAO_VOICE_COUNT]);
        g_AkaoCurTrack--;
    }
}

void Spu_StopActiveVoices(void) {
    AkaoTrack *voice = g_AkaoVoiceChannelTable;
    unsigned int mask = AKAO_SPU_VOICE_SFX_START_MASK;
    unsigned int i = 0;
    unsigned int skip_flag = 0x02000000;
    AkaoU32 *track_flags = &voice->flags;

    do {
        if (g_SpuActiveVoiceMask & mask) {
            AkaoTrack *fields = (AkaoTrack *)((AkaoU8 *)track_flags - 0x38);

            if ((fields->key_on_mask & skip_flag) == 0) {
                g_SpuPendingKeyOffMask |= mask;
                SeqOp_DeactivateVoice(voice, mask);
                fields->flags = 0;
            }
        }
        i++;
        track_flags += sizeof(AkaoTrack) / sizeof(AkaoU32);
        voice++;
        mask <<= 1;
    } while (i < 0xC);

    g_AkaoVoiceUpdateFlags |= AKAO_VOICE_PARAM_PITCH;
    Seq_MarkTrack34MaskDirty();
    Seq_MarkTrack38MaskDirty();
    Seq_MarkTrack3CMaskDirty();
}


void Seq_SetPlaybackMode1AndRefreshVoices(void) {
    AkaoTrack *base = g_AkaoVoiceStateTable;

    g_AkaoPlaybackMode = 1;
    Seq_MarkDirtyTracks(base);
    g_AkaoCurTrack++;
    Seq_MarkDirtyTracks(&base[AKAO_VOICE_COUNT]);
    g_AkaoCurTrack--;
    Spu_MarkActiveVoicesDirty();
}

void Seq_SetPlaybackMode4AndRefreshVoices(void) {
    AkaoTrack *base = g_AkaoVoiceStateTable;

    g_AkaoPlaybackMode = 4;
    Seq_MarkDirtyTracks(base);
    g_AkaoCurTrack++;
    Seq_MarkDirtyTracks(&base[AKAO_VOICE_COUNT]);
    g_AkaoCurTrack--;
    Spu_MarkActiveVoicesDirty();
}

void Seq_SetPlaybackMode2AndRefreshVoices(void) {
    AkaoTrack *base = g_AkaoVoiceStateTable;

    g_AkaoPlaybackMode = 2;
    Seq_MarkDirtyTracks(base);
    g_AkaoCurTrack++;
    Seq_MarkDirtyTracks(&base[AKAO_VOICE_COUNT]);
    g_AkaoCurTrack--;
    Spu_MarkActiveVoicesDirty();
}

void Seq_SetGlobalD2B8AndDirtyAllTracks(AkaoValueCommand *cmd) {
    unsigned int i = 0;
    int value = cmd->value;
    AkaoTrackUpdateSlot *slot;

    slot = g_AkaoTrackStateArray;
    g_AkaoVoicePortamentoResetMask = value;
    for (; i < 0x18; i++, slot++) {
        slot->update_flags |= AKAO_VOICE_PARAM_VOLUME;
    }
}

void Seq_SetCurrentTrackField56(AkaoValueCommand *arg0) {
    g_AkaoCurTrack->condition_value = arg0->value;
}

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
  AkaoTrack *voice;
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
        if ((voice->key_on_mask & 0x02000000) != 0)
        {
          pending &= ~bit;
        }
      }
      i++;
      voice++;
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

void Seq_GetGlobalPitch(unsigned int *out);

void Spu_RestoreStoppedVoices(void) {
    unsigned int pending = g_SpuStoppedVoiceMask;
    unsigned int saved;
    unsigned int mask;
    AkaoTrackUpdateSlot *slot;

    if (pending != 0) {
        mask = AKAO_SPU_VOICE_SFX_START_MASK;
        slot = (AkaoTrackUpdateSlot *)&g_AkaoVoiceChannelTable[0].update_flags;
        do {
            if (pending & mask) {
                pending &= ~mask;
                slot->update_flags |= AKAO_VOICE_PARAM_RESUME;
            }
            mask <<= 1;
            slot++;
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
    AkaoQueueEntry *entry = g_AkaoMessageQueue;

    if (g_AkaoMessageQueueCount != 0) {
        do {
            Akao_MessageHandlers[entry->opcode.id](entry);
            g_AkaoMessageQueueCount--;
            entry++;
        } while (g_AkaoMessageQueueCount != 0);
    }
}

void Akao_AllocMessageSlot(AkaoQueueEntry **out_msg) {
    *out_msg = g_AkaoMessageQueue;
    *out_msg = &g_AkaoMessageQueue[g_AkaoMessageQueueCount];
    g_AkaoMessageQueueCount++;
}

void Seq_SetParamWithReset(unsigned int arg0) {
    unsigned int value;

    Seq_GetGlobalPitch(&value);
    if (value != arg0) {
        SpuSetReverb(0);
        SpuSetReverbModeType(arg0 | 0x100);
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
    g_AkaoMessageQueueLock = 1;
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
        if (D_8009CD88->timing.bank_id != result) {
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
        sequence = g_AkaoCmd24Sequence;
        msg->arg2 = g_AkaoCmd.arg2;
        g_AkaoCmd24Sequence.value = ((sequence.value + 1) & 0x1FF) + 0x400;
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
    g_AkaoMessageQueueLock = 0;
    return result;
}
