/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "common.h"
#include "pe1/battle.h"
#include "pe1/inventory.h"

/* Active-list commands from the item menus and the pending command result
 * (D_8009D010) that the menu loop polls and clears. */

extern BattleEntity *D_8009D254[];
extern u8 *D_8009D1E0;
extern int D_8009D010;
#include "pe1/battle_modifiers.h"
int Menu_GetBattleEquipMode(void);
void Battle_DispatchSpecialAction(int);
void MenuWidget_InitPool(void);

void Inv_SetActiveList(int mode, int *slot) {
    Combatant *active;
    int *selection;
    BattleEntity *entity;
    int value;
    int present;
    selection = slot;
    entity = D_8009D254[0];
    active = 0;
    if (entity) {
        value = (int)entity->core;
        present = value != 0;
            active = (Combatant *)(value & -present);
    }
    switch (mode) {
    case 0:
        D_8009D010 = *selection + 3;
        break;
    case 1:
        D_8009D010 = *selection + 0x183;
        break;
    case 2:
        if (active) {
            u8 *out = D_8009D1E0;
            if (out) {
                *(BattleAction *)out = *(BattleAction *)(active->action);
                Inv_BuildWeaponList(0, (WeaponListOutput *)D_8009D1E0);
            }
            if (Menu_GetBattleEquipMode()) D_8009D010 = 0x197;
            else Battle_DispatchSpecialAction(0x197);
        }
        break;
    case 3:
        if (active) {
            u8 *out = D_8009D1E0;
            if (out) {
                *(BattleAttributes *)out = *active->attributes;
                Inv_BuildArmorList((BattleAttributes *)D_8009D1E0);
            }
            if (Menu_GetBattleEquipMode()) D_8009D010 = 0x198;
            else Battle_DispatchSpecialAction(0x198);
        }
        break;
    case 5:
        if (active) {
            register BattleAction *data asm("$5") = active->action;
            ItemDataRecord *item = *(ItemDataRecord **)selection;
            register u32 word asm("$3") = data->attackWord;
            register int mask asm("$4") = -0x400;
            int kind;
            int encoded;
            u32 flags2;
            register u32 shifted asm("$2");
            word = (word & mask) | (item->ammo & 0x3FF);
            data->attackWord = word;
                    item = *(ItemDataRecord **)selection;
            kind = item->kind;
            data = active->action;
            if (kind != 0 && (unsigned)kind < 8) {
                asm volatile("" ::: "memory");
                encoded = kind - 4;
                if (encoded <= 0) encoded = 1;
            } else {
                            item = *(ItemDataRecord **)selection;
                kind = item->kind;
                if ((unsigned)kind < 19) encoded = 0;
                else encoded = kind - 18;
            }
            flags2 = data->attackWord;
            shifted = 0xFFCFFFFF;
            flags2 &= shifted;
            shifted = encoded & 3;
            shifted <<= 20;
            flags2 |= shifted;
            data->attackWord = flags2;
        }
        break;
    case 8: D_8009D010 = 0x199; break;
    case 9: D_8009D010 = -1; break;
    case 10: D_8009D010 = 1000; break;
    case 11: D_8009D010 = 1; break;
    case 12:
        D_8009D018 = 4;
        Inv_SelectActiveList(0);
        BattleCmd_LoadWeaponModifiers();
        D_8009D010 = 2;
        break;
    default: break;
    }
    if (D_8009D010) MenuWidget_InitPool();
}

int Menu_GetCommandResult(void) {
    return D_8009D010;
}

void Menu_ClearCommandResult(void) {
    D_8009D010 = 0;
}
