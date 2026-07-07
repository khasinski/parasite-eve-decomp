/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
s32 Inv_GetAyaSlotLimit();
M2C_UNK Inv_RebuildSelectableMask();
s32 Inv_IsSlotSelectable();
s32 Inv_CanAddActiveListItemToAya();
void *Item_LookupBaseData();
extern M2C_UNK *g_InvItemPtr;
extern s32 g_InvSlotLimit;
extern M2C_UNK *g_InvSelectionBits;
extern struct { char _[16]; } D_8009D05C_o __asm__("g_AyaItemSelectionBits");
#define g_AyaItemSelectionBits (*(M2C_UNK *)&D_8009D05C_o)
extern s32 g_InvSelectionBitWords;
extern struct { char _[16]; } D_800A1FD4_o __asm__("g_BattleCountTable");
#define g_BattleCountTable (*(M2C_UNK *)&D_800A1FD4_o)
extern struct { char _[16]; } D_800C0E20_o __asm__("g_AyaEquippedWeaponSlot");
extern struct { char _[16]; } D_800C0E20_w __asm__("g_AyaEquippedWeaponSlot");
extern struct { char _[16]; } D_800C0E20_r2 __asm__("g_AyaEquippedWeaponSlot");
extern struct { char _[16]; } D_800C0E20_w2 __asm__("g_AyaEquippedWeaponSlot");
#define g_AyaEquippedWeaponSlot (*(s8 *)&D_800C0E20_o)
extern struct { char _[16]; } D_800C0E22_o __asm__("g_AyaEquippedArmorSlot");
extern struct { char _[16]; } D_800C0E22_w __asm__("g_AyaEquippedArmorSlot");
extern struct { char _[16]; } D_800C0E22_r2 __asm__("g_AyaEquippedArmorSlot");
extern struct { char _[16]; } D_800C0E22_w2 __asm__("g_AyaEquippedArmorSlot");
#define g_AyaEquippedArmorSlot (*(s8 *)&D_800C0E22_o)
extern struct { char _[16]; } g_AyaInventoryItems_o __asm__("g_AyaInventoryItems");
#define g_AyaInventoryItems (*(M2C_UNK *)&g_AyaInventoryItems_o)

