/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

typedef signed char s8;
typedef unsigned char u8;
typedef unsigned short u16;

extern int D_8009CF3C;
extern struct { char _[16]; } g_AyaEquippedWeaponSlot_obj __asm__("D_800C0E20");
extern struct { char _[16]; } g_AmmoLoaded_obj __asm__("D_800C0E06");
extern struct { char _[16]; } g_AmmoCapacity_obj __asm__("D_800C0E08");

#define g_AyaEquippedWeaponSlot (*(s8 *)&g_AyaEquippedWeaponSlot_obj)
#define g_AmmoLoaded (*(u16 *)&g_AmmoLoaded_obj)
#define g_AmmoCapacity (*(u16 *)&g_AmmoCapacity_obj)
#define U16_AT(ptr, offset) (*(u16 *)((u8 *)(ptr) + (offset)))

int BattleCmd_GetRemainingAmmo(int *out);
u8 *Item_LookupBaseData(unsigned int index);
int Menu_GetBattleEquipMode(void);
int Inv_GetSlotHighlightState(int type, int fallback);
int Battle_GetEnemyEscapeFlag(void);
u8 *Inv_LookupActiveListData(int index);
int Inv_GetWeaponCategoryAmmoBase(unsigned int arg0);

int Menu_CheckItemAffordable(int item) {
    register int item_reg asm("$17");
    register int ammo_remaining asm("$18");
    register int mode_mask asm("$19");
    register int enabled asm("$16");
    u8 *data;
    int ammo_max;
    int battle_allowed;
    int type;

    item_reg = item;
    ammo_remaining = BattleCmd_GetRemainingAmmo(&ammo_max);
    data = Item_LookupBaseData(item_reg + 0xEB);
    mode_mask = data[5] >> Menu_GetBattleEquipMode();

    if (item_reg == 5) {
        if (ammo_remaining < (ammo_max / 3)) {
            enabled = 0;
        } else {
            enabled = mode_mask & 1;
        }
    } else {
        if (item_reg == 6) {
            if (ammo_remaining > 0) {
                enabled = mode_mask & 1;
            } else {
                enabled = 0;
            }
        } else if (item_reg == 0x13) {
            if (ammo_remaining < ammo_max) {
                enabled = 0;
            } else {
                enabled = mode_mask & 1;
            }
        } else if (ammo_remaining < Inv_GetSlotHighlightState(item_reg, 0)) {
            enabled = 0;
        } else {
            enabled = mode_mask & 1;
        }
    }

    if (D_8009CF3C != 0) {
        battle_allowed = 0;
        if ((unsigned int)(item_reg - 6) < 3) {
            battle_allowed = 0;
        } else if (item_reg == 0xA) {
            battle_allowed = 0;
        } else if (item_reg == 0x13) {
            battle_allowed = 0;
        } else {
            battle_allowed = 1;
        }
    } else {
        battle_allowed = 1;
    }
    enabled &= battle_allowed;

    if (item_reg == 5 && Battle_GetEnemyEscapeFlag() != 0) {
        enabled = 0;
    }

    if (item_reg == 6) {
        data = Inv_LookupActiveListData(g_AyaEquippedWeaponSlot);
        type = data[6];
        if (type != 8) {
            if (type == 0 || type >= 8) {
                if (type < 0x13) {
                    type = 0;
                } else {
                    type -= 0x12;
                }
            } else {
                type -= 4;
                if (type <= 0) {
                    type = 1;
                }
            }

            if (type == 1 || type == 3) {
                if (U16_AT(data, 0xA) + Inv_GetWeaponCategoryAmmoBase(type - 1) > 0) {
                    enabled &= 1;
                } else {
                    enabled = 0;
                }
            } else {
                enabled = 0;
            }
        } else {
            enabled = 0;
        }
    }

    if (item_reg < 3 || item_reg == 0x12) {
        if (g_AmmoCapacity >= g_AmmoLoaded) {
            enabled = 0;
        }
    }

    return enabled;
}
