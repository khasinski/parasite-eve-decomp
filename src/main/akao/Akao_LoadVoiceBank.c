#include "common.h"
void Asset_Find08w(int arg0, int arg1, int arg2, int arg3, int arg4);

typedef struct AkaoVoiceActor {
    u8 pad_00[0x0C];
    u8 animGroup;
    u8 animSet;
    u8 animIndex;
    u8 frameSkip;
    u8 pad_10[0x06];
    u16 frame;
    u8 pad_18[0x02];
    u16 targetFrame;
    int frameStep;
    u8 pad_20[0x0A];
    s16 posXInteger; /* integer halves of the 16.16 position */
    u8 pad_2c[0x02];
    s16 posYInteger;
    u8 pad_30[0x02];
    s16 posZInteger;
} AkaoVoiceActor;

typedef struct AkaoVoiceBankEntry {
    u8 group;
    u8 set;
    u8 index;
    u8 frame;
    u16 ids[2];
} AkaoVoiceBankEntry;

typedef struct AkaoVoiceBankState {
    int flags;
    u8 pad_04[0x0D];
    u8 first_dynamic_entry;
    u8 active_id_slot;
} AkaoVoiceBankState;

extern AkaoVoiceBankEntry D_80094488[];
extern AkaoVoiceBankState D_800B0CD8;
extern AkaoVoiceActor *D_8009D254;
extern int D_8009D1A0;

int Akao_LoadVoiceBank(AkaoVoiceActor *actor) {
    AkaoVoiceActor *actor_reg = actor;
    int frame;
    int step;
    int target;
    int i;
    int entry;
    AkaoVoiceBankState *state;
    int id;

    if (actor_reg == 0) {
        return -1;
    }

    state = &D_800B0CD8;
    frame = actor_reg->frame;
    step = actor_reg->frameStep;
    target = actor_reg->targetFrame;

    if (step > 0 && frame < target) {
        int next_frame;
        int extra;
        next_frame = frame + 1;
        extra = actor_reg->frameSkip;
        frame = next_frame + extra;
    } else if (step < 0 && target < frame) {
        int next_frame;
        int extra;
        next_frame = frame - 1;
        extra = actor_reg->frameSkip;
        frame = next_frame - extra;
    }

    if (actor_reg == D_8009D254 && (D_8009D1A0 & 2) == 0 && (state->flags & 0x800000) == 0) {
        u16 *entry_ids;
        entry_ids = D_80094488[0].ids;
        entry = 0;
        for (i = 0; i < 4; entry_ids += 4, i++, entry++) {
            if (D_80094488[entry].index == actor_reg->animIndex) {
                int entry_frame = D_80094488[entry].frame;
                if ((step > 0 && entry_frame >= target && entry_frame < frame) ||
                    (step < 0 && frame < entry_frame && entry_frame <= target)) {
                    id = entry_ids[state->active_id_slot];
                    if (id != 0) {
                        Asset_Find08w(id, 0, actor_reg->posXInteger,
                                      actor_reg->posYInteger, actor_reg->posZInteger);
                    }
                }
            }
        }
    }

    i = 4;
    if (i >= state->first_dynamic_entry + 4) {
        return 0;
    }

    entry = 4;
    do {
        if (D_80094488[entry].group == actor_reg->animGroup &&
            D_80094488[entry].set == actor_reg->animSet &&
            D_80094488[entry].index == actor_reg->animIndex) {
            int entry_frame = D_80094488[entry].frame;
            if ((step > 0 && entry_frame >= target && entry_frame < frame) ||
                (step < 0 && frame < entry_frame && entry_frame <= target)) {
                id = D_80094488[entry].ids[state->active_id_slot];
                if (id != 0) {
                    Asset_Find08w(id, 0, actor_reg->posXInteger,
                                  actor_reg->posYInteger, actor_reg->posZInteger);
                }
            }
        }

        entry++;
        i++;
    } while (i < state->first_dynamic_entry + 4);

    return 0;
}
