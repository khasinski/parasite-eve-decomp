/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "pe1/battle_runtime.h"

extern struct { BattleEntity *value; char pad[12]; } D_8009D254_absolute __asm__("D_8009D254");
extern struct { Combatant *value; char pad[12]; } D_8009D278_absolute __asm__("D_8009D278");
extern struct { s32 value; char pad[12]; } D_8009D2FC_absolute __asm__("D_8009D2FC");
#define PLAYER (D_8009D254_absolute.value)
#define ACTIVE (D_8009D278_absolute.value)
#define SECOND_SOUND (D_8009D2FC_absolute.value)

extern s16 D_8009D27C;
extern volatile u8 D_8009CE39, D_8009CE3B;
int Battle_CalcDistToPlayer(BattleEntity *actor, BattleEntity *player);
s16 Battle_CalcAngleToTarget(RenderObjectEntity *object, void *target);
int Gte_Atan2(int y, int x);
int Battle_StepCharacterAction(BattleInitSlot *slot);

#define CORE_HEALTH(entity) (*(s32 *)((u8 *)(entity)->core + 0x10))

void Battle_UpdatePlayerTurn(void)
{
    BattleInitSlot *slot = &D_800BE830[D_8009D1D4];
    register BattleEntity *player asm("$5") = PLAYER;
    BattleEntity *actor;
    Combatant *active;
    int angle;
    int category;
    u8 step;
    int diff;

    if (player->actionMode != 12) {
        active = ACTIVE;
        if (active->stateFlags & 0x200000) {
            BattleEntity *target = slot->actor;
            if (target->core != 0 && CORE_HEALTH(target) > 0) {
                diff = target->renderObject.target_y;
                diff -= D_8009D27C;
                angle = Gte_Atan2(diff, Battle_CalcDistToPlayer(target, player));
                if (angle < -0xAB) category = 0;
                else if (angle < 0xE4) category = 1;
                else category = 2;
                Entity_SetActionMode(PLAYER, ((u8 *)ACTIVE)[0x14 + category]);
                ACTIVE->stateFlags &= ~0x200000;
            }
        }

        {
            BattleEntity *angle_actor = slot->actor;
            PLAYER->facingAngle = Battle_CalcAngleToTarget(&angle_actor->renderObject, &PLAYER->posX);
            step = Battle_StepCharacterAction(slot);
            if (step == 0) return;
            if (step != 1) return;
        }

        {
            if (slot->actor != D_800BE830[D_8009D1D4 + 1].actor ||
                CORE_HEALTH(slot->actor) <= 0) {
                u8 count = D_8009CE39;
                u8 delta = D_8009CE3B;
                register Combatant *effect asm("$4");
                asm volatile("" : : "r"(count), "r"(delta));
                effect = ACTIVE;
                D_8009CE39 = count + delta;
                asm volatile("" : : "r"(effect) : "memory");
                if (effect->action->attackWord & 0x3FF)
                    Battle_StartEnemyAttackEffect(slot->actor);
            }
        }

        D_8009D1D4++;
        if (D_8009D1D4 != D_8009CE3C) return;
        if (ACTIVE->action->actionCode.actionId != 8)
            Pm_SendCmd(D_8009D200, 0, 0, 2, 0, 0);
        {
            int sound = SECOND_SOUND;
            if (sound != -1)
                Pm_SendCmd(sound, 0, 0, 2, 0, 0);
        }
        return;
    }

    if (player->animLastFrame != player->animPrev.parts.integer) return;
    actor = slot->actor;
    diff = actor->renderObject.target_y;
    diff -= D_8009D27C;
    angle = Gte_Atan2(diff, Battle_CalcDistToPlayer(actor, player));
    if (angle < -0xAB) category = 0;
    else if (angle < 0xE4) category = 1;
    else category = 2;
    Entity_SetActionMode(PLAYER, ((u8 *)ACTIVE)[0x14 + category]);
}

#include "common.h"
#include "pe1/battle.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

