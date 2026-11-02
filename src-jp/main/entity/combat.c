/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "common.h"
#include "pe1/asset_find.h"
#include "pe1/battle.h"
#include "pe1/battle_runtime.h"
#include "pe1/entity_animation.h"
#include "pe1/game_timers.h"
#include "pe1/inventory.h"
#include "pe1/item_ids.h"
#include "pe1/menu_flags.h"
#include "pe1/player_entity.h"
#include "pe1/render_prim.h"

/* COMMON supplies small-data metadata to MASPSX: these words are reached
 * through $gp, every other extern stays absolute. */
u16 g_BattleHitActionPending;
unsigned char g_AyaStatusEffectTimer;

int rand(void);
s16 Entity_ApplyHitAndSetAnim(void *arg0);

void Entity_ResolveDropTable(void *arg0) {
    Combatant *state;
    EnemyCombatant *entry;
    u16 scale;
    u32 flags;
    u32 word;
    int masked;
    int value;
    int roll;

    state = g_ActiveActor;
    scale = (u16)state->stat20 / 5U;
    entry = *(EnemyCombatant **)arg0;
    flags = state->stateFlags;

    if (flags & 0x1000) {
        scale = (u16)((scale * 3) / 10);
    }
    if (flags & 0x100) {
        scale = (u16)((scale * 3) / 10);
    }

    word = entry->coreFlags;
    if ((int)((word >> 21) & 7) < 3) {
        EnemyActionEffect *action;

        action = entry->effect;
        masked = state->attributes->parameterWord.fields.first;
        if (action->category == 0) {
            action->state = 4;
        } else if (action->category != 1) {
            action->state = 3;
        }
    } else {
        int secondaryParameter = state->attributes->parameterWord.fields.second;
        masked = secondaryParameter;
    }

    value = entry->effect->power - scale - masked;
    roll = rand();

    {
        Combatant *chance_state;
        int chance;

        roll %= 100;
        chance_state = g_ActiveActor;
        chance = ((int)entry->effectChance *
                  (100 - (int)chance_state->attributes->parameterWord.fields.third)) /
                 100;
        if (roll < chance) {
            value = value * 3 / 2;
            chance_state->stateFlags |= 0x8000;
        }
    }

    {
        Combatant *after_state;

        after_state = g_ActiveActor;
        if (!(after_state->stateFlags & 0x200)) {
            if (after_state->atbGauge > 0) {
                value /= 2;
            }
            if (value > 0) {
                EnemyActionEffect *action;

                action = entry->effect;
                if (action->effectType == 0xA) {
                    value /= 2;
                }
                after_state->curHP = after_state->curHP - value;
            }
            if ((s16)g_ActiveActor->curHP != 0) {
                Entity_ApplyHitAndSetAnim(arg0);
                if (!(g_PlayerEntity->flags & 0x100)) {
                    g_BattlePendingEnemySpawn = arg0;
                }
            }
        } else if (value > 0) {
            EnemyActionEffect *action;

            action = entry->effect;
            if (action->effectType == 0xA) {
                value /= 2;
            }
            after_state->exp_or_acc -= value * ((after_state->maxAtk * 4) / (s16)after_state->maxHP);
        }
    }

    if (entry->effect->effectType != 0) {
        Battle_ApplyEnemyAttack(entry);
    }
}

int Battle_CalcRelativeAngle(BattleEntity *arg0, BattleEntity *actor);

static inline FieldActor *CurrentPlayer(void) {
    return g_PlayerEntity;
}

