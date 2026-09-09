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
    char *voice;
    register char *field asm("$4");
    int flags;

    if (arg0 == 0) {
        if (*(int *)(g_AkaoCurTrack + 4) != 0) {
            goto body;
        }
        return;
    } else if (arg0 != *(u16 *)(g_AkaoCurTrack + 0x54)) {
        return;
    }

body:
    *(int *)(g_AkaoCurTrack + 0x18) = 0xFFFFFF;

    i = 0;
    mode3 = 3;
    mode1 = 1;
    fallback = g_AkaoDefaultVoiceProgram;
    mode5 = 5;
    field = arg1 + 0x116;
    voice = arg1;

    do {
        i++;
        *(u16 *)(field - 0xC0) = mode3;
        *(u16 *)(field - 0xBE) = mode1;
        *(char **)(voice + 0) = fallback;
        flags = *(int *)(field - 0x22);
        voice += sizeof(AkaoTrack);
        *(u16 *)(field + 0) = mode5;
        flags |= AKAO_VOICE_PARAM_ADSR_RELEASE;
        *(int *)(field - 0x22) = flags;
        field += sizeof(AkaoTrack);
    } while (i < 0x18);
}

void Spu_ManageVoices(int arg0, int arg1) {
    u32 mask;
    u32 i;
    register char *field asm("$18");
    char *voice;
    u32 id;
    u32 active;
    register int control asm("$22");
    register u32 flag1 asm("$23");
    int value;
    int flags;
    int best;

    control = arg1;
    mask = AKAO_SPU_VOICE_SFX_START_MASK;
    id = (u16_1)arg0;

    if (id == 0xFFFF) {
        return;
    }

    voice = g_AkaoVoiceChannelTable;
    active = g_SpuActiveVoiceMask;

    if ((control & 0x0FFFFFFF) != 0) {
        i = 0;
        flag1 = 0x100000;
        id = 0x200000;
        field = voice + 0x38;
        do {
            if ((active & mask) != 0) {
                if ((*(int *)(field - 0xC) & control) != 0) {
                    flags = *(int *)(field + 0);
                    value = flags & flag1;
                    if (value != 0) {
                        value = flags | id;
                        *(int *)(field + 0) = value;
                    } else {
                        g_SpuPendingKeyOffMask |= mask;
                        SeqOp_DeactivateVoice(voice, mask);
                        *(int *)(field + 0) = 0;
                    }
                }
            }
            i++;
            field += sizeof(AkaoTrack);
            voice += sizeof(AkaoTrack);
            mask <<= 1;
        } while (i < 12);
        goto finish;
    }

    if (control < 0) {
        value = (((id << 3) + id) << 3) - id;
        voice = g_AkaoVoiceChannelTable + (value << 2);
        mask <<= id;
        if ((active & mask) != 0) {
            Spu_ManageVoices(*(int *)(voice + 0x28), 0);
        }
        mask <<= 1;
        voice += sizeof(AkaoTrack);
        value = active & mask;
        if (value != 0) {
            Spu_ManageVoices(*(int *)(voice + 0x28), 0);
        }
        return;
    }

    i = 0;
    value = control & 0x40000000;
    if (value != 0) {
        do {
            value = *(int *)(voice + 0x2C);
            if (value != 0) {
                active &= ~mask;
            }
            i++;
            voice += sizeof(AkaoTrack);
            mask <<= 1;
        } while (i < 12);

        voice = g_AkaoVoiceChannelTable;
        mask = AKAO_SPU_VOICE_SFX_START_MASK;
        best = 0;
        i = 0;
        do {
            if ((active & mask) != 0) {
                flags = *(int *)(voice + 0x50);
                if (best < flags) {
                    best = flags;
                }
            }
            i++;
            voice += sizeof(AkaoTrack);
            mask <<= 1;
        } while (i < 12);

        voice = g_AkaoVoiceChannelTable;
        mask = AKAO_SPU_VOICE_SFX_START_MASK;
        i = 0;
        flag1 = 0x100000;
        control = 0x200000;
        field = voice + 0x38;
        do {
            if ((active & mask) != 0) {
                if (best == *(int *)(field + 0x18)) {
                    flags = *(int *)(field + 0);
                    value = flags & flag1;
                    if (value != 0) {
                        value = flags | control;
                        *(int *)(field + 0) = value;
                    } else {
                        g_SpuPendingKeyOffMask |= mask;
                        SeqOp_DeactivateVoice(voice, mask);
                        *(int *)(field + 0) = 0;
                    }
                }
            }
            i++;
            field += sizeof(AkaoTrack);
            voice += sizeof(AkaoTrack);
            mask <<= 1;
        } while (i < 12);
        goto finish;
    }

    flag1 = 0x100000;
    control = 0x200000;
    field = voice + 0x38;
    do {
        if ((active & mask) != 0) {
            if (*(int *)(field - 0x10) == id) {
                flags = *(int *)(field + 0);
                value = flags & flag1;
                if (value != 0) {
                    value = flags | control;
                    *(int *)(field + 0) = value;
                } else {
                    g_SpuPendingKeyOffMask |= mask;
                    SeqOp_DeactivateVoice(voice, mask);
                    *(int *)(field + 0) = 0;
                }
            }
        }
        i++;
        field += sizeof(AkaoTrack);
        voice += sizeof(AkaoTrack);
        mask <<= 1;
    } while (i < 12);

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
    register AkaoTrack *track_reg asm("$8");
    unsigned int mask = voice_mask;
    int pan;
    int pan_target;
    register unsigned int *active_mask asm("$4");
    unsigned int old_active_mask;
    unsigned int old_pending_mask;

    track_reg = track;
    track_reg->key_off_mask = source->key_off_mask;
    track_reg->key_on_mask = source->key_on_mask;
    asm volatile("" : : : "memory");
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
        AkaoNestedVoiceSlot *slot;
        register unsigned int busy_mask asm("$7");
        register unsigned int *active_loop_mask asm("$6");
        int count;

        mask = 0x1000;
        slot = D_800BC000;
        count = 0xC;
        busy_mask = 0x02000000;
        active_loop_mask = active_mask;
        do {
            if ((slot->flags & busy_mask) == 0) {
                unsigned int not_mask;
                unsigned int active_value;
                register unsigned int off_value asm("$4");

                not_mask = ~mask;
                active_value = *active_loop_mask;
                off_value = D_800BCD60;
                active_value &= not_mask;
                off_value |= mask;
                *active_loop_mask = active_value;
                D_800BCD60 = off_value;
            }
            count--;
            slot++;
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
