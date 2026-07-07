/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */

int g_InvPackedListCount;
short g_InvPackedList[];

int *g_InvSelectionBits;
int g_InvSelectionBitWords;

int Inv_GetPackedListItem(int arg0) {
    if (arg0 >= 0 && arg0 < g_InvPackedListCount) {
        return g_InvPackedList[arg0];
    }
    return 0;
}

void Inv_ClearSelectionBitset(void) {
    int i;

    for (i = 0; i < g_InvSelectionBitWords; i++) {
        g_InvSelectionBits[i] = 0;
    }
}
