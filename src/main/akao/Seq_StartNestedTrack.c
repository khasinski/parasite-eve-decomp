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
