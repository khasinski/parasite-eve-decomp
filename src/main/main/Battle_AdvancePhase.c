/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;
typedef signed short s16;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

extern struct { char _[16]; } D_800BE830_o __asm__("D_800BE830");
extern struct { char _[16]; } D_8009D254_o __asm__("D_8009D254");
extern struct { char _[16]; } D_8009D278_o __asm__("D_8009D278");
extern struct { char _[16]; } D_8009D20C_o __asm__("D_8009D20C");
extern struct { char _[16]; } D_800942E4_o __asm__("D_800942E4");

#define D_8009D254 (*(u8 **)&D_8009D254_o)
#define D_8009D278 (*(u8 **)&D_8009D278_o)
#define D_8009D20C (*(u8 **)&D_8009D20C_o)
#define D_800942E4 (*(u8 **)&D_800942E4_o)
#define QUEUE_ENTRY(index) ((u8 *)&D_800BE830_o + ((u8)(index) << 3))
#define QUEUE_ENTITY(index) (*(u8 **)QUEUE_ENTRY(index))
#define QUEUE_ACTION(index) (*(s16 *)(QUEUE_ENTRY(index) + 4))

#define U8_AT(ptr, off) (*(u8 *)((u8 *)(ptr) + (off)))
#define S8_AT(ptr, off) (*(s8 *)((u8 *)(ptr) + (off)))
#define U16_AT(ptr, off) (*(u16 *)((u8 *)(ptr) + (off)))
#define S16_AT(ptr, off) (*(s16 *)((u8 *)(ptr) + (off)))
#define U32_AT(ptr, off) (*(u32 *)((u8 *)(ptr) + (off)))
#define S32_AT(ptr, off) (*(int *)((u8 *)(ptr) + (off)))

extern u8 D_8009D1D4;
extern u8 D_8009CE3C;
extern u8 D_8009CE38;
extern u8 D_8009CE39;
extern u8 D_8009CE3A;
extern u8 D_8009CE3B;
extern s8 D_8009D2A0;
extern u32 D_8009D1A0;
extern int D_8009D258;
extern s16 D_8009D27C;

int Akao_SendPositionalCmdStereo(int cmd, int arg1, int x, int y, int z);
int Battle_ApplySpellEffect(int action, void *entity);
int Battle_CalcAngleToTarget(void *from, void *to);
int Battle_CalcDistToPlayer(void *entity, void *player);
int Battle_DispatchEntityEffect(void);
int Battle_HaltOnPositiveX(void);
int Battle_StepAyaAction(void);
int Battle_StepCharacterAction(void *entry);
int BattleCmd_CommitAmmoAndUpdate(void);
int Gte_Atan2(int y, int x);
int Inv_CheckSlotUsable(int slot);
int Scene_LoadRoomAssets(int id, void *entity);
void Battle_UpdateEntityFacing(void *entity);
void Entity_SetActionMode(void *entity, int mode);
void Pm_StopAll(void);

static int Battle_IsItemAction(int action) {
    return (unsigned int)(action - 3) < 0x180;
}

static int Battle_IsSpecialAction(int action) {
    return (unsigned int)(action - 3) < 0x194;
}

static int Battle_ActionRoomIndex(int action) {
    return action - 0x183;
}

static int Battle_QueueEntryAlive(int index) {
    u8 *slot;
    u8 *actor;

    slot = QUEUE_ENTITY(index);
    if (slot == 0) {
        return 0;
    }
    actor = *(u8 **)slot;
    if (actor == 0) {
        return 0;
    }
    return S32_AT(actor, 0x10) > 0;
}

static int Battle_QueueEntryBusy(int index) {
    u8 *slot;

    slot = QUEUE_ENTITY(index);
    if (!Battle_QueueEntryAlive(index)) {
        return 0;
    }
    return (U32_AT(slot, 0x98) & 0x4000) == 0;
}

static void Battle_ClearQueuedItems(void) {
    int index;
    int action;

    index = D_8009D1D4;
    while ((u8)index < D_8009CE3C) {
        action = QUEUE_ACTION(index);
        if (Battle_IsItemAction(action)) {
            Inv_CheckSlotUsable(action - 3);
        }
        index++;
    }

    D_8009CE3C = 0;
    D_8009D1D4 = 0;
    D_8009D1A0 &= ~0x100;
}

static void Battle_AdvanceCursorAndMaybeClearFlag(void) {
    int next;

    next = D_8009D1D4 + 1;
    D_8009D1D4 = next;
    if (!Battle_IsSpecialAction(QUEUE_ACTION(next)) || U8_AT(D_8009D254, 0x0E) >= 4) {
        D_8009D1A0 &= ~0x100;
    }
}

