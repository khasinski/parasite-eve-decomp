#include "pe1/battle_entity_anim.h"

#define U16_AT(ptr, offset) (*(u16 *)((u8 *)(ptr) + (offset)))

void Battle_StepEntityAnimState(BattleEntity *entity) {
    EnemyCombatant *state;
    BattleEntity *iter;
    u8 color;
    int product;
    int diff;
    int i;
    u8 all_done;

    state = (EnemyCombatant *)entity->core;

    switch (state->deathAnimPhase) {
    case 0:
        D_8009D2A0--;
        state->deathAnimPhase++;
        /* fallthrough */
    case 1:
        if (state->deathPersist != 0) {
            if (U16_AT(entity, 0x16) < state->deathAnimFrame &&
                entity->actionMode == 0 &&
                (s8)state->field04.bytes.field05 < 2) {
                return;
            }

            entity->entityFlags |= 0x100;
            Anim_SetInterpRate(&entity->renderObject, 0x16);
            state->deathFadeStep = 0;
            state->deathAnimPhase++;
            entity->motionX = 0;
            entity->motionY = 0;
            entity->motionZ = 0;
            entity->entityFlags |= 0x1000;
            Akao_Cmd_21(0, state->field08);

            iter = g_FieldActorListHead;
            while (iter != 0) {
                if (iter->core == 0 && iter->parent == entity) {
                    Anim_SetInterpRate(&iter->renderObject, 0x16);
                }
                iter = iter->next;
            }
        } else {
            Akao_Cmd_21(0, state->field08);
            state->deathAnimPhase = 3;
        }
        break;

    case 2:
        color = (-0x80 - (state->deathFadeStep << 4)) & 0xFF;
        Render_FadeEntityColor(&entity->renderObject, color, 0x80, color);
        if (color == 0) {
            entity->renderObject.flags_9C |= 2;
            state->deathFadeStep = 0;
            state->deathAnimPhase++;
        } else {
            if (state->deathFadeStep == 0 && state->deathAssetEnabled != 0) {
                Asset_Find08w(
                    state->deathAssetId,
                    0,
                    entity->renderObject.target_x,
                    entity->renderObject.target_y,
                    entity->renderObject.target_z);
            }
            state->deathFadeStep++;
        }

        iter = g_FieldActorListHead;
        while (iter != 0) {
            if (iter->core == 0 && iter->parent == entity) {
                Render_FadeEntityColor(&iter->renderObject, color, 0x80, color);
                if (color == 0) {
                    iter->renderObject.flags_9C |= 2;
                }
            }
            iter = iter->next;
        }
        break;

    case 3:
        if (entity->renderObject.variant_visible != 0 && state->deathPersist != 0) {
            return;
        }

        entity->entityFlags |= 0x410;
        Battle_SlotFree(entity);

        iter = g_FieldActorListHead;
        while (iter != 0) {
            if (iter->core == 0 && iter->parent == entity) {
                iter->entityFlags |= 0x10;
            }
            iter = iter->next;
        }

        product = state->rewardFactorA * state->rewardFactorB;
        diff = state->rewardBase - product;
        D_8009D304 += state->hpMirror;
        if (diff > 0) {
            D_8009D21C += diff;
        }

        for (i = 0; (u8)i < 10; i++) {
            if (D_800A7FF0[(u8)i].id == 0) {
                D_800A7FF0[(u8)i].id = state->rewardSlotId;
                break;
            }
        }

        if (state->lootItemId != 0) {
            for (i = 0; (u8)i < 10; i++) {
                if (D_800A7FF0[(u8)i].id == 0) {
                    D_800A7FF0[(u8)i].id = (u16)state->lootItemId;
                    if (state->lootItemAux >= 0) {
                        D_800A7FF0[(u8)i].extra = state->lootItemAux;
                    }
                    break;
                }
            }
        }

        if ((s8)D_8009D2A0 != 0) {
            return;
        }

        iter = g_FieldActorListHead;
        all_done = 1;
        while (iter != 0) {
            if (iter != D_8009D254 && iter->core != 0) {
                all_done = 0;
            }
            iter = iter->next;
        }

        if (all_done && (s16)g_ActiveActor->curHP > 0) {
            Battle_StartDeathAnim();
        }
        break;
    }
}
