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
