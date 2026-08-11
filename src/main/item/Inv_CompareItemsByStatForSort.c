#include "pe1/inventory_sort.h"

int Inv_CompareItemsByStatForSort(const short *leftIndex,
                                  const short *rightIndex) {
    ItemDataRecord *left;
    ItemDataRecord *right;
    int leftStat;
    int rightStat;

    leftStat = 0;
    rightStat = 0;
    left = g_InvSortLookup(*leftIndex);
    right = g_InvSortLookup(*rightIndex);

    switch (g_InvSortStatIndex) {
    case 0:
        leftStat = left->baseStats[0] + left->bonusStats[0];
        rightStat = right->baseStats[0] + right->bonusStats[0];
        break;
    case 1:
        leftStat = left->baseStats[1] + left->bonusStats[1];
        rightStat = right->baseStats[1] + right->bonusStats[1];
        break;
    case 2:
        leftStat = left->baseStats[2] + left->bonusStats[2];
        rightStat = right->baseStats[2] + right->bonusStats[2];
        break;
    }

    if (rightStat < leftStat) {
        return -1;
    }
    if (leftStat < rightStat) {
        return 1;
    }
    if (right->itemId < left->itemId) {
        return 1;
    }
    return -(left->itemId < right->itemId);
}
