#include "pe1/inventory_sort.h"

int Inv_CompareItemsForSort(const short *leftIndex, const short *rightIndex) {
    ItemDataRecord *left;
    ItemDataRecord *right;
    int leftKind;
    int rightKind;
    int leftStat;
    int rightStat;
    int leftValue;
    int rightValue;

    leftValue = *leftIndex;
    rightValue = *rightIndex;
    if (g_InvSortLookup != Inv_LookupActiveListData) {
        if (leftValue == 0) {
            return rightValue > 0;
        }
        if (rightValue == 0) {
            return -1;
        }
    }

    left = g_InvSortLookup(leftValue);
    right = g_InvSortLookup(rightValue);
    leftKind = D_8009D0B8[left->kind];
    rightKind = D_8009D0B8[right->kind];
    leftStat = D_8009D0BC[((unsigned char *)left->bonusStats)[0] & 0xf];
    rightStat = D_8009D0BC[((unsigned char *)right->bonusStats)[0] & 0xf];

    if (left == 0) {
        if (right != 0) {
            return 1;
        }
    } else if (right == 0) {
        return -1;
    }
    if (rightKind < leftKind) {
        return 1;
    }
    if (leftKind < rightKind) {
        return -1;
    }
    if (rightStat < leftStat) {
        return 1;
    }
    if (leftStat < rightStat) {
        return -1;
    }
    if (right->itemId < left->itemId) {
        return 1;
    }
    return -(left->itemId < right->itemId);
}
