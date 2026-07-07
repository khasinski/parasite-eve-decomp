/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

short *g_InvItemPtr;

short Inv_GetActiveListItem(int index) {
    return g_InvItemPtr[index];
}
