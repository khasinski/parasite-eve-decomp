/* CC1_FLAGS: -g3 -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
s32 Inv_GetAyaSlotLimit();
M2C_UNK Inv_RebuildSelectableMask();
M2C_UNK Inv_SortWeaponSubrange();
M2C_UNK Inv_SortAmmoSubrange();
M2C_UNK qsort();
M2C_UNK Inv_RebuildSelectableMask();
extern M2C_UNK g_InvSortRankTableA[];
#define g_InvSortRankTableA (g_InvSortRankTableA[0])
extern M2C_UNK D_80092428[];
#define D_80092428 (D_80092428[0])
extern M2C_UNK D_80092440[];
#define D_80092440 (D_80092440[0])
extern M2C_UNK g_InvTypeRankTableA[];
#define g_InvTypeRankTableA (g_InvTypeRankTableA[0])
extern M2C_UNK g_InvTypeRankTableB[];
#define g_InvTypeRankTableB (g_InvTypeRankTableB[0])
extern s16 *g_InvItemPtr;
extern s32 g_InvSlotLimit;
extern M2C_UNK *g_InvSelectionBits;
extern M2C_UNK g_AyaItemSelectionBits[];
#define g_AyaItemSelectionBits (g_AyaItemSelectionBits[0])
extern s32 g_InvSelectionBitWords;
extern s32 D_8009D0A4;
extern s32 D_8009D0A8;
extern s16 *g_InvSortListBase;
extern s32 g_InvSortListCount;
extern M2C_UNK *g_InvLookupPtr;
extern M2C_UNK *g_InvSortRankTable;
extern M2C_UNK *g_InvSortTypeRankTable;
extern s8 g_AyaEquippedWeaponSlot[];
#define g_AyaEquippedWeaponSlot (g_AyaEquippedWeaponSlot[0])
extern s8 g_AyaEquippedArmorSlot[];
#define g_AyaEquippedArmorSlot (g_AyaEquippedArmorSlot[0])
extern M2C_UNK Inv_LookupData[];
#define Inv_LookupData (Inv_LookupData[0])
extern M2C_UNK Inv_CompareItemsForSort[];
#define Inv_CompareItemsForSort (Inv_CompareItemsForSort[0])
extern s16 g_AyaInventoryItems[];
#define g_AyaInventoryItems (g_AyaInventoryItems[0])

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
    g_InvSlotLimit = Inv_GetAyaSlotLimit();
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