static int Battle_MapActionTypeIsSimple(void) {
    u8 *room;
    int idx;
    int value;

    room = D_800942E4;
    if (room == 0) {
        return 0;
    }
    idx = D_8009D258;
    value = U8_AT(room, (((((idx * 5) << 5) + idx) << 2) - idx) << 2);
    return (unsigned int)(value - 1) < 2;
}

static void Battle_ClearEntityAnimLocks(void) {
    u8 *node;
    u8 *actor;

    node = D_8009D20C;
    while (node != 0) {
        if (node != D_8009D254) {
            actor = *(u8 **)node;
            if (actor != 0 && *(u8 **)(actor + 0x18) != 0) {
                U8_AT(*(u8 **)(actor + 0x18), 0) = 4;
                U32_AT(actor, 0) &= 0xC0FFFFFF;
            }
        }
        node = *(u8 **)(node + 4);
    }
}

static void Battle_SendActionSound(int action, u8 *slot) {
    static u16 sound_ids[] = {
        0x0101, 0x0102, 0x0103, 0x0104, 0x0105, 0x0106, 0x0107, 0x0108,
        0x0109, 0x010A, 0x010B, 0x010C, 0x010D, 0x010E, 0x010F, 0x0110,
        0x0111, 0x0112, 0x0113, 0x0114,
    };
    int idx;
    int x;
    int y;
    int z;

    idx = Battle_ActionRoomIndex(action);
    if ((unsigned int)idx >= 20) {
        return;
    }

    if (idx == 7 || idx == 8 || idx == 10) {
        x = S16_AT(QUEUE_ENTITY(D_8009D1D4), 0x268);
        y = S16_AT(QUEUE_ENTITY(D_8009D1D4), 0x26A);
        z = S16_AT(QUEUE_ENTITY(D_8009D1D4), 0x26C);
    } else {
        x = S16_AT(D_8009D254, 0x2A);
        y = S16_AT(D_8009D254, 0x2E);
        z = S16_AT(D_8009D254, 0x32);
    }
    Akao_SendPositionalCmdStereo(sound_ids[idx], 0, x, y, z);
}

static void Battle_StartSpecialAction(u8 *entry, int action) {
    int index;
    int target_index;
    int keep_slot;
    u8 *slot;
    u8 *player;
    int angle;
    int dir;

    slot = *(u8 **)entry;
    player = D_8009D254;
    if (slot != player) {
        angle = Battle_CalcAngleToTarget(slot + 0x1B4, player + 0x28);
        S16_AT(player, 0x3A) = angle;
    }

    index = D_8009D1D4;
    target_index = Battle_ActionRoomIndex(action);
    if (target_index == 0x13) {
        keep_slot = 0;
        for (dir = index; (u8)dir < (u8)(index + 7); dir++) {
            if (Battle_QueueEntryBusy(dir)) {
                keep_slot = 1;
                slot = QUEUE_ENTITY(dir);
                break;
            }
        }
        if (!keep_slot) {
            D_8009D1D4 += 7;
        } else if (slot != 0) {
            for (dir = index; (u8)dir < (u8)(index + 7); dir++) {
                if (!Battle_QueueEntryAlive(dir)) {
                    QUEUE_ENTITY(dir) = slot;
                }
            }
            return;
        }
    } else if (Battle_QueueEntryBusy(index)) {
        return;
    } else {
        D_8009D1D4++;
    }

    if (Battle_IsSpecialAction(QUEUE_ACTION(D_8009D1D4)) && U8_AT(D_8009D254, 0x0E) < 4) {
        D_8009D1A0 &= ~0x100;
    }

    Entity_SetActionMode(D_8009D254, U8_AT(D_8009D278, 0x12));
    U32_AT(D_8009D278, 0x4C) |= 0x200000;
}

