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
