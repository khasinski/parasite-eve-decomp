/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

extern int *g_InvSelectionBits;

void Inv_SetSelectionBit(int arg0) {
    g_InvSelectionBits[arg0 >> 5] |= 1 << (arg0 & 0x1F);
}
