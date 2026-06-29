/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int *g_InvSelectionBits;

int Inv_TestSelectionBit(int arg0) {
    int word;
    int mask;

    word = g_InvSelectionBits[arg0 >> 5];
    mask = 1 << (arg0 & 0x1F);
    return (word & mask) != 0;
}