s32 Inv_TransferItem(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    register M2C_UNK *var_v0 asm("$2");
    s16 *temp_s0;
    s32 var_a0;
    s32 var_a0_2;
    s32 var_a0_3;
    s32 var_s4;
    u16 *temp_a0;
    u16 *temp_a0_2;
    u16 *temp_a1;
    register u16 *var_a1 asm("$5");
    u32 temp_v0;
    s8 te20;
    s8 te22;
    register u8 *tb48 asm("$2");
    register s32 ta1m asm("$5");
    s32 ta0m;
    register u32 tl1 asm("$2");
    u32 tl0;
    register s32 tsm asm("$2");
    register u8 *tla asm("$2");
    register s32 tam2 asm("$5");
    register u8 *tb48c asm("$2");
    u32 temp_v0_2;
    register u32 tl1b asm("$2");
    u32 tl0b;
    u32 temp_v1;
    u32 temp_v1_2;

    g_InvItemPtr = &g_AyaInventoryItems;
    var_s4 = 1;
    g_InvSlotLimit = Inv_GetAyaSlotLimit();
    g_InvSelectionBits = &g_AyaItemSelectionBits;
    g_InvSelectionBitWords = 2;
    if (arg0 == 0xD) {
        if (arg2 == arg0) {
            var_v0 = (M2C_UNK *)&D_800A1FD4_o;
            var_a1 = (u16 *)((u8 *)&D_800A1FD4_o + (arg1 * 2));
            var_a0 = arg3 * 2;
            goto block_33;
        }
        goto block_4;
    }
block_4:
    if (arg0 == 0xE) {
        if (arg2 == arg0) {
            ta1m = arg1 * 2;
            tb48 = (u8 *)g_InvItemPtr;
            ta0m = arg3 * 2;
            temp_a1 = (u16 *)((u32)ta1m + (u32)tb48);
            temp_a0 = (u16 *)((u32)ta0m + (u32)tb48);
            tl1 = *temp_a1;
            tl0 = *temp_a0;
            temp_v0 = tl1 ^ tl0;
            *temp_a1 = temp_v0;
            temp_v1 = *temp_a0 ^ temp_v0;
            *temp_a0 = temp_v1;
            *temp_a1 ^= temp_v1;
            te20 = g_AyaEquippedWeaponSlot;
            if (te20 == arg1) {
                (*(s8 *)&D_800C0E20_w) = arg3;
            } else if (te20 == arg3) {
                (*(s8 *)&D_800C0E20_w2) = arg1;
            }
            te22 = g_AyaEquippedArmorSlot;
            if (te22 == arg1) {
                (*(s8 *)&D_800C0E22_w) = arg3;
            } else if (te22 == arg3) {
                (*(s8 *)&D_800C0E22_w2) = arg1;
            }
            goto block_34;
        }
        goto block_15;
    }
block_15:
    if (arg0 == 0xD) {
        if (Inv_IsSlotSelectable(arg3) != 0) {
            var_a0_2 = 0;
            if ((g_InvItemPtr == &g_AyaInventoryItems) && ((g_AyaEquippedWeaponSlot == arg3) || (g_AyaEquippedArmorSlot == arg3))) {
                var_a0_2 = 1;
            }
            if (var_a0_2 == 0) {
                tsm = arg1 * 2;
                temp_s0 = (s16 *)((u32)tsm + (u32)(u8 *)&D_800A1FD4_o);
                if (((u8) M2C_FIELD(Item_LookupBaseData(*temp_s0 - 1), u8 *, 6) >= 0x10U) && ((u8) M2C_FIELD(Item_LookupBaseData(*temp_s0 - 1), u8 *, 6) < 0x13U)) {
                    var_s4 = Inv_CanAddActiveListItemToAya(arg1) == 0;
                } else {
                    tla = (u8 *)&D_800A1FD4_o;
                    tam2 = arg1 * 2;
                    var_a1 = (u16 *)((u32)tam2 + (u32)tla);
                    var_v0 = g_InvItemPtr;
                    var_a0 = arg3 * 2;
                    goto block_33;
                }
                goto block_34;
            }
            goto block_35;
        }
        goto block_35;
    }
    if (Inv_IsSlotSelectable(arg1) != 0) {
        var_a0_3 = 0;
        if ((g_InvItemPtr == &g_AyaInventoryItems) && ((g_AyaEquippedWeaponSlot == arg1) || (g_AyaEquippedArmorSlot == arg1))) {
            var_a0_3 = 1;
        }
        if (var_a0_3 == 0) {
            tam2 = arg1 * 2;
            tb48c = (u8 *)g_InvItemPtr;
            var_a0 = arg3 * 2;
            var_a1 = (u16 *)((u32)tam2 + (u32)tb48c);
            var_v0 = (M2C_UNK *)&D_800A1FD4_o;
block_33:
            temp_a0_2 = (u16 *)((u32)var_a0 + (u32)var_v0);
            tl1b = *var_a1;
            tl0b = *temp_a0_2;
            temp_v0_2 = tl1b ^ tl0b;
            *var_a1 = temp_v0_2;
            temp_v1_2 = *temp_a0_2 ^ temp_v0_2;
            *temp_a0_2 = temp_v1_2;
            *var_a1 ^= temp_v1_2;
block_34:
            Inv_RebuildSelectableMask();
            return var_s4;
        }
        goto block_35;
    }
block_35:
    var_s4 = 0;
    __asm__("" : "=r"(var_s4) : "0"(var_s4));
    return var_s4;
}
