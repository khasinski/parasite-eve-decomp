/* CC1_FLAGS: -g3 -G1 */
/* MASPSX_FLAGS: --expand-div -G1 */

#include "common.h"
#include "pe1/battle_runtime.h"

void Battle_ApplyDamage(s32);
void Battle_SubActionStep(void);
s32 Inv_CountByValue(s32);
void Inv_FindItemById(s32);
extern s32 D_8009CDDC;
extern u8 D_8009CE34;
extern u16 D_8009D228;

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

    combatant = D_8009D278;
    state_flags = &combatant->stateFlags;
    if (((*state_flags) & 3) == 1) {
        Battle_SubActionStep();
        timer40 = D_8009D278->statusTimer40 - D_8009D278->statusStep3E;
        D_8009D278->statusTimer40 = timer40;
        if ((timer40 << 0x10) <= 0) {
            (*state_flags) = (s32) ((*state_flags) & ~3);
        }
    }
    if (((*state_flags) & 0xC) == 4) {
        timer42 = D_8009D278->statusTimer42 - D_8009D278->statusStep3C;
        D_8009D278->statusTimer42 = timer42;
        if ((timer42 << 0x10) <= 0) {
            (*state_flags) = (s32) ((*state_flags) & ~0xC);
            D_8009D2E8 &= ~0x10;
        }
    }
    if (((*state_flags) & 0x30) == 0x10) {
        timer44 = D_8009D278->statusTimer44 - D_8009D278->statusStep3C;
        D_8009D278->statusTimer44 = timer44;
        if ((timer44 << 0x10) <= 0) {
            (*state_flags) = (s32) ((*state_flags) & ~0x30);
        }
    }
    status_group = (*state_flags) & 0xC0;
    if ((status_group == 0x40) || (status_group == 0x80)) {
        timer46 = D_8009D278->statusTimer46 - D_8009D278->statusStep3C;
        D_8009D278->statusTimer46 = timer46;
        if ((timer46 << 0x10) <= 0) {
            if (((*state_flags) & 0xC0) == 0x80 && D_8009D254->actionMode == 0x11) {
                Entity_SetActionMode(D_8009D254, D_8009D278->actionMode12);
            }
            (*state_flags) = (s32) ((*state_flags) & ~0xC0);
        } else if (((*state_flags) & 0xC0) == 0x80) {
            D_8009D254->motionX = 0;
            D_8009D254->motionY = 0;
            D_8009D254->motionZ = 0;
        }
    }
    if ((*state_flags) & 0x100) {
        global_timer = D_8009D228 - 1;
        D_8009D228 = global_timer;
        if ((global_timer << 0x10) <= 0) {
            (*state_flags) = (s32) ((*state_flags) & ~0x100);
        }
    }
    if (((*state_flags) & 0x200) && D_8009D278->exp_or_acc <= 0x10000) {
        D_8009D278->exp_or_acc = 0x10000;
        (*state_flags) = (s32) ((*state_flags) & ~0x200);
    }
    if ((*state_flags) & 0x400) {
        if (D_8009CDDC != 0) {
            current_hp = (s16) D_8009D278->curHP;
            if (current_hp < (s16) D_8009D278->maxHP) {
                D_8009D278->curHP = (s16) (current_hp + 1);
                D_8009D278->hpMirror = (u16) (D_8009D278->hpMirror + 1);
            }
        }
        accumulator = D_8009D278->exp_or_acc -
                    (D_8009D278->maxAtk /
                     (D_8009D278->field04.fieldId04 * 0x1E));
        D_8009D278->exp_or_acc = accumulator;
        if (accumulator <= 0x10000) {
            D_8009D278->exp_or_acc = 0x10000;
            (*state_flags) = (s32) ((*state_flags) & ~0x400);
        }
    }
    if ((*state_flags) & 0x01000000) {
        effect_timer = D_8009CE34 - 1;
        D_8009CE34 = effect_timer;
        if ((effect_timer << 0x18) <= 0) {
            (*state_flags) = (s32) ((*state_flags) & 0xFEFFFFFF);
        }
    }
    if (D_8009D278->attributes->effectFlags & 0x4000) {
        consumed_item = 0;
        if ((s16) D_8009D278->curHP > 0) {
            do {
                consumed_item_snapshot = consumed_item & 0xFF;
                max_hp = (s16) D_8009D278->maxHP;
                hp_threshold = max_hp / 5;
                if ((s16) D_8009D278->curHP < hp_threshold) {
                    if (Inv_CountByValue(0xA) != 0) {
                        consumed_item = 1;
                        Battle_ApplyDamage(0xA);
                        Inv_FindItemById(0xA);
                        continue;
                    }
                    if (Inv_CountByValue(9) != 0) {
                        consumed_item = 1;
                        Battle_ApplyDamage(9);
                        Inv_FindItemById(9);
                        continue;
                    }
                    if (Inv_CountByValue(8) != 0) {
                        consumed_item = 1;
                        Battle_ApplyDamage(8);
                        Inv_FindItemById(8);
                        continue;
                    }
                    if (Inv_CountByValue(7) != 0) {
                        consumed_item = 1;
                        Battle_ApplyDamage(7);
                        Inv_FindItemById(7);
                        continue;
                    }
                    consumed_item_snapshot = consumed_item & 0xFF;
                    if (Inv_CountByValue(6) != 0) {
                        consumed_item = 1;
                        Battle_ApplyDamage(6);
                        Inv_FindItemById(6);
                        continue;
                    }
                }
                break;
            } while (1);
            if (consumed_item_snapshot != 0) {
                Scene_LoadRoomAssets(0x56, D_8009D254);
                Asset_Find08Alt(0x4B4, 0, D_8009D254->posX.parts.integer, D_8009D254->posY.parts.integer, (s32) D_8009D254->posZ.parts.integer);
            }
        }
    }
    if (D_8009D278->attributes->effectFlags & 0x8000) {
        if (((*state_flags) & 3) == 1) {
            if (Inv_CountByValue(0xD) != 0) {
                Battle_ApplyDamage(0xD);
                status_item_0 = 0xD;
                goto consume_status_group0;
            }
            if (Inv_CountByValue(0x11) != 0) {
                Battle_ApplyDamage(0x11);
                status_item_0 = 0x11;
consume_status_group0:
                Inv_FindItemById(status_item_0);
                Scene_LoadRoomAssets(0x57, D_8009D254);
                Asset_Find08Alt(0x4B5, 0, D_8009D254->posX.parts.integer, D_8009D254->posY.parts.integer, (s32) D_8009D254->posZ.parts.integer);
            }
        }
        if (((*state_flags) & 0xC) == 4) {
            if (Inv_CountByValue(0xF) != 0) {
                Battle_ApplyDamage(0xF);
                status_item_1 = 0xF;
                goto consume_status_group1;
            }
            if (Inv_CountByValue(0x11) != 0) {
                Battle_ApplyDamage(0x11);
                status_item_1 = 0x11;
consume_status_group1:
                Inv_FindItemById(status_item_1);
                Scene_LoadRoomAssets(0x57, D_8009D254);
                Asset_Find08Alt(0x4B5, 0, D_8009D254->posX.parts.integer, D_8009D254->posY.parts.integer, (s32) D_8009D254->posZ.parts.integer);
            }
        }
        if (((*state_flags) & 0x30) == 0x10) {
            if (Inv_CountByValue(0xE) != 0) {
                Battle_ApplyDamage(0xE);
                status_item_2 = 0xE;
                goto consume_status_group2;
            }
            if (Inv_CountByValue(0x11) != 0) {
                Battle_ApplyDamage(0x11);
                status_item_2 = 0x11;
consume_status_group2:
                Inv_FindItemById(status_item_2);
                Scene_LoadRoomAssets(0x57, D_8009D254);
                Asset_Find08Alt(0x4B5, 0, D_8009D254->posX.parts.integer, D_8009D254->posY.parts.integer, (s32) D_8009D254->posZ.parts.integer);
            }
        }
        status_group_again = (*state_flags) & 0xC0;
        if ((status_group_again == 0x40) || (status_group_again == 0x80)) {
            if (Inv_CountByValue(0x10) != 0) {
                Battle_ApplyDamage(0x10);
                status_item_3 = 0x10;
                goto consume_status_group3;
            }
            if (Inv_CountByValue(0x11) != 0) {
                Battle_ApplyDamage(0x11);
                status_item_3 = 0x11;
consume_status_group3:
                Inv_FindItemById(status_item_3);
                Scene_LoadRoomAssets(0x57, D_8009D254);
                Asset_Find08Alt(0x4B5, 0, D_8009D254->posX.parts.integer, D_8009D254->posY.parts.integer, (s32) D_8009D254->posZ.parts.integer);
            }
        }
        if (((*state_flags) & 0x1000) && (Inv_CountByValue(0x11) != 0)) {
            Battle_ApplyDamage(0x11);
            Inv_FindItemById(0x11);
            Scene_LoadRoomAssets(0x57, D_8009D254);
            Asset_Find08Alt(0x4B5, 0, D_8009D254->posX.parts.integer, D_8009D254->posY.parts.integer, (s32) D_8009D254->posZ.parts.integer);
        }
    }
}
