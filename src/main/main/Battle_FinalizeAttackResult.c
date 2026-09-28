/* CC1_FLAGS: -fno-force-mem -G8 */
/* MASPSX_FLAGS: -G8 */

#include "pe1/battle_runtime.h"
#include "pe1/psyq_nop.h"

typedef struct BattleGlobalSlot {
    char storage[16];
} BattleGlobalSlot;

extern BattleGlobalSlot D_8009D278_first __asm__("D_8009D278");
extern BattleGlobalSlot D_8009D278_second __asm__("D_8009D278");
extern BattleGlobalSlot D_8009D254_large __asm__("D_8009D254");

#define ACTIVE_COMBATANT_FIRST (*(Combatant **)&D_8009D278_first)
#define ACTIVE_COMBATANT_SECOND (*(Combatant **)&D_8009D278_second)
#define PLAYER_ENTITY (*(BattleEntity **)&D_8009D254_large)

void Battle_FinalizeAttackResult(void)
{
    Combatant *combatant;
    BattleAction *action;
    u32 attack_word;

    combatant = ACTIVE_COMBATANT_FIRST;
    action = combatant->action;

    if (action->actionCode.actionId == 8) {
        BattleEntity *player;

        D_8009D294 = 1;
        player = PLAYER_ENTITY;
        Asset_Find08Alt(0x46C, 0, player->posX.parts.integer,
                        player->posY.parts.integer, player->posZ.parts.integer);
        return;
    }

    attack_word = action->attackWord;
    if ((attack_word & 0x3FF) != 0) {
        D_8009D294 = 1;
        if (!(combatant->stateFlags & 0x100000)) {
            BattleEntity *player;

            Pm_SendCmd(D_8009D200, 0, 0, 1, 0, 0);
            player = PLAYER_ENTITY;
            Akao_SetPos3D(0, 0, player->posX.parts.integer,
                          player->posY.parts.integer, player->posZ.parts.integer);
        }

        {
            Combatant *next_combatant;
            BattleAction *next_action;
            u32 next_attack_word;

            next_combatant = ACTIVE_COMBATANT_SECOND;
            next_action = next_combatant->action;
            next_attack_word = next_action->attackWord;
            next_action->attackWord = (next_attack_word & ~0x3FF) |
                                      (((next_attack_word & 0x3FF) - 1) & 0x3FF);
        }
        return;
    }

    {
        BattleEntity *player;

        D_8009D294 = 0;
        player = PLAYER_ENTITY;
        Asset_Find08Alt(0x46E, 0, player->posX.parts.integer,
                        player->posY.parts.integer, player->posZ.parts.integer);
    }
}


extern char battle_entity_large[16] __asm__("D_8009D254");
extern char battle_entity_after_set[16] __asm__("D_8009D254");
extern char battle_entity_alias_0[16] __asm__("D_8009D254");
extern char battle_entity_alias_1[16] __asm__("D_8009D254");
extern char battle_entity_alias_2[16] __asm__("D_8009D254");
extern char battle_entity_alias_3[16] __asm__("D_8009D254");
extern char battle_entity_alias_4[16] __asm__("D_8009D254");
extern char battle_entity_alias_5[16] __asm__("D_8009D254");
extern char battle_entity_alias_6[16] __asm__("D_8009D254");
extern char battle_entity_alias_7[16] __asm__("D_8009D254");

extern char battle_actor_large[16] __asm__("D_8009D278");
extern char battle_sfx_slot_large[16] __asm__("D_8009D2FC");

extern u8 D_8009D274, D_8009CE39, D_8009CE3A;
extern s16 D_8009D27C;
extern int D_8009D2FC;
int Gte_Atan2(int y, int x);
int BattleCmd_CommitAmmoAndUpdate(void);

#define D_8009D254 (*(BattleEntity **)battle_entity_large)
#define D_8009D278 (*(Combatant **)battle_actor_large)
#define D_8009D2FC (*(int *)battle_sfx_slot_large)

