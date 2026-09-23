/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "common.h"
extern u8 D_8009D254[];
extern u8 *D_8009D1E0;
extern int D_8009D010;
extern int D_8009D018;
void Inv_BuildWeaponList(int, void *);
void Inv_BuildArmorList(void *);
int Menu_GetBattleEquipMode(void);
void Battle_DispatchSpecialAction(int);
void Inv_SelectActiveList(int);
void BattleCmd_LoadWeaponModifiers(void);
void MenuWidget_InitPool(void);
struct WeaponBlock { u32 word[6]; };
struct ArmorBlock { u32 word[2]; };
void Inv_SetActiveList(int mode, int *slot) {
    u8 *active;
    int *selection;
    u8 *entity;
    int value;
    int present;
    selection = slot;
    entity = *(u8 **)D_8009D254;
    active = 0;
    if (entity) {
        value = *(int *)entity;
        present = value != 0;
            active = (u8 *)(value & -present);
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
                *(struct WeaponBlock *)out = *(struct WeaponBlock *)(*(u8 **)(active+0x68));
                Inv_BuildWeaponList(0, D_8009D1E0);
            }
            if (Menu_GetBattleEquipMode()) D_8009D010 = 0x197;
            else Battle_DispatchSpecialAction(0x197);
        }
        break;
    case 3:
        if (active) {
            u8 *out = D_8009D1E0;
            if (out) {
                *(struct ArmorBlock *)out = *(struct ArmorBlock *)(*(u8 **)(active+0x6C));
                Inv_BuildArmorList(D_8009D1E0);
            }
            if (Menu_GetBattleEquipMode()) D_8009D010 = 0x198;
            else Battle_DispatchSpecialAction(0x198);
        }
        break;
    case 5:
        if (active) {
            register u8 *data asm("$5") = *(u8 **)(active + 0x68);
            u8 *item = *(u8 **)selection;
            register u32 word asm("$3") = *(u32 *)(data + 0xC);
            register int mask asm("$4") = -0x400;
            int kind;
            int encoded;
            register u32 flag_mask asm("$2");
            u32 flags2;
            register u32 shifted asm("$2");
            word = (word & mask) | (*(u16 *)(item + 0xA) & 0x3FF);
            *(u32 *)(data + 0xC) = word;
                    item = *(u8 **)selection;
            kind = item[6];
            data = *(u8 **)(active + 0x68);
            if (kind != 0 && (unsigned)kind < 8) {
                asm volatile("" ::: "memory");
                encoded = kind - 4;
                if (encoded <= 0) encoded = 1;
            } else {
                            item = *(u8 **)selection;
                kind = item[6];
                if ((unsigned)kind < 19) encoded = 0;
                else encoded = kind - 18;
            }
            flags2 = *(u32 *)(data+0xC);
            flag_mask = 0xFFCFFFFF;
            flags2 &= flag_mask;
            shifted = encoded & 3;
            shifted <<= 20;
            flags2 |= shifted;
            *(u32 *)(data+0xC) = flags2;
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
