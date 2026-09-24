#include "common.h"
void Asset_Find08w(int arg0, int arg1, int arg2, int arg3, int arg4);

typedef struct AkaoVoiceActor {
    u8 pad_00[0x0C];
    u8 field_0c;
    u8 field_0d;
    u8 field_0e;
    u8 field_0f;
    u8 pad_10[0x06];
    u16 field_16;
    u8 pad_18[0x02];
    u16 field_1a;
    int field_1c;
    u8 pad_20[0x0A];
    s16 field_2a;
    u8 pad_2c[0x02];
    s16 field_2e;
    u8 pad_30[0x02];
    s16 field_32;
} AkaoVoiceActor;

typedef struct AkaoVoiceBankEntry {
    u8 field_0;
    u8 field_1;
    u8 field_2;
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
extern struct { char _[16]; } D_8009D254_o __asm__("D_8009D254");
extern struct { char _[16]; } D_8009D1A0_o __asm__("D_8009D1A0");
#define D_8009D254 (*(AkaoVoiceActor **)&D_8009D254_o)
#define D_8009D1A0 (*(int *)&D_8009D1A0_o)

int Akao_LoadVoiceBank(AkaoVoiceActor *actor) {
    register AkaoVoiceActor *actor_reg asm("$17") = actor;
    register int frame asm("$19");
    register int step asm("$22");
    register int target asm("$21");
    register int i asm("$18");
    int offset;
    register AkaoVoiceBankState *state asm("$23");
    int id;

    if (actor_reg == 0) {
        return -1;
    }

    state = &D_800B0CD8;
    frame = actor_reg->field_16;
    step = *(int *)((char *)actor_reg + 0x1C);
    target = actor_reg->field_1a;

    if (step > 0 && frame < target) {
        register int next_frame asm("$2");
        register int extra asm("$3");
        next_frame = frame + 1;
        extra = actor_reg->field_0f;
        frame = next_frame + extra;
    } else if (step < 0 && target < frame) {
        register int next_frame asm("$2");
        register int extra asm("$3");
        next_frame = frame - 1;
        extra = actor_reg->field_0f;
        frame = next_frame - extra;
    }

    if (actor_reg == D_8009D254 && (D_8009D1A0 & 2) == 0 && (state->flags & 0x800000) == 0) {
        register u16 *entry_ids asm("$20");
        entry_ids = D_80094488[0].ids;
        offset = 0;
        for (i = 0; i < 4; entry_ids += 4, i++, offset += 8) {
            if (*(u8 *)((char *)D_80094488 + offset + 2) == actor_reg->field_0e) {
                int entry_frame = *(u8 *)((char *)D_80094488 + offset + 3);
                if ((step > 0 && entry_frame >= target && entry_frame < frame) ||
                    (step < 0 && frame < entry_frame && entry_frame <= target)) {
                    id = entry_ids[state->active_id_slot];
                    if (id != 0) {
                        Asset_Find08w(id, 0, *(s16 *)((char *)actor_reg + 0x2A),
                                      *(s16 *)((char *)actor_reg + 0x2E), *(s16 *)((char *)actor_reg + 0x32));
                    }
                }
            }
        }
    }

    i = 4;
    if (i >= state->first_dynamic_entry + 4) {
        return 0;
    }

    offset = 0x20;
    do {
        if (*(u8 *)((char *)D_80094488 + offset) == actor_reg->field_0c &&
            *(u8 *)((char *)D_80094488 + offset + 1) == actor_reg->field_0d &&
            *(u8 *)((char *)D_80094488 + offset + 2) == actor_reg->field_0e) {
            int entry_frame = *(u8 *)((char *)D_80094488 + offset + 3);
            if ((step > 0 && entry_frame >= target && entry_frame < frame) ||
                (step < 0 && frame < entry_frame && entry_frame <= target)) {
                register u16 *ids_base asm("$2") = D_80094488[0].ids;
                id = *(u16 *)((char *)ids_base + offset + state->active_id_slot * 2);
                if (id != 0) {
                    Asset_Find08w(id, 0, *(s16 *)((char *)actor_reg + 0x2A),
                                  *(s16 *)((char *)actor_reg + 0x2E), *(s16 *)((char *)actor_reg + 0x32));
                }
            }
        }

        offset += 8;
        i++;
    } while (i < state->first_dynamic_entry + 4);

    return 0;
}
