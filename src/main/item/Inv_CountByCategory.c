/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int g_InvSlotLimit;

int Inv_GetActiveListItemType(int arg0);

int Inv_CountByCategory(int arg0) {
    int index;
    int count;

    index = 0;
    count = 0;
    while (index < g_InvSlotLimit) {
        if (Inv_GetActiveListItemType(index) == arg0) {
            count++;
        }
        index++;
    }

    return count;
}
