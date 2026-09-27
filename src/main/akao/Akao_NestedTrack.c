#include "common.h"
#include "pe1/akao.h"

extern char *g_AkaoCurTrack;
extern char g_AkaoDefaultVoiceProgram[];

typedef unsigned short u16_1;

extern char g_AkaoVoiceChannelTable[];
extern u32 g_SpuActiveVoiceMask;
extern u32 g_SpuPendingKeyOffMask;
extern u32 g_AkaoVoiceUpdateFlags;

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
