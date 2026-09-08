/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "pe1/inventory.h"

extern s32 D_8009D0A0;
extern ItemDataRecord *(*g_InvLookupPtr)(s16) __asm__("D_8009D0B4");

int Inv_CompareItemsByStatForSort(s16 *leftIndex, s16 *rightIndex)
{
    ItemDataRecord *left;
    ItemDataRecord *right;
    int leftValue;
    int rightValue;
    int leftName;
    int rightName;

    leftValue = 0;
    rightValue = 0;
    left = g_InvLookupPtr(*leftIndex);
    right = g_InvLookupPtr(*rightIndex);

    switch (D_8009D0A0) {
    case 0:
        leftValue = left->baseStats[0] + left->bonusStats[0];
        rightValue = right->baseStats[0] + right->bonusStats[0];
        break;
    case 1:
        leftValue = left->baseStats[1] + left->bonusStats[1];
        rightValue = right->baseStats[1] + right->bonusStats[1];
        break;
    case 2:
        leftValue = left->baseStats[2] + left->bonusStats[2];
        rightValue = right->baseStats[2] + right->bonusStats[2];
        break;
    }

    if (rightValue < leftValue) {
        return -1;
    }
    if (leftValue < rightValue) {
        return 1;
    }

    leftName = left->itemId;
    rightName = right->itemId;
    return (unsigned int)rightName < (unsigned int)leftName
               ? 1
               : -((unsigned int)leftName < (unsigned int)rightName);
}

#include "common.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
M2C_UNK qsort();
extern s32 D_8009D0A0;
extern s32 D_8009D0A4;
extern u16 *g_InvSortListBase;
extern s32 g_InvSortListCount;
extern ItemDataRecord *(*g_InvLookupPtr)(s16);

void Inv_SortWeaponSubrange(void) {
    u16 *var_s0;
    u16 *var_s1;
    s32 mask;
    s32 var_a0;
    u16 tmpu;

    var_s0 = g_InvSortListBase;
    D_8009D0A0 = D_8009D0A4;
    mask = 0x1FE;
    if (var_s0 < g_InvSortListBase + g_InvSortListCount) {
        while (*(s16 *) var_s0 != 0) {
            if ((mask >> g_InvLookupPtr((s16)*var_s0)->kind) & 1) {
                break;
            }
            var_s0 += 1;
            if (!(var_s0 < g_InvSortListBase + g_InvSortListCount)) {
                return;
            }
        }
        if (var_s0 < g_InvSortListBase + g_InvSortListCount) {
            tmpu = *var_s0;
            var_s1 = var_s0;
            if (*(s16 *) var_s0 != 0) {
                do {
                    var_a0 = tmpu << 0x10;
                    if (!((mask >> g_InvLookupPtr((s16)(var_a0 >> 0x10))->kind) & 1)) {
                        break;
                    }
                    var_s0 += 1;
                    if (!(var_s0 < g_InvSortListBase + g_InvSortListCount)) {
                        break;
                    }
                    tmpu = *var_s0;
                } while (*(s16 *) var_s0 != 0);
                qsort(var_s1, (s32) (var_s0 - var_s1), 2, &Inv_CompareItemsByStatForSort);
            }
        }
    }
}

#include "common.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
M2C_UNK qsort();
extern s32 D_8009D0A0;
extern s32 D_8009D0A8;
extern u16 *g_InvSortListBase;
extern s32 g_InvSortListCount;
extern ItemDataRecord *(*g_InvLookupPtr)(s16);

void Inv_SortAmmoSubrange(void) {
    u16 *var_s0;
    u16 *var_s1;
    s32 mask;
    s32 var_a0;
    u16 tmpu;

    var_s0 = g_InvSortListBase;
    D_8009D0A0 = D_8009D0A8;
    mask = 0x200;
    if (var_s0 < g_InvSortListBase + g_InvSortListCount) {
        while (*(s16 *) var_s0 != 0) {
            if ((mask >> g_InvLookupPtr((s16)*var_s0)->kind) & 1) {
                break;
            }
            var_s0 += 1;
            if (!(var_s0 < g_InvSortListBase + g_InvSortListCount)) {
                return;
            }
        }
        if (var_s0 < g_InvSortListBase + g_InvSortListCount) {
            tmpu = *var_s0;
            var_s1 = var_s0;
            if (*(s16 *) var_s0 != 0) {
                do {
                    var_a0 = tmpu << 0x10;
                    if (!((mask >> g_InvLookupPtr((s16)(var_a0 >> 0x10))->kind) & 1)) {
                        break;
                    }
                    var_s0 += 1;
                    if (!(var_s0 < g_InvSortListBase + g_InvSortListCount)) {
                        break;
                    }
                    tmpu = *var_s0;
                } while (*(s16 *) var_s0 != 0);
                qsort(var_s1, (s32) (var_s0 - var_s1), 2, &Inv_CompareItemsByStatForSort);
            }
        }
    }
}

#include "common.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
s32 Inv_GetAyaSlotLimit();
M2C_UNK Inv_RebuildSelectableMask();
void Inv_SortWeaponSubrange(void);
void Inv_SortAmmoSubrange(void);
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
extern u16 *g_InvSortListBase;
extern s32 g_InvSortListCount;
extern M2C_UNK *g_InvSortRankTable;
extern M2C_UNK *g_InvSortTypeRankTable;
extern s8 g_AyaEquippedWeaponSlot[];
#define g_AyaEquippedWeaponSlot (g_AyaEquippedWeaponSlot[0])
extern s8 g_AyaEquippedArmorSlot[];
#define g_AyaEquippedArmorSlot (g_AyaEquippedArmorSlot[0])
ItemDataRecord *Inv_LookupData(unsigned int index);
int Inv_CompareItemsForSort(const s16 *leftIndex, const s16 *rightIndex);
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

/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

extern M2C_UNK D_80092410[];
#define D_80092410 (D_80092410[0])
extern u16 g_WayneStorageItems[];
void Inv_TransferItemAlt2(int arg0);

void Sort_ContainerItems(int arg0) {
    if (arg0 != 0) {
        g_InvSortRankTable = &g_InvSortRankTableA;
        g_InvSortTypeRankTable = &g_InvTypeRankTableA;
    } else {
        g_InvSortRankTable = &D_80092410;
        g_InvSortTypeRankTable = &g_InvTypeRankTableB;
    }

    g_InvSortListBase = g_WayneStorageItems;
    g_InvSortListCount = 0x64;
    g_InvLookupPtr = Inv_LookupData;
    qsort(g_WayneStorageItems, 0x64, 2, Inv_CompareItemsForSort);
    Inv_SortWeaponSubrange();
    Inv_SortAmmoSubrange();
    Inv_SortInventoryByMode(2, arg0);
    Inv_TransferItemAlt2(0xF400);
}