static void Battle_CommitSpecialAction(u8 *entry, int action) {
    int room_id;
    int angle;
    int dist;
    int dir;

    Pm_StopAll();
    room_id = Battle_ActionRoomIndex(action);

    if (room_id == 6) {
        if ((U32_AT(*(u8 **)(D_8009D278 + 0x68), 0x0C) & 0x3FF) == 0) {
            BattleCmd_CommitAmmoAndUpdate();
        }

        U32_AT(D_8009D278, 0x4C) |= 0x100000;
        dist = Battle_CalcDistToPlayer(*(u8 **)entry, D_8009D254);
        angle = Gte_Atan2(S16_AT(*(u8 **)entry, 0x26A) - D_8009D27C, dist);
        if (angle < -0xAB) {
            dir = 0;
        } else if (angle < 0xE4) {
            dir = 1;
        } else {
            dir = 2;
        }
        Entity_SetActionMode(D_8009D254, U8_AT(D_8009D278, 0x14 + dir));
        U32_AT(D_8009D278, 0x4C) &= ~0x200000;
        D_8009CE39 = 0x58;
        D_8009CE38 = 0;
        D_8009CE3A = 0x18;
        D_8009CE3B = 0;
    } else {
        Entity_SetActionMode(D_8009D254, 0x0F);
    }

    if (room_id != 0x13) {
        D_8009D258 = Scene_LoadRoomAssets(room_id, *(void **)entry);
    }

    Battle_SendActionSound(action, *(u8 **)entry);
    D_8009D1A0 |= 0x100;
    U32_AT(D_8009D278, 0x4C) &= ~0x200000;
}

static void Battle_FinishAyaAction(void) {
    int index;
    int action;
    u8 *slot;

    if (!Battle_StepAyaAction()) {
        return;
    }

    U32_AT(D_8009D278, 8) = 0;
    U32_AT(D_8009D254, 0x98) &= ~0x100;
    index = D_8009D1D4 - 7;
    while ((u8)index < D_8009D1D4) {
        slot = QUEUE_ENTITY(index);
        if (slot != D_8009D254) {
            Battle_UpdateEntityFacing(slot);
        }
        index++;
    }

    Battle_ClearEntityAnimLocks();
    U32_AT(D_8009D278, 0x4C) &= ~0x80000;
    Battle_HaltOnPositiveX();
}

static void Battle_StepCharacterPhase(u8 *entry) {
    int action_type;

    S16_AT(D_8009D254, 0x3A) = Battle_CalcAngleToTarget(*(u8 **)entry + 0x1B4, D_8009D254 + 0x28);
    Battle_StepCharacterAction(entry);

    if (Battle_MapActionTypeIsSimple()) {
        return;
    }

    U32_AT(D_8009D278, 8) = 0;
    U32_AT(D_8009D254, 0x98) &= ~0x100;
    D_8009D1A0 &= ~0x100;
    U32_AT(D_8009D278, 0x4C) = (U32_AT(D_8009D278, 0x4C) & ~0x100000) | 0x200000;
    Battle_HaltOnPositiveX();

    action_type = QUEUE_ACTION(D_8009D1D4);
    if (action_type == 1 || action_type == 2) {
        Battle_DispatchEntityEffect();
    }
}

static void Battle_ApplyQueuedEffect(u8 *entry) {
    int action;

    if (U8_AT(D_8009D254, 0x0F) == U16_AT(D_8009D254, 0x1A)) {
        Entity_SetActionMode(D_8009D254, U8_AT(D_8009D278, 0x12));
    }

    if (Battle_MapActionTypeIsSimple()) {
        if (QUEUE_ACTION(D_8009D1D4) != 0x196) {
            return;
        }
    }

    U32_AT(D_8009D278, 0x4C) |= 0x200000;
    action = QUEUE_ACTION(D_8009D1D4);
    Battle_ApplySpellEffect(Battle_ActionRoomIndex(action), *(void **)entry);
    if (action != 0x196) {
        Battle_AdvanceCursorAndMaybeClearFlag();
    }
}

void Battle_AdvancePhase(void) {
    int index;
    int action;
    u8 *entry;

    index = D_8009D1D4;
    entry = QUEUE_ENTRY(index);

    if (D_8009D2A0 == 0) {
        Battle_ClearQueuedItems();
        return;
    }

    if (U32_AT(D_8009D278, 0x4C) & 0x200000) {
        action = QUEUE_ACTION(index);
        Battle_StartSpecialAction(entry, action);
        Battle_CommitSpecialAction(entry, action);
        return;
    }

    if (U32_AT(D_8009D278, 0x4C) & 0x80000) {
        Battle_FinishAyaAction();
        return;
    }

    if (U32_AT(D_8009D278, 0x4C) & 0x100000) {
        Battle_StepCharacterPhase(entry);
        return;
    }

    Battle_ApplyQueuedEffect(entry);
    index = D_8009D1D4;
    while ((u8)index < D_8009CE3C) {
        action = QUEUE_ACTION(index);
        if ((unsigned int)(action - 1) < 2) {
            Battle_DispatchEntityEffect();
            break;
        }
        index++;
    }
}
