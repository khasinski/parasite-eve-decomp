/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int g_InvPackedListCount;
short g_InvPackedList[];

int Inv_GetPackedListItem(int arg0) {
    if (arg0 >= 0 && arg0 < g_InvPackedListCount) {
        return g_InvPackedList[arg0];
    }
    return 0;
}
