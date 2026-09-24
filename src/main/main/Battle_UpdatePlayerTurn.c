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
