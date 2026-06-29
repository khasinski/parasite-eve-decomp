/* CC1_FLAGS: -g3 -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
s32 func_80052F70();
M2C_UNK Inv_RebuildSelectableMask();
M2C_UNK Inv_SortWeaponSubrange();
M2C_UNK Inv_SortAmmoSubrange();
M2C_UNK qsort();
M2C_UNK Inv_RebuildSelectableMask();
extern struct { char _[16]; } D_800923F8_o __asm__("g_InvSortRankTableA");
#define g_InvSortRankTableA (*(M2C_UNK *)&D_800923F8_o)
extern struct { char _[16]; } D_80092428_o __asm__("D_80092428");
#define D_80092428 (*(M2C_UNK *)&D_80092428_o)
extern struct { char _[16]; } D_80092440_o __asm__("D_80092440");
#define D_80092440 (*(M2C_UNK *)&D_80092440_o)
extern struct { char _[16]; } D_80092458_o __asm__("g_InvTypeRankTableA");
#define g_InvTypeRankTableA (*(M2C_UNK *)&D_80092458_o)
extern struct { char _[16]; } D_80092468_o __asm__("g_InvTypeRankTableB");
#define g_InvTypeRankTableB (*(M2C_UNK *)&D_80092468_o)
extern s16 *g_InvItemPtr;
extern s32 g_InvSlotLimit;
extern M2C_UNK *g_InvSelectionBits;
extern struct { char _[16]; } D_8009D05C_o __asm__("g_AyaItemSelectionBits");
#define g_AyaItemSelectionBits (*(M2C_UNK *)&D_8009D05C_o)
extern s32 g_InvSelectionBitWords;
extern s32 D_8009D0A4;
extern s32 D_8009D0A8;
extern s16 *g_InvSortListBase;
extern s32 g_InvSortListCount;
extern M2C_UNK *g_InvLookupPtr;
extern M2C_UNK *g_InvSortRankTable;
extern M2C_UNK *g_InvSortTypeRankTable;
extern struct { char _[16]; } D_800C0E20_o __asm__("g_AyaEquippedWeaponSlot");
#define g_AyaEquippedWeaponSlot (*(s8 *)&D_800C0E20_o)
extern struct { char _[16]; } D_800C0E22_o __asm__("g_AyaEquippedArmorSlot");
#define g_AyaEquippedArmorSlot (*(s8 *)&D_800C0E22_o)
extern struct { char _[16]; } Inv_LookupData_o __asm__("Inv_LookupData");
#define Inv_LookupData (*(M2C_UNK *)&Inv_LookupData_o)
extern struct { char _[16]; } func_8005AFFC_o __asm__("Inv_CompareItemsForSort");
#define Inv_CompareItemsForSort (*(M2C_UNK *)&func_8005AFFC_o)
extern struct { char _[16]; } g_AyaInventoryItems_o __asm__("g_AyaInventoryItems");
#define g_AyaInventoryItems (*(s16 *)&g_AyaInventoryItems_o)

void Inv_SortInventoryByMode(s32 arg0, s32 arg1) {
    s16 *var_a0;
    s16 *var_a0_2;
    s32 temp_s1;
    s32 var_s0;
    s32 var_v1;
    s32 var_v1_2;

    switch (arg0) {
    case 0:
        g_InvSortRankTable = &D_80092428;
        D_8009D0A8 = arg1;
        break;
    case 1:
        g_InvSortRankTable = &g_InvSortRankTableA;
        D_8009D0A4 = arg1;
        break;
    case 2:
        g_InvSortRankTable = &D_80092440;
        if (arg1 != 0) {
            g_InvSortTypeRankTable = &g_InvTypeRankTableA;
        } else {
            g_InvSortTypeRankTable = &g_InvTypeRankTableB;
        }
        break;
    }
    g_InvItemPtr = &g_AyaInventoryItems;
    g_InvSlotLimit = func_80052F70();
    g_InvSelectionBits = &g_AyaItemSelectionBits;
    g_InvSelectionBitWords = 2;
    temp_s1 = g_InvItemPtr[g_AyaEquippedWeaponSlot];
    if (g_AyaEquippedArmorSlot >= 0) {
        var_s0 = g_InvItemPtr[g_AyaEquippedArmorSlot];
    } else {
        var_s0 = -1;
    }
    g_InvLookupPtr = &Inv_LookupData;
    g_InvSortListBase = g_InvItemPtr;
    g_InvSortListCount = g_InvSlotLimit;
    qsort(g_InvItemPtr, g_InvSlotLimit, 2, &Inv_CompareItemsForSort);
    Inv_SortWeaponSubrange();
    Inv_SortAmmoSubrange();
    for (var_v1 = 0; var_v1 < g_InvSlotLimit; var_v1++) {
        if (g_InvItemPtr[var_v1] == temp_s1) {
            break;
        }
    }
    if (var_v1 < g_InvSlotLimit) {
        g_AyaEquippedWeaponSlot = var_v1;
    }
    if (var_s0 >= 0) {
        for (var_v1_2 = 0; var_v1_2 < g_InvSlotLimit; var_v1_2++) {
            if (g_InvItemPtr[var_v1_2] == var_s0) {
                break;
            }
        }
        if (var_v1_2 < g_InvSlotLimit) {
            g_AyaEquippedArmorSlot = var_v1_2;
        }
    }
    Inv_RebuildSelectableMask();
}
