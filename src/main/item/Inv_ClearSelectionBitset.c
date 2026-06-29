/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int *g_InvSelectionBits;
int g_InvSelectionBitWords;

void Inv_ClearSelectionBitset(void) {
    int i;

    for (i = 0; i < g_InvSelectionBitWords; i++) {
        g_InvSelectionBits[i] = 0;
    }
}
