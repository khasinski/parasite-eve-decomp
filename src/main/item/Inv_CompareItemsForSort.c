/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "pe1/inventory_sort.h"

extern ItemDataRecord *(*g_InvLookupPtr)(short);

int Inv_CompareItemsForSort(const short *leftIndex, const short *rightIndex) {
    ItemDataRecord *left;
    ItemDataRecord *right;
    int leftKind;
    int rightKind;
    int leftSubtype;
    int rightSubtype;
    int leftName;
    int rightName;
    int leftItem;
    int rightItem;
    ItemDataRecord *(*defaultLookup)(int);
    ItemDataRecord *(*lookup)(int);
    defaultLookup = Inv_LookupActiveListData;
    asm volatile("" ::: "memory");
    leftItem = *leftIndex;
    rightItem = *rightIndex;
    lookup = g_InvLookupPtr;
    if (lookup != defaultLookup) {
        if (leftItem == 0) {
            goto left_item_is_empty;
        }
        if (rightItem == 0) {
            return -1;
        }
    }

    left = lookup(leftItem);
    right = g_InvLookupPtr(rightItem);
    leftKind = D_8009D0B8[left->kind];
    rightKind = D_8009D0B8[right->kind];
    leftSubtype = D_8009D0BC[((unsigned char *)left)[0xE] & 0xF];
    rightSubtype = D_8009D0BC[((unsigned char *)right)[0xE] & 0xF];

    if (left == 0) {
        goto left_record_is_empty;
    }
    if (right == 0) {
        return -1;
    }
    asm volatile("" ::: "memory");
    goto compare_keys;

left_record_is_empty:
    if (right != 0) {
        return 1;
    }

compare_keys:
    if (rightKind < leftKind) {
        return 1;
    }
    if (leftKind < rightKind) {
        return -1;
    }
    if (rightSubtype < leftSubtype) {
        return 1;
    }
    if (leftSubtype < rightSubtype) {
        return -1;
    }
    leftName = left->itemId;
    rightName = right->itemId;
    return (unsigned int)rightName < (unsigned int)leftName
               ? 1
               : -((unsigned int)leftName < (unsigned int)rightName);

left_item_is_empty:
    return rightItem != 0;
}