s16 Entity_ApplyHitAndSetAnim(void *arg0) {
    void *saved_arg;
    s32 value;
    FieldActor *switch_actor;
    FieldActor *actor;
    Combatant *state;
    short bucket;
    u32 flags;

    saved_arg = arg0;
    value = 0;
    switch_actor = CurrentPlayer();

    switch (switch_actor->mode) {
    case 6:
    case 8:
    case 10:
    case 12:
    case 13:
    case 14:
    case 15: {
        FieldActor *case_actor;
        u8 mode;
        u8 submode;

        case_actor = CurrentPlayer();
        mode = case_actor->mode;
        g_BattleHitActionMode[0] = mode;
        submode = case_actor->action;
        g_BattleHitActionPending = 1;
        g_BattleHitActionSubmode[0] = submode;
        g_BattleHitActionParam[0] = *(u32 *)&case_actor->anim;
        break;
    }
    case 7:
    case 9:
    case 11: {
        FieldActor *case_actor;
        u8 mode;
        u8 submode;

        case_actor = CurrentPlayer();
        mode = case_actor->mode;
        g_BattleHitActionMode[0] = mode;
        submode = case_actor->action;
        g_BattleHitActionPending = 1;
        g_BattleHitActionSubmode[0] = submode;
        g_BattleHitActionParam[0] = case_actor->action << 16;
        break;
    }
    }

    state = g_ActiveActor;
    if ((state->stateFlags & 0x12000) == 0) {
        value = Battle_CalcRelativeAngle(saved_arg, CurrentPlayer());
        if ((s16)value < 0x200) {
            bucket = 0;
        } else if ((s16)value < 0x600) {
            bucket = 2;
        } else if ((s16)value < 0xA00) {
            bucket = 1;
        } else if ((s16)value < 0xE00) {
            bucket = 3;
        } else {
            bucket = 0;
        }

        Entity_SetActionMode((BattleEntity *)CurrentPlayer(), bucket);

        {
            FieldActor *effect_actor;

            effect_actor = CurrentPlayer();
            Asset_Find08Alt(0x46A, 0, Pe1Fixed_Integer(&effect_actor->pos_x), Pe1Fixed_Integer(&effect_actor->pos_y), Pe1Fixed_Integer(&effect_actor->pos_z));
        }

        {
            FieldActor *flag_actor;

            flag_actor = CurrentPlayer();
            flags = flag_actor->flags;
            actor = flag_actor;
        }
        if (flags & 0x100) {
            actor->flags = flags & ~0x100;
            g_BattleHitActionPending = 2;
        }
    }

    return (s16)value;
}

void Battle_ApplyDamage(s32);
void Battle_SubActionStep(void);
s32 Inv_CountByValue(s32);

