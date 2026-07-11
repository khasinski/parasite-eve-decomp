/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;
typedef signed short s16;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

extern struct { char _[16]; } D_8009D254_o __asm__("D_8009D254");
extern struct { char _[16]; } D_8009D278_o __asm__("D_8009D278");
extern struct { char _[16]; } D_8009D2FC_o __asm__("D_8009D2FC");
extern struct { char _[16]; } D_800BE830_o __asm__("D_800BE830");

#define D_8009D254 (*(u8 **)&D_8009D254_o)
#define D_8009D278 (*(u8 **)&D_8009D278_o)
#define D_8009D2FC (*(int *)&D_8009D2FC_o)
#define QUEUE_ENTRY(index) ((u8 *)&D_800BE830_o + ((u8)(index) << 3))
#define QUEUE_ENTITY(index) (*(u8 **)QUEUE_ENTRY(index))
#define QUEUE_ACTION(index) (*(s16 *)(QUEUE_ENTRY(index) + 4))
#define U8_AT(ptr, offset) (*(u8 *)((u8 *)(ptr) + (offset)))
#define S16_AT(ptr, offset) (*(s16 *)((u8 *)(ptr) + (offset)))
#define U16_AT(ptr, offset) (*(u16 *)((u8 *)(ptr) + (offset)))
#define U32_AT(ptr, offset) (*(u32 *)((u8 *)(ptr) + (offset)))

extern u8 D_8009CE38;
extern u8 D_8009CE39;
extern u8 D_8009CE3A;
extern u8 D_8009CE3C;
extern u8 D_8009D1D4;
extern u8 D_8009D1DC;
extern int D_8009D200;
extern u8 D_8009D274;
extern s16 D_8009D27C;

int Battle_CalcDistToPlayer(void *arg0, void *arg1);
int Gte_Atan2(int arg0, int arg1);
void Entity_SetActionMode(void *entity, int mode);
int BattleCmd_CommitAmmoAndUpdate(int arg0);
int Akao_SetPos3D(int arg0, int arg1, u16 x, u16 y, u16 z);
void Battle_FinalizeAttackResult(void);
int Pm_SendCmd(int arg0, int arg1, int arg2, int arg3, int arg4, int arg5);