extern char battle_actor_group_0[16] __asm__("D_8009D278");
extern char battle_actor_group_1[16] __asm__("D_8009D278");
extern char battle_actor_group_2[16] __asm__("D_8009D278");
extern char battle_actor_group_3[16] __asm__("D_8009D278");
extern char battle_actor_group_4[16] __asm__("D_8009D278");
extern char battle_actor_group_5[16] __asm__("D_8009D278");
extern char battle_actor_group_6[16] __asm__("D_8009D278");
extern char battle_actor_group_7[16] __asm__("D_8009D278");
extern char battle_actor_group_8[16] __asm__("D_8009D278");
extern char battle_actor_group_11[16] __asm__("D_8009D278");
extern char battle_actor_group_12[16] __asm__("D_8009D278");
extern char battle_actor_group_13[16] __asm__("D_8009D278");
extern char battle_actor_group_16[16] __asm__("D_8009D278");
extern char battle_actor_group_17[16] __asm__("D_8009D278");
int Battle_StepCharacterAction(BattleInitSlot *slot)
{
    BattleInitSlot * turnSlot = slot;
    int result = 0;
    register BattleAction *activeAction asm("$4");
    Combatant * active;
    BattleEntity * caseEntity;
    BattleEntity *idleEntity;
    Combatant *idleActor;
    int retryCount;
    int indexB;
    register int nextIndexB asm("$3");
    int indexAlt;
    int indexDefault;
    int angle;
    int direction;

    switch (D_8009D254->actionMode) {
    case 6:
    case 8:
    case 10: {
        BattleEntity *current = D_8009D254;
        if (current->animLastFrame != ((u16 *)&current->animFrame)[1])
            goto finish;
        if (D_8009CE38[0] != 0) {
            D_8009CE38[0]--;
        } else if (D_8009CE39 != 0) {
            D_8009CE39--;
        } else {
            goto ready;
        }
        current->entityFlags |= 0x100;
        goto finish;
    ready:
        }
        D_8009D274 = 0;
        { u8 animMode = (*(Combatant **)battle_actor_group_0)->action->animMode[1]; PE1_NOP(); D_8009CE39 = animMode; }
        asm volatile("");
        { register int y asm("$16") = *(s16 *)((u8 *)turnSlot->actor + 0x26A); angle = y - D_8009D27C; }
        { register int firstFacing asm("$4") = Gte_Atan2(angle, Battle_CalcDistToPlayer(turnSlot->actor, D_8009D254));
        if (firstFacing < -0xAB) direction = 0; else { register int cmp asm("$2") = firstFacing < 0xE4; if (cmp) direction = 1; else direction = 2; } }
        Entity_SetActionMode(D_8009D254, *((u8 *)(*(Combatant **)battle_actor_group_1) + 0x17 + direction));
        (*(BattleEntity **)battle_entity_after_set)->entityFlags &= ~0x100;
        D_8009D1DC = (*(volatile u32 *)&(*(Combatant **)battle_actor_group_2)->action->turnWord) & 0xF;
        Battle_FinalizeAttackResult();
        goto finish;

    case 7:
    case 9:
    case 11:
        caseEntity = D_8009D254;
        if (caseEntity->animLastFrame != caseEntity->animPrev.parts.integer)
            goto finish;
        indexB = D_8009D1D4;
        if (D_800BE830[indexB].field04 == 0x189) {
            idleActor = (*(Combatant **)battle_actor_group_3);
            idleEntity = D_8009D254;
            goto set_idle;
        }
        { active = (*(Combatant **)battle_actor_group_4);
        activeAction = active->action;
        if ((activeAction->turnWord & 0xC0) == 0xC0) {
            int pending = D_8009D1DC;
            int narrow;
            register int one asm("$2") = 1;
            asm volatile("" : "=r"(pending) : "0"(pending));
            narrow = (u8)pending;
            retryCount = pending;
            if (narrow != one) goto alternate;
        }
        nextIndexB = indexB + 1;
        { register int offset asm("$5") = nextIndexB << 3;
        { int nextCode = D_800BE830[nextIndexB].field04;
        D_8009D1DC = 0;
        if (nextCode >= 3 || nextIndexB == D_8009CE3C) {
            Entity_SetActionMode(D_8009D254, active->actionMode12);
            (*(Combatant **)battle_actor_group_5)->stateFlags |= 0x200000;
            { u8 animMode = (*(Combatant **)battle_actor_group_5)->action->animMode[0]; PE1_NOP(); D_8009CE38[0] = animMode; }
            result = 1; goto finish;
        }
        }
        { EnemyCombatant *enemy = (EnemyCombatant *)((BattleInitSlot *)((u8 *)D_800BE830 + offset))->actor->core; if (!(enemy->hpAlive > 0 && enemy)) {
            Entity_SetActionMode(D_8009D254, active->actionMode12);
            D_8009D1D4++;
            (*(Combatant **)battle_actor_group_6)->stateFlags |= 0x200000;
            goto action_done;
        }
        }
        }
        }
        { register BattleAction *attackAction asm("$4") = active->action;
        if (attackAction->attackWord & 0x3FF) {
            { register int y asm("$16") = *(s16 *)((u8 *)turnSlot->actor + 0x26A); angle = y - D_8009D27C; }
            { register int facing asm("$4") = Gte_Atan2(angle, Battle_CalcDistToPlayer(turnSlot->actor, D_8009D254));
        if (facing < -0xAB) direction = 0; else { register int cmp asm("$2") = facing < 0xE4; if (cmp) direction = 1; else direction = 2; } }
            Entity_SetActionMode((*(BattleEntity **)battle_entity_alias_7), *((u8 *)(*(Combatant **)battle_actor_group_7) + 0x14 + direction));
            result = 1;
            { register Combatant *postActor asm("$4") = (*(Combatant **)battle_actor_group_8);
              { register BattleEntity *postEntity asm("$3") = (*(BattleEntity **)battle_entity_alias_0);
                postEntity->animFrame = (u32)(*(BattleEntity **)battle_entity_alias_0)->animLastFrame << 16;
                (*(BattleEntity **)battle_entity_alias_0)->entityFlags |= 0x100; }
              { register BattleAction *postAction asm("$3") = postActor->action;
                { u32 turnWord = postAction->turnWord;
                D_8009CE39 += D_8009CE3A;
                D_8009D1DC = turnWord & 0xF; } } }
            goto finish;
        }
        }
        if (BattleCmd_CommitAmmoAndUpdate()) {
            Entity_SetActionMode((*(BattleEntity **)battle_entity_alias_1), 0xC);
            Akao_SetPos3D(1, 0, (*(BattleEntity **)battle_entity_alias_2)->posX.parts.integer,
                          (*(BattleEntity **)battle_entity_alias_2)->posY.parts.integer, (*(BattleEntity **)battle_entity_alias_2)->posZ.parts.integer);
        action_done:
            result = 1; asm volatile(""); goto finish;
        }
        result = 1; goto finish;

    alternate:
        if (activeAction->attackWord & 0x3FF) {
            indexAlt = D_8009D1D4;
            {
                EnemyCombatant *enemy = (EnemyCombatant *)D_800BE830[indexAlt].actor->core;
                int nextRetry;
                if (enemy->hpAlive <= 0) goto target_dead;
asm volatile("" : : "r"(enemy));
                nextRetry = retryCount - 1;
                if (enemy) goto target_alive;
            target_dead:
            Entity_SetActionMode(caseEntity, active->actionMode12);
            (*(Combatant **)battle_actor_group_13)->stateFlags |= 0x200000;
            D_8009D1DC = (*(volatile u32 *)&(*(Combatant **)battle_actor_group_13)->action->turnWord) & 0xF;
            result = 1; goto finish;
            target_alive:
            D_8009D1DC = nextRetry;
            Battle_FinalizeAttackResult();
            goto finish;
            }
        }
        if (BattleCmd_CommitAmmoAndUpdate()) {
            Entity_SetActionMode((*(BattleEntity **)battle_entity_alias_4), 0xC);
            Akao_SetPos3D(1, 0, (*(BattleEntity **)battle_entity_alias_5)->posX.parts.integer,
                          (*(BattleEntity **)battle_entity_alias_5)->posY.parts.integer, (*(BattleEntity **)battle_entity_alias_5)->posZ.parts.integer);
            goto finish;
        }
        idleActor = (*(Combatant **)battle_actor_group_16);
        idleEntity = (*(BattleEntity **)battle_entity_alias_6);
    set_idle:
        result = 1;
        Entity_SetActionMode(idleEntity, idleActor->actionMode12);
        goto finish;
    default:
        indexDefault = D_8009D1D4;
        if (D_800BE830[indexDefault].field04 == 0x189)
            goto finish;
        D_8009D1D4 = indexDefault + 1;
        if ((u8)(indexDefault + 1) != D_8009CE3C)
            goto finish;
        if (D_8009D200 < 0)
            goto finish;
        if ((*(Combatant **)battle_actor_group_17)->action->actionCode.actionId != 8)
            Pm_SendCmd(D_8009D200, 0, 0, 2, 0, 0);
        Pm_SendCmd(D_8009D2FC, 0, 0, 2, 0, 0);
        goto finish;
    }
finish:
    return result;
}
