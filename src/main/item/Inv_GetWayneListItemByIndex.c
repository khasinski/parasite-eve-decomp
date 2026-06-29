/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int g_InvTransferItemCount;
short g_InvTransferItemList[];

int Inv_GetWayneListItemByIndex(int arg0) {
    if (arg0 >= 0 && arg0 < g_InvTransferItemCount) {
        return g_InvTransferItemList[arg0];
    }
    return 0;
}