extern struct { char _[16]; } D_8009D254_a __asm__("D_8009D254");
extern struct { char _[16]; } D_8009D254_b __asm__("D_8009D254");
extern struct { char _[16]; } D_8009D254_c __asm__("D_8009D254");
extern struct { char _[16]; } D_8009D254_d __asm__("D_8009D254");
extern struct { char _[16]; } D_8009D254_e __asm__("D_8009D254");
extern struct { char _[16]; } D_8009D278_a __asm__("D_8009D278");
extern struct { char _[16]; } D_8009D278_b __asm__("D_8009D278");
extern struct { char _[16]; } D_8009D278_c __asm__("D_8009D278");
extern struct { char _[16]; } D_8009D278_d __asm__("D_8009D278");
extern BattleGameStateWindow D_8009D1A0_r0 __asm__("D_8009D1A0");
extern BattleGameStateWindow D_8009D1A0_w0 __asm__("D_8009D1A0");
extern BattleGameStateWindow D_8009D1A0_r1 __asm__("D_8009D1A0");
extern BattleGameStateWindow D_8009D1A0_w1 __asm__("D_8009D1A0");
extern u8 D_8009D1D4;
extern u16 D_800BE834[];

#define D254_A (*(BattleEntity **)&D_8009D254_a)
#define D254_B (*(BattleEntity **)&D_8009D254_b)
#define D254_C (*(BattleEntity **)&D_8009D254_c)
#define D254_D (*(BattleEntity **)&D_8009D254_d)
#define D254_E (*(BattleEntity **)&D_8009D254_e)
#define D278_A (*(Combatant **)&D_8009D278_a)
#define D278_B (*(Combatant **)&D_8009D278_b)
#define D278_C (*(Combatant **)&D_8009D278_c)
#define D278_D (*(Combatant **)&D_8009D278_d)
#define D1A0_R0 (D_8009D1A0_r0.flags)
#define D1A0_W0 (D_8009D1A0_w0.flags)
#define D1A0_R1 (D_8009D1A0_r1.flags)
#define D1A0_W1 (D_8009D1A0_w1.flags)

#define U8_AT(ptr, offset) (*(u8 *)((u8 *)(ptr) + (offset)))
#define S16_AT(ptr, offset) (*(s16 *)((u8 *)(ptr) + (offset)))
#define U16_AT(ptr, offset) (*(u16 *)((u8 *)(ptr) + (offset)))
#define U32_AT(ptr, offset) (*(u32 *)((u8 *)(ptr) + (offset)))
#define ACTION_AT(index) (*(u16 *)((u8 *)D_800BE834 + ((u8)(index) << 3)))

void Battle_ApplyDamage(int action);

void Battle_ApplyPlayerHit(void) {
    u32 committed;
    register int mask asm("$5");
    register unsigned int index asm("$3");
    Combatant *actor;
    int flags;
    u16 action;

    committed = 0x200000;
    if (D278_A->stateFlags & committed) {
        Entity_SetActionMode(D254_A, 0xE);
        Asset_Find08Alt(0x4B3, 0, D254_B->posX.parts.integer,
                        D254_B->posY.parts.integer, D254_B->posZ.parts.integer);

        mask = ~0x200000;
        actor = D278_B;
        index = actor->stateFlags;
        flags = D1A0_R0;
        D1A0_W0 = flags | 0x100;
        actor->stateFlags = index & mask;
    }

    if (D254_C->animLastFrame == D254_C->animPrev.parts.integer) {
        Entity_SetActionMode(D254_C, D278_C->actionMode12);
        actor = D278_D;
        index = D_8009D1D4;
        actor->stateFlags |= committed;

        Battle_ApplyDamage((s16)ACTION_AT(index) - 3);
        Scene_LoadRoomAssets(0x55, D254_D);

        index = D_8009D1D4;
        index++;
        action = ACTION_AT(index);

        D_8009D1D4 = index;

        if ((unsigned int)(action - 3) >= 0x194 || D254_E->actionMode < 4) {
            D1A0_W1 = D1A0_R1 & ~0x100;
        }
    }
}
