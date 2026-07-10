#include "pe1/akao.h"

typedef struct AkaoNestedVoiceSlot {
    unsigned char pad_00[0x2C];
    unsigned int field_2C;
    unsigned char pad_30[0xEC];
} AkaoNestedVoiceSlot;

typedef struct AkaoNestedSource {
    unsigned char pad_00[4];
    unsigned int key_off_mask;
    unsigned int key_on_mask;
    unsigned char pan;
    unsigned char pad_0D[3];
    unsigned int field_10;
} AkaoNestedSource;

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
    register unsigned int mask asm("$16") = voice_mask;
    AkaoNestedVoiceSlot *slot;
    int count;
    unsigned int slot_mask;

    track->key_off_mask = source->key_off_mask;
    track->key_on_mask = source->key_on_mask;
    track->panpot_slide_duration = 0;
    track->panpot = source->pan << 8;
    track->field_56 = 2;
    track->pan_duration = 1;
    track->parent_track_id = 1;
    track->panpot_duration = 0;
    track->field_50_duration = -2;
    track->pan_target = (source->field_10 & 0x7F) << 8;

    Akao_InitVoiceState(track, script);

    D_800BCD50 |= mask;
    D_800BCD5C |= mask;

    mask = ~mask;
    D_800BCD54 &= mask;
    D_800BCD58 &= mask;
    D_800BCD6C &= mask;
    D_800BCD70 &= mask;
    D_800BCD74 &= mask;

    if ((D_8009D2DC & 2) != 0) {
        slot = D_800BC000;
        count = 0xC;
        slot_mask = 0x1000;
        do {
            if ((slot->field_2C & 0x02000000) == 0) {
                D_800BCD50 &= ~slot_mask;
                D_800BCD60 |= slot_mask;
            }
            count--;
            slot++;
            slot_mask <<= 1;
        } while (count != 0);
    }
}