void Entity_UpdateTimers(void) {
    s32 status_item_0;
    s32 status_item_1;
    s32 status_item_2;
    s32 status_item_3;
    s16 current_hp;
    s32 accumulator;
    s32 status_group;
    s32 status_group_again;
    s32 consumed_item;
    u8 consumed_item_snapshot;
    s32 hp_threshold;
    s32 max_hp;
    u16 timer46;
    u16 global_timer;
    u16 timer40;
    u16 timer42;
    u16 timer44;
    u8 effect_timer;
    Combatant *combatant;
    s32 *state_flags;

    combatant = g_ActiveActor;
    state_flags = &combatant->stateFlags;
    if (((*state_flags) & AYA_POISON_MASK) == AYA_POISON) {
        Battle_SubActionStep();
        timer40 = g_ActiveActor->statusTimer40 - g_ActiveActor->statusStep3E;
        g_ActiveActor->statusTimer40 = timer40;
        if ((timer40 << 0x10) <= 0) {
            (*state_flags) = ((*state_flags) & ~AYA_POISON_MASK);
        }
    }
    if (((*state_flags) & AYA_CONFUSION_MASK) == AYA_CONFUSION) {
        timer42 = g_ActiveActor->statusTimer42 - g_ActiveActor->statusStep3C;
        g_ActiveActor->statusTimer42 = timer42;
        if ((timer42 << 0x10) <= 0) {
            (*state_flags) = ((*state_flags) & ~AYA_CONFUSION_MASK);
            D_8009CDA8 &= ~FIELD_MOVE_CONFUSED;
        }
    }
    if (((*state_flags) & AYA_DARKNESS_MASK) == AYA_DARKNESS) {
        timer44 = g_ActiveActor->statusTimer44 - g_ActiveActor->statusStep3C;
        g_ActiveActor->statusTimer44 = timer44;
        if ((timer44 << 0x10) <= 0) {
            (*state_flags) = ((*state_flags) & ~AYA_DARKNESS_MASK);
        }
    }
    status_group = (*state_flags) & AYA_STIFFNESS_MASK;
    if ((status_group == AYA_STIFFNESS_SLOW) || (status_group == AYA_STIFFNESS_LOCK)) {
        timer46 = g_ActiveActor->statusTimer46 - g_ActiveActor->statusStep3C;
        g_ActiveActor->statusTimer46 = timer46;
        if ((timer46 << 0x10) <= 0) {
            if (((*state_flags) & AYA_STIFFNESS_MASK) == AYA_STIFFNESS_LOCK && D_8009CD10->actionMode == 0x11) {
                Entity_SetActionMode(D_8009CD10, g_ActiveActor->actionMode12);
            }
            (*state_flags) = ((*state_flags) & ~AYA_STIFFNESS_MASK);
        } else if (((*state_flags) & AYA_STIFFNESS_MASK) == AYA_STIFFNESS_LOCK) {
            D_8009CD10->motionX = 0;
            D_8009CD10->motionY = 0;
            D_8009CD10->motionZ = 0;
        }
    }
    if ((*state_flags) & AYA_HASTE) {
        global_timer = g_ActorEffectFlag100Timer - 1;
        g_ActorEffectFlag100Timer = global_timer;
        if ((global_timer << 0x10) <= 0) {
            (*state_flags) = ((*state_flags) & ~AYA_HASTE);
        }
    }
    if (((*state_flags) & AYA_BARRIER) && g_ActiveActor->exp_or_acc <= 0x10000) {
        g_ActiveActor->exp_or_acc = 0x10000;
        (*state_flags) = ((*state_flags) & ~AYA_BARRIER);
    }
    if ((*state_flags) & AYA_GENE_HEAL) {
        if (g_ActiveDrawSlot != 0) {
            current_hp = (s16) g_ActiveActor->curHP;
            if (current_hp < (s16) g_ActiveActor->maxHP) {
                g_ActiveActor->curHP = (s16) (current_hp + 1);
                g_ActiveActor->hpMirror = (u16) (g_ActiveActor->hpMirror + 1);
            }
        }
        accumulator = g_ActiveActor->exp_or_acc -
                    (g_ActiveActor->maxAtk /
                     (g_ActiveActor->field04.fieldId04 * 0x1E));
        g_ActiveActor->exp_or_acc = accumulator;
        if (accumulator <= 0x10000) {
            g_ActiveActor->exp_or_acc = 0x10000;
            (*state_flags) = ((*state_flags) & ~AYA_GENE_HEAL);
        }
    }
    if ((*state_flags) & 0x01000000) {
        effect_timer = g_AyaStatusEffectTimer - 1;
        g_AyaStatusEffectTimer = effect_timer;
        if ((effect_timer << 0x18) <= 0) {
            (*state_flags) = ((*state_flags) & 0xFEFFFFFF);
        }
    }
    if (g_ActiveActor->attributes->effectFlags & EFFECT_AUTO_MEDICINE) {
        consumed_item = 0;
        if ((s16) g_ActiveActor->curHP > 0) {
            do {
                consumed_item_snapshot = consumed_item & 0xFF;
                max_hp = (s16) g_ActiveActor->maxHP;
                hp_threshold = max_hp / 5;
                if ((s16) g_ActiveActor->curHP < hp_threshold) {
                    if (Inv_CountByValue(ITEM_FULL_RECOVER) != 0) {
                        consumed_item = 1;
                        Battle_ApplyDamage(ITEM_FULL_RECOVER);
                        Inv_FindItemById(ITEM_FULL_RECOVER);
                        continue;
                    }
                    if (Inv_CountByValue(ITEM_MEDICINE_4) != 0) {
                        consumed_item = 1;
                        Battle_ApplyDamage(ITEM_MEDICINE_4);
                        Inv_FindItemById(ITEM_MEDICINE_4);
                        continue;
                    }
                    if (Inv_CountByValue(ITEM_MEDICINE_3) != 0) {
                        consumed_item = 1;
                        Battle_ApplyDamage(ITEM_MEDICINE_3);
                        Inv_FindItemById(ITEM_MEDICINE_3);
                        continue;
                    }
                    if (Inv_CountByValue(ITEM_MEDICINE_2) != 0) {
                        consumed_item = 1;
                        Battle_ApplyDamage(ITEM_MEDICINE_2);
                        Inv_FindItemById(ITEM_MEDICINE_2);
                        continue;
                    }
                    consumed_item_snapshot = consumed_item & 0xFF;
                    if (Inv_CountByValue(ITEM_MEDICINE_1) != 0) {
                        consumed_item = 1;
                        Battle_ApplyDamage(ITEM_MEDICINE_1);
                        Inv_FindItemById(ITEM_MEDICINE_1);
                        continue;
                    }
                }
                break;
            } while (1);
            if (consumed_item_snapshot != 0) {
                Scene_LoadRoomAssets(0x56, D_8009CD10);
                Asset_Find08Alt(0x4B4, 0, D_8009CD10->posX.parts.integer, D_8009CD10->posY.parts.integer, D_8009CD10->posZ.parts.integer);
            }
        }
    }
    if (g_ActiveActor->attributes->effectFlags & EFFECT_AUTO_CURE) {
        if (((*state_flags) & AYA_POISON_MASK) == AYA_POISON) {
            if (Inv_CountByValue(ITEM_CURE_P) != 0) {
                Battle_ApplyDamage(ITEM_CURE_P);
                status_item_0 = ITEM_CURE_P;
                Inv_FindItemById(status_item_0);
                Scene_LoadRoomAssets(0x57, D_8009CD10);
                Asset_Find08Alt(0x4B5, 0, D_8009CD10->posX.parts.integer, D_8009CD10->posY.parts.integer, D_8009CD10->posZ.parts.integer);
            } else if (Inv_CountByValue(ITEM_FULL_CURE) != 0) {
                Battle_ApplyDamage(ITEM_FULL_CURE);
                status_item_0 = ITEM_FULL_CURE;
                Inv_FindItemById(status_item_0);
                Scene_LoadRoomAssets(0x57, D_8009CD10);
                Asset_Find08Alt(0x4B5, 0, D_8009CD10->posX.parts.integer, D_8009CD10->posY.parts.integer, D_8009CD10->posZ.parts.integer);
            }
        }
        if (((*state_flags) & AYA_CONFUSION_MASK) == AYA_CONFUSION) {
            if (Inv_CountByValue(ITEM_CURE_C) != 0) {
                Battle_ApplyDamage(ITEM_CURE_C);
                status_item_1 = ITEM_CURE_C;
                Inv_FindItemById(status_item_1);
                Scene_LoadRoomAssets(0x57, D_8009CD10);
                Asset_Find08Alt(0x4B5, 0, D_8009CD10->posX.parts.integer, D_8009CD10->posY.parts.integer, D_8009CD10->posZ.parts.integer);
            } else if (Inv_CountByValue(ITEM_FULL_CURE) != 0) {
                Battle_ApplyDamage(ITEM_FULL_CURE);
                status_item_1 = ITEM_FULL_CURE;
                Inv_FindItemById(status_item_1);
                Scene_LoadRoomAssets(0x57, D_8009CD10);
                Asset_Find08Alt(0x4B5, 0, D_8009CD10->posX.parts.integer, D_8009CD10->posY.parts.integer, D_8009CD10->posZ.parts.integer);
            }
        }
        if (((*state_flags) & AYA_DARKNESS_MASK) == AYA_DARKNESS) {
            if (Inv_CountByValue(ITEM_CURE_D) != 0) {
                Battle_ApplyDamage(ITEM_CURE_D);
                status_item_2 = ITEM_CURE_D;
                Inv_FindItemById(status_item_2);
                Scene_LoadRoomAssets(0x57, D_8009CD10);
                Asset_Find08Alt(0x4B5, 0, D_8009CD10->posX.parts.integer, D_8009CD10->posY.parts.integer, D_8009CD10->posZ.parts.integer);
            } else if (Inv_CountByValue(ITEM_FULL_CURE) != 0) {
                Battle_ApplyDamage(ITEM_FULL_CURE);
                status_item_2 = ITEM_FULL_CURE;
                Inv_FindItemById(status_item_2);
                Scene_LoadRoomAssets(0x57, D_8009CD10);
                Asset_Find08Alt(0x4B5, 0, D_8009CD10->posX.parts.integer, D_8009CD10->posY.parts.integer, D_8009CD10->posZ.parts.integer);
            }
        }
        status_group_again = (*state_flags) & AYA_STIFFNESS_MASK;
        if ((status_group_again == AYA_STIFFNESS_SLOW) || (status_group_again == AYA_STIFFNESS_LOCK)) {
            if (Inv_CountByValue(ITEM_CURE_M) != 0) {
                Battle_ApplyDamage(ITEM_CURE_M);
                status_item_3 = ITEM_CURE_M;
                Inv_FindItemById(status_item_3);
                Scene_LoadRoomAssets(0x57, D_8009CD10);
                Asset_Find08Alt(0x4B5, 0, D_8009CD10->posX.parts.integer, D_8009CD10->posY.parts.integer, D_8009CD10->posZ.parts.integer);
            } else if (Inv_CountByValue(ITEM_FULL_CURE) != 0) {
                Battle_ApplyDamage(ITEM_FULL_CURE);
                status_item_3 = ITEM_FULL_CURE;
                Inv_FindItemById(status_item_3);
                Scene_LoadRoomAssets(0x57, D_8009CD10);
                Asset_Find08Alt(0x4B5, 0, D_8009CD10->posX.parts.integer, D_8009CD10->posY.parts.integer, D_8009CD10->posZ.parts.integer);
            }
        }
        if (((*state_flags) & 0x1000) && (Inv_CountByValue(ITEM_FULL_CURE) != 0)) {
            Battle_ApplyDamage(ITEM_FULL_CURE);
            Inv_FindItemById(ITEM_FULL_CURE);
            Scene_LoadRoomAssets(0x57, D_8009CD10);
            Asset_Find08Alt(0x4B5, 0, D_8009CD10->posX.parts.integer, D_8009CD10->posY.parts.integer, D_8009CD10->posZ.parts.integer);
        }
    }
}