int Battle_StepCharacterAction(void **arg0) {
    int result;
    int mode;
    int angle;
    int dir;
    int index;
    int next_index;
    u8 *entity;
    u8 *actor;
    u8 *action_def;
    u8 *target_entity;
    u8 *target_core;

    result = 0;
    entity = D_8009D254;
    mode = U8_AT(entity, 0xE);

    if ((u32)(mode - 6) < 6) {
        if ((mode & 1) == 0) {
            if (U8_AT(entity, 0xF) != U16_AT(entity, 0x16)) {
                return result;
            }

            if (D_8009CE38 != 0) {
                D_8009CE38--;
                U32_AT(entity, 0x98) |= 0x100;
                return result;
            }

            if (D_8009CE39 != 0) {
                D_8009CE39--;
                U32_AT(entity, 0x98) |= 0x100;
                return result;
            }

            actor = D_8009D278;
            action_def = *(u8 **)(actor + 0x68);
            D_8009D274 = 0;
            D_8009CE39 = U8_AT(action_def, 0x15);

            angle = Gte_Atan2(S16_AT(*arg0, 0x26A) - D_8009D27C,
                              Battle_CalcDistToPlayer(*arg0, entity));
            if (angle < -0xAB) {
                dir = 0;
            } else if (angle < 0xE4) {
                dir = 1;
            } else {
                dir = 2;
            }

            Entity_SetActionMode(entity, U8_AT(actor, 0x17 + dir));
            U32_AT(entity, 0x98) &= ~0x100;
            D_8009D1DC = U32_AT(action_def, 0x10) & 0xF;
            Battle_FinalizeAttackResult();
            return result;
        }

        if (U8_AT(entity, 0xF) != U16_AT(entity, 0x1A)) {
            return result;
        }

        index = D_8009D1D4;
        if (QUEUE_ACTION(index) == 0x189) {
            Entity_SetActionMode(entity, U8_AT(D_8009D278, 0x12));
            return 1;
        }

        actor = D_8009D278;
        action_def = *(u8 **)(actor + 0x68);
        next_index = index + 1;

        if (((U32_AT(action_def, 0x10) & 0xC0) == 0xC0) && (D_8009D1DC == 1)) {
            goto process_current_action;
        }

        D_8009D1DC = 0;
        if ((QUEUE_ACTION(next_index) >= 3) || ((u8)next_index == D_8009CE3C)) {
            Entity_SetActionMode(entity, U8_AT(actor, 0x12));
            U32_AT(actor, 0x4C) |= 0x200000;
            D_8009CE38 = U8_AT(action_def, 0x14);
            return 1;
        }

        target_entity = QUEUE_ENTITY(next_index);
        target_core = *(u8 **)target_entity;
        if ((*(int *)(target_core + 0x10) <= 0) || (target_core == 0)) {
            Entity_SetActionMode(entity, U8_AT(actor, 0x12));
            D_8009D1D4++;
            U32_AT(actor, 0x4C) |= 0x200000;
            D_8009D1DC = U32_AT(*(u8 **)(actor + 0x68), 0x10) & 0xF;
            return 1;
        }

        if ((U32_AT(action_def, 0xC) & 0x3FF) != 0) {
            angle = Gte_Atan2(S16_AT(*arg0, 0x26A) - D_8009D27C,
                              Battle_CalcDistToPlayer(*arg0, entity));
            if (angle < -0xAB) {
                dir = 0;
            } else if (angle < 0xE4) {
                dir = 1;
            } else {
                dir = 2;
            }

            Entity_SetActionMode(entity, U8_AT(actor, 0x14 + dir));
            U32_AT(entity, 0x14) = U8_AT(entity, 0xF) << 16;
            U32_AT(entity, 0x98) |= 0x100;
            D_8009CE39 += D_8009CE3A;
            D_8009D1DC = U32_AT(*(u8 **)(D_8009D278 + 0x68), 0x10) & 0xF;
            return 1;
        }

        if (BattleCmd_CommitAmmoAndUpdate((int)action_def) == 0) {
            return 1;
        }
        Entity_SetActionMode(D_8009D254, 0xC);
        Akao_SetPos3D(1, 0, S16_AT(D_8009D254, 0x2A), S16_AT(D_8009D254, 0x2E),
                      S16_AT(D_8009D254, 0x32));
        return 1;

process_current_action:
        if ((U32_AT(action_def, 0xC) & 0x3FF) != 0) {
            target_entity = QUEUE_ENTITY(index);
            target_core = *(u8 **)target_entity;
            if ((*(int *)(target_core + 0x10) <= 0) || (target_core == 0)) {
                Entity_SetActionMode(entity, U8_AT(actor, 0x12));
                U32_AT(actor, 0x4C) |= 0x200000;
                D_8009D1DC = U32_AT(*(u8 **)(actor + 0x68), 0x10) & 0xF;
                return 1;
            }

            D_8009D1DC = D_8009D1DC - 1;
            Battle_FinalizeAttackResult();
            return result;
        }

        if (BattleCmd_CommitAmmoAndUpdate((int)action_def) != 0) {
            Entity_SetActionMode(D_8009D254, 0xC);
            Akao_SetPos3D(1, 0, S16_AT(D_8009D254, 0x2A), S16_AT(D_8009D254, 0x2E),
                          S16_AT(D_8009D254, 0x32));
            return result;
        }

        Entity_SetActionMode(D_8009D254, U8_AT(D_8009D278, 0x12));
        return 1;
    }

    index = D_8009D1D4;
    if (QUEUE_ACTION(index) != 0x189) {
        next_index = index + 1;
        D_8009D1D4 = next_index;
        if (((u8)next_index == D_8009CE3C) && (D_8009D200 >= 0)) {
            actor = D_8009D278;
            action_def = *(u8 **)(actor + 0x68);
            if (S16_AT(action_def, 0x6) != 8) {
                Pm_SendCmd(D_8009D200, 0, 0, 2, 0, 0);
            }
            Pm_SendCmd(D_8009D2FC, 0, 0, 2, 0, 0);
        }
    }

    return result;
}
